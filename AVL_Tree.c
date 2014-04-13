/* 
* Author:		xusongqi@live.com
* 
* Created Time: 2014年03月24日 星期一 21时36分31秒
* 
* FileName:     AVL_Tree.c
* 
* Description:  
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
#define LH	 1
#define EH	 0
#define RH  -1
#define SIZE 10

typedef int Status;
typedef int ElemType;
_Bool	taller	= false;
_Bool	shorter	= false;

/*定义树的结构体*/
typedef struct AVLNode
{
	ElemType	key;			//关键字
	int			bf;				//结点的平衡度
	struct AVLNode *leftChild;	//左孩子
	struct AVLNode *rightChild;	//右孩子
}AVLNode,*AVLTree;

/*定义栈结构体*/
typedef struct Stack
{
	AVLTree node;//栈顶指针
	struct Stack *next;//指向下层
}Stack;

/*函数声明*/
Status InsertNode(AVLTree *node, ElemType key);	//插入结点
Status DeleteNode(AVLTree *node, ElemType key);	//删除结点
int	 MinElement(AVLTree node);					//寻找当前子树的最小关键字
void LeftBalance(AVLTree *node);				//左平衡
void RightBalance(AVLTree *node);				//右平衡
void LeftRotate(AVLTree *node);					//左旋
void RightRotate(AVLTree *node);				//右旋
Status PreOrder(AVLTree tree);					//先序输出AVL树
Status InOrder(AVLTree tree);					//中序输出AVL树
Status PostOrder(AVLTree tree);					//后序输出AVL树

//void CreateStack(Stack *stack);					//初始化栈
//void PushStack(Stack *stack, AVLTree node);		//入栈
//Stack *PopStack(Stack *stack);					//出栈

//插入节点
Status InsertNode(AVLTree *node, ElemType key)
{
//	puts("into insert ok");//这是调试代码

	/*插入新的结点，树长高*/
	if((*node) == NULL)
	{
		(*node) = (AVLTree)malloc(sizeof(AVLNode));
		(*node)->leftChild	= NULL;
		(*node)->rightChild	= NULL;
		(*node)->key = key;
		(*node)->bf  = EH;

		taller = true;
	}

	/*若关键字小于当前结点*/
	else if(key < (*node)->key)
	{
		if(! InsertNode(&((*node)->leftChild), key) )
		  return false;//若插入失败返回false
		if(taller)
		{//已插入左子树且左子树长高
			switch((*node)->bf)
			{
				case LH://本身左高，现在左子树高再次+1，所以需要调整
					LeftBalance(node);	taller = false;	break;
				case EH://原本该结点平衡，现在左子树高+1，所以自身高度也+1（taller = true）
					(*node)->bf = LH;	taller = true;	break;
				case RH://原本右子树高，现在左子树高+1，所以平衡
					(*node)->bf = EH;	taller = false;	break;
			}
		}
	}

	/*若关键字大于当前结点*/
	else if(key > (*node)->key)
	{
		if(! InsertNode(&((*node)->rightChild), key) )
		  return false;//若插入失败返回false
		if(taller)
		{//已插入右子树且右子树长高
			switch((*node)->bf)
			{
				case LH://原本左子树高，现在右子树高+1，所以平衡
					(*node)->bf = EH;	taller = false;	break;
				case EH://原本该结点平衡，现在右子树高+1，所以自身高度也+1（taller = true）
					(*node)->bf = RH;	taller = true;	break;
				case RH://本身右高，现在右子树高再次+1，所以需要调整
					RightBalance(node);	taller = false;	break;
			}
		}
	}

	/*key值重复*/
	else
	{taller = false;	return false;	}

	return true;
}/*end of insert()*/

/*删除结点*/
Status DeleteNode(AVLTree *node, ElemType key)
{
	/*待删结点小于当前结点*/
	if(key < (*node)->key)
	{
		DeleteNode( &((*node)->leftChild), key);
		if(shorter)//已删除结点，当左子树变短时，进行平衡判断
		{
			switch( (*node)->bf )
			{
				case LH://原来左高，现在左子树-1，所以等高
					(*node)->bf = EH;	shorter = true;
					break;
				case EH://原来等高，现在左子树-1，所以右高
					(*node)->bf = RH;	shorter = false;
					break;
				case RH://原来右高，现在左子树-1，所以需要左旋，调整平衡
					LeftBalance(node);	shorter = true;
			}
		}
	}

	/*待删结点大于当前结点*/
	else if(key > (*node)->key)
	{
		DeleteNode( &((*node)->rightChild), key);
		if(shorter)//已删除结点，当左子树变短时，进行平衡判断
		{
			switch( (*node)->bf )
			{
				case LH://原来左高，现在右子树-1，所以需要右旋，调整平衡
					LeftBalance(node);	shorter = true;
					break;
				case EH://原来等高，现在右子树-1，所以左高
					(*node)->bf = LH;	shorter = false;
					break;
				case RH://原来右高，现在右子树-1，所以等高
					(*node)->bf = EH;	shorter = true;
					break;
			}
		}
	}

	/*待删结点等于当前结点*/
	else
	{
		/*如果待删结点有右子树*/
		if( (*node)->rightChild )
		{
			(*node)->key = MinElement( (*node)->rightChild );
			DeleteNode( &((*node)->rightChild), (*node)->key );
			if(shorter)//已删除结点，当右子树变短时，进行平衡判断
			{
				switch( (*node)->bf )
				{
					case LH://原来左高，现在右子树-1，所以需要右旋，调整平衡
						LeftBalance(node);	shorter = true;
						break;
					case EH://原来等高，现在右子树-1，所以左高
						(*node)->bf = LH;	shorter = false;
						break;
					case RH://原来右高，现在右子树-1，所以等高
						(*node)->bf = EH;	shorter = true;
						break;
				}
			}
		}

		/*如果当前结点仅有左子树*/
		//那么该结点只可能存在于倒数第二层，所以无需判断删除结点后，当前结点是否失衡
		else if( (*node)->leftChild )
		{
			(*node)->key = (*node)->leftChild->key;
			(*node)->leftChild = NULL;
			free( (*node)->leftChild );
		}

		/*该结点为叶结点*/
		else
		{
			free(*node);
			*node = NULL;
			shorter = true;
		}
	}
}

