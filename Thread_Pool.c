/* 
* Author:		xusongqi@live.com
* 
* Created Time: 2014年04月13日 星期日 14时02分25秒
* 
* FileName:     Threads_Pool.c
* 
* Description:  编译时加后缀: -lpthread
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>

/*任务队列元素*/
struct job
{
	void * (*callback_function)(void *arg);	//线程回调函数
	void * arg;		//回调函数的参数
	struct job * next;
};

/*进程池结构体*/
struct threadpool
{
	int thread_num;			//线程池中开启的线程数
	struct job *head;		//指向job的头指针，任务队列的头
	struct job *tail;		//指向job的尾指针，任务队列的尾
	pthread_t *pthreads;	//线程池中所有线程的pthread_t存储空间头地址
	pthread_mutex_t mutex;	//互斥锁
	pthread_cond_t queue_empty;			//队列为空的条件变量
	pthread_cond_t queue_not_empty;		//队列非空的条件变量
	pthread_cond_t queue_not_full;		//队列非满的条件变量
	int queue_max_num;		//任务队列的最大长度
	int queue_cur_num;		//任务队列的当前长度
	int queue_close;		//任务队列是否关闭 0：开启；1：关闭
	int pool_close;			//线程池是否关闭   0：开启；1：关闭
};

struct threadpool * threadpool_init(int thread_num, int queue_max_num);
int	threadpool_add_job(struct threadpool * pool, void * (*callback_function)(void * arg), void * arg);
int threadpool_destroy(struct threadpool * pool);
void * threadpool_function(void * pool_arg);

struct threadpool * threadpool_init(int thread_num, int queue_max_num)
{
	struct threadpool * pool= NULL;
	do//循环的目的只是为了初始化时，若任意if初始化失败，直接跳过下面的初始化
	{
		/*申请线程池空间*/
		pool = malloc(sizeof(struct threadpool));
		if(NULL == pool)
		{
			printf("Failed to malloc threadpool!\n");
			break;
		}

		/*初始化线程池参数*/
		pool->thread_num = thread_num;
		pool->queue_max_num = queue_max_num;
		pool->queue_cur_num = 0;
		pool->head = NULL;
		pool->tail = NULL;

		/*初始化条件变量*/
		if( pthread_mutex_init(&(pool->mutex), NULL) )
		{
			printf("Failed to init mutex!\n");
			break;
		}

		if( pthread_cond_init(&(pool->queue_empty), NULL) )
		{
			printf("Failed to init queue_empty!\n");
			break;
		}

		if( pthread_cond_init(&(pool->queue_not_empty), NULL) )
		{
			printf("Failed to init queue_not_empty!\n");
			break;
		}

		if( pthread_cond_init(&(pool->queue_not_full), NULL) )
		{
			printf("Failed to init queue_not_full!\n");
			break;
		}

		/*开辟存储所有线程的pthread_t的空间*/
		pool->pthreads = malloc(sizeof(pthread_t) * thread_num);
		if(NULL == pool->pthreads)
		{
			printf("Failed to malloc pthreads!\n");
			break;
		}

		/*初始化任务队列信号量为开启，线程池信号量为开启*/
		pool->queue_close = 0;
		pool->pool_close = 0;
		
		/*初始化所有线程*/
		int i;
		for(i = 0; i < pool->thread_num; i++)
		{
			pthread_create(&(pool->pthreads[i]), NULL, threadpool_function, (void *)pool);
		}

		return pool;
	}while(0);

	return NULL;
}

/*将任务添加到任务队列中的函数*/
int threadpool_add_job(struct threadpool *pool, void * (*callback_function)(void * arg), void * arg)
{
	assert(NULL != pool);
	assert(NULL != callback_function);
	assert(NULL != arg);

	pthread_mutex_lock(&(pool->mutex));//对线程加锁
	//如果任务队列已满，将进程阻塞直到任务队列非满
	while((pool->queue_cur_num == pool->queue_max_num) && !(pool->queue_close || pool->pool_close))
	{
		pthread_cond_wait(&(pool->queue_not_full),&(pool->mutex));
	}

	if(pool->queue_close || pool->pool_close)//任务队列关闭或线程池关闭，则退出
	  {
		  pthread_mutex_unlock(&(pool->mutex));
		  return -1;
	  }

	//开辟一个新任务空间pjob存储当前任务
	struct job * pjob = (struct job *)malloc(sizeof(struct job));
	if(NULL == pjob)
	{
		pthread_mutex_unlock(&(pool->mutex));
		return -1;
	}

	//将任务存储到新的任务空间pjob
	pjob->callback_function = callback_function;
	pjob->arg = arg;
	pjob->next = NULL;

	//将任务插到任务队列
	if(NULL == pool->head)
	{
		pool->head = pool->tail = pjob;
		pthread_cond_broadcast(&(pool->queue_not_empty));	//队列为空时，当有任务到来，则通知线程池中的线程：队列非空
	}
	else
	{
		pool->tail->next = pjob;
		pool->tail = pjob;
	}
	pool->queue_cur_num++;
	pthread_mutex_unlock(&(pool->mutex));
	return 0;
}

/*线程工作入口函数*/
void * threadpool_function(void * pool_arg)
{
	struct threadpool * pool = (struct threadpool *)pool_arg; //获取线程池地址
	struct job *pjob = NULL;		//初始化为NULL，后面会用到
	
	while(1)//循环从任务队列中提取任务
	{
		pthread_mutex_lock(&(pool->mutex));
		//任务队列为空时，线程等待队列非空
		while((pool->queue_cur_num == 0) && !pool->pool_close)	
		{
			pthread_cond_wait(&(pool->queue_not_empty),&(pool->mutex));
		}

		//如果线程池被关闭，线程退出
		if(pool->pool_close)
		{
			pthread_mutex_unlock(&(pool->mutex));
			pthread_exit(NULL);
		}

		//取出一个任务，任务队列长度-1
		pool->queue_cur_num--;
		pjob = pool->head;
		//若取出任务后任务队列长度为空，将head = tail = NULL
		if(pool->queue_cur_num == 0)
		{
			pool->head = pool->tail = NULL;
		}
		//若不为空，头结点指向队列中的下一个任务
		else
		{
			pool->head = pjob->next;
		}

		//threadpool_destory函数执行时，只有队列非空才可以执行销毁线程函数
		if(pool->queue_cur_num == 0)
		{
			pthread_cond_signal(&(pool->queue_empty));
		}
		
		//如果队列非满，通知threadpool_add_job函数添加新任务
		if(pool->queue_cur_num == pool->queue_max_num - 1)
		{
			pthread_cond_broadcast(&(pool->queue_not_full));
		}
		pthread_mutex_unlock(&(pool->mutex));

		//线程真正要做的：调用回调函数，进入工作
		(*(pjob->callback_function))(pjob->arg);
		free(pjob);//执行完毕，销毁该任务
		pjob = NULL;
	}
}

/*销毁线程池函数*/
int threadpool_destroy(struct threadpool * pool)
{
	assert(pool != NULL);

	pthread_mutex_lock(&(pool->mutex));
	if(pool->queue_close || pool->pool_close)//线程池已经退出了，则直接返回
	{
		pthread_mutex_unlock(&(pool->mutex));
		return -1;
	}

	/*关闭任务队列*/
	pool->queue_close = 1;
	while(pool->queue_cur_num != 0)
	{
		//等待队列为空
		pthread_cond_wait(&(pool->queue_empty),&(pool->mutex));
	}

	/*关闭线程池*/
	pool->pool_close = 1;
	pthread_mutex_unlock(&(pool->mutex));
	pthread_cond_broadcast(&(pool->queue_not_empty));//唤醒所有正在阻塞的线程
	pthread_cond_broadcast(&(pool->queue_not_full));//唤醒添加任务的threadpool_add_job函数

	int i;
	for(i = 0; i < pool->thread_num; i++)
	{
		pthread_join(pool->pthreads[i], NULL);//等待线程池的所有线程执行完毕
	}

	pthread_mutex_destroy(&(pool->mutex));
	pthread_cond_destroy(&(pool->queue_empty));
	pthread_cond_destroy(&(pool->queue_not_empty));
	pthread_cond_destroy(&(pool->queue_not_full));
	free(pool->pthreads);
	pool->pthreads = NULL;

	struct job *tmp;
	while(pool->head != NULL)
	{
		tmp = pool->head;
		pool->head = tmp->next;
		free(tmp);
	}
	free(pool);
	pool = NULL;

	return 0;
}

void * work(void * arg)
{
	int *work_msg = (int *)arg;
	printf("threadpool %ld callback function: %d.\n",pthread_self(),*work_msg);
	sleep(1);
}

int main(void)
{
	struct threadpool * pool = threadpool_init(10,20);

	int i,*j = (int *)malloc(sizeof(int)*40);
	for(i = 0; i < 40; i++)
	{
		j[i] = i;
		threadpool_add_job(pool,work,&j[i]);
	}

	sleep(4);
	threadpool_destroy(pool);
	return 0;
}