/*寻找最小元素*/
int MinElement(AVLTree node)
{
	if( node->leftChild )
	  return MinElement(node->leftChild);
	else 
	  return node->key;
}

/*左平衡*/
void LeftBalance(AVLTree *node)
{
	AVLTree lc = (*node)->leftChild;//结点的左孩子
	AVLTree rd = lc->rightChild;	//结点的左孩子的右孩子

	switch(lc->bf)
	{
		case LH:
			(*node)->bf = lc->bf = EH;
			RightRotate(node);
			break;
		case RH:
			switch(rd->bf)
			{
				case LH:
					(*node)->bf = RH;	lc->bf = EH;
					break;
				case EH:
					(*node)->bf = lc->bf = EH;
					break;
				case RH:
					(*node)->bf = EH;	lc->bf = LH;
					break;
			}
			rd->bf = EH;
			LeftRotate(&((*node)->leftChild));//对该结点的左子树进行左旋
			RightRotate(node);//对该子树进行右旋
	}
}

/*右平衡*/
void RightBalance(AVLTree *node)
{
	AVLTree rc = (*node)->rightChild;//结点的右孩子
	AVLTree ld = rc->leftChild;	//结点的右孩子的左孩子

	switch(rc->bf)
	{
		case RH:
			(*node)->bf = rc->bf = EH;
			LeftRotate(node);
			break;
		case LH:
			switch(ld->bf)
			{
				case RH:
					(*node)->bf = LH;	rc->bf = EH;
					break;
				case EH:
					(*node)->bf = rc->bf = EH;
					break;
				case LH:
					(*node)->bf = EH;	rc->bf = RH;
					break;
			}
			ld->bf = EH;
			RightRotate(&((*node)->rightChild));//对该结点的右子树进行右旋
			LeftRotate(node);
	}

}

/*右旋：左树高旋转*/
void RightRotate(AVLTree *node)
{
	AVLTree lc = (*node)->leftChild;

	(*node)->leftChild = lc->rightChild;
	lc->rightChild = (*node);
	*node = lc;
}

/*左旋：右树高调整*/
void LeftRotate(AVLTree *node)
{
	AVLTree rc = (*node)->rightChild;

	(*node)->rightChild = rc->leftChild;
	rc->leftChild = (*node);
	*node = rc;
}

/*先序输出*/
Status PreOrder(AVLTree tree)
{
	if(tree != NULL)
	{
		printf("%d ",tree->key);
		PreOrder(tree->leftChild);
		PreOrder(tree->rightChild);
	}
}

/*中序输出*/
Status InOrder(AVLTree tree)
{
	if(tree != NULL)
	{
		InOrder(tree->leftChild);
		printf("%d ",tree->key);
		InOrder(tree->rightChild);
	}
}

/*后序输出*/
Status PostOrder(AVLTree tree)
{
	if(tree != NULL)
	{
		PostOrder(tree->leftChild);
		PostOrder(tree->rightChild);
		printf("%d ",tree->key);
	}
}

int main(void)
{
	int array[SIZE] = {3,5,4,6,9,1,8,10,7,2};
//	int array[SIZE] = {1,2,3,4,5,6,7,8,9,10};
	int choose;
	int del;
	int i;
	AVLTree root = NULL;

	//插入数组中的元素
	for(i = 0; i < SIZE; i++)
	{ 
		//puts("into for() ok");//这是调试代码
		InsertNode(&root, array[i]);
		puts("中序");
		InOrder(root);//调试语句
		puts("\n先序");
		PreOrder(root);
		puts("\n后序");
		PostOrder(root);
		puts("");
	}
	while(1)
	{
		printf("1.exit\n");
		printf("2.delete\n");
		printf("3.preorder\n");
		printf("4.inorder\n");
		printf("5.postorder\n");
		scanf("%d",&choose);

		switch(choose)
		{
			case 1:exit(0);
				   break;
			case 2:printf("which?\n");
				   scanf("%d",&del);
				   DeleteNode(&root,del);
				   break;
			case 3:PreOrder(root);
				   puts("");
				   break;
			case 4:InOrder(root);
				   puts("");
				   break;
			case 5:PostOrder(root);
				   puts("");
				   break;
			default:puts("WRONG CHOOSE\n\n");
					sleep(1);
		}
	}
}
