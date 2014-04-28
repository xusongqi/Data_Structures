/* 
* Author:		xusongqi@live.com
* 
* Created Time: 2014年04月28日 星期一 10时15分23秒
* 
* FileName:     RB_Tree.c
* 
* Description:  
*
*/

#include <stdio.h>
#include <stdlib.h>

#define true	1
#define false	0
#define RED		1
#define BLACK	0

typedef struct rbtree
{
	_Bool	color;
	int		key;
	struct rbtree *leftChild;
	struct rbtree *rightChild;
	struct rbtree *parent;
}RB_Node, *RB_Tree;

static RB_Tree RB_Search_Assist(int key, RB_Tree index, RB_Tree * parent_return);	//辅助查询函数
static RB_Tree RB_Search(int key, RB_Tree index);		//查找函数
RB_Tree RB_Insert(int key, RB_Tree index);				//插入函数
static RB_Tree RB_Insert_Rebalance(RB_Tree node, RB_Tree index);	//插入再平衡
static RB_Tree RB_Rotate_Left(RB_Tree node, RB_Tree index);			//左旋
static RB_Tree RB_Rotate_Right(RB_Tree node, RB_Tree index);		//右旋
void PreOrder(RB_Tree tree);		//先序输出
void InOrder(RB_Tree tree);			//中序输出
void PostOrder(RB_Tree tree);		//后序输出

/*辅助搜索函数*/
static RB_Tree RB_Search_Assist(int key, RB_Tree index, RB_Tree * parent_return)
{
	RB_Tree node = index;//node初始化指向root，之后会移动到key应插入的位置
	RB_Tree parent = NULL;//parent若不为空，则指向key应插入节点的父节点，并返回给调用函数(插入，删除)；若为空则忽略（查找）
	//(插入函数传入的&parent恒不为空)

	while(node)//向下寻找key应插入点，到叶子节点为止
	{
		parent = node;
		if(key < node->key)//key小于当前节点，当前节点指向自身左孩子
		{
			node = node->leftChild;
		}
		else if(key > node->key)//key大于当前节点，当前节点指向自身右孩子
		{
			node = node->rightChild;
		}
		else//key == node->key	树中存在值等于key的节点，返回当前节点（查找，插入，删除）
		{
			return node;
		}
	}

	if(parent_return)//当parent非空，传回目标节点的父节点（插入，删除）
	{
		*parent_return = parent;
	}

	return NULL;//未找到值等于key的节点，返回空（查找）
}

/*查找函数*/
static RB_Tree RB_Search(int key, RB_Tree index)
{
	return RB_Search_Assist(key, index, NULL);
}

/*插入函数*/
RB_Tree RB_Insert(int key, RB_Tree index)
{
	RB_Tree parent = NULL;//插入节点的父节点
	RB_Tree node = NULL;//插入节点
	
	/*判定：若辅助查找函数返回非空，则树中已存在相同的key值。返回root节点。
	 * （赋值表达式返回的值等于表达式左值）*/
	if( node = RB_Search_Assist(key, index, &parent) )
	{
		return index;
	}
	/*开辟节点（已从辅助查询函数中得到插入点的parent位置）*/
	node = (RB_Tree)malloc(sizeof(RB_Node));
	node->color = RED;		//新节点初始化为红色
	node->key = key;		//写入key值
	node->parent = parent;	//之前带回了parent位置
	node->leftChild = NULL;
	node->rightChild = NULL;

	/*插入新节点*/
	if(parent)//若parent不为空（树非空）
	{
		if(key < parent->key)
		{
			parent->leftChild = node;
		}
		else
		{
			parent->rightChild = node;
		}
	}
	else//!parent	树为空，当前节点成为root点
	{
		index = node;
	}

	/*树的再平衡*/
	return RB_Insert_Rebalance(node, index);
}

/*插入后再平衡函数*/
static RB_Tree RB_Insert_Rebalance(RB_Tree node, RB_Tree index)
{
	RB_Tree parent, grandpa, uncle, temp;//父节点，祖父节点，叔叔节点，临时节点

	/*循环体：校正从插入点开始到root结束*/
	while((parent = node->parent) && (parent->color == RED))
	{
		grandpa = parent->parent;
		/*当父节点为祖父节点的左孩子*/
		if(parent == grandpa->leftChild)
		{
			uncle = grandpa->rightChild;
			/*情况一：父节点与叔节点均为红色*/
			if(uncle && uncle->color == RED)
			{
				uncle->color = BLACK;//叔节点染黑
				parent->color = BLACK;//父节点染黑
				grandpa->color = RED;//祖父节点染红
				node = grandpa;//指向祖父节点，继续算法
			}
			else//叔节点为黑（为黑或为空）
			{
				/*情况二：叔节点为黑，当前节点为父节点右孩子*/
				if(node == parent->rightChild)
				{
					index = RB_Rotate_Left(parent, index);//以父节点为轴，左旋
					temp = parent;
					parent = node;
					node = temp;//并将指针指向旋转前的父节点，继续算法
				}
				//注意：情况二必然导致情况三；但情况三未必经过情况二而来

				/*情况三：叔节点为黑，当前节点为父节点的左孩子*/
				parent->color = BLACK;//父节点染黑
				grandpa->color = RED;//祖父节点染红
				index = RB_Rotate_Right(grandpa, index);//以祖父节点为轴，左旋；继续算法
				//情况三的调整结束之后，node的父节点变成黑色，调整结束。
			}
		}
		/*父节点为祖父节点右孩子，情形与上类似但镜像对称*/
		else//parent == grandpa->rightChild
		{
			uncle = grandpa->leftChild;
			/*情况一：父节点与叔节点均为红色*/
			if(uncle && uncle->color == RED)
			{
				uncle->color = BLACK;//叔节点染黑
				parent->color = BLACK;//父节点染黑
				grandpa->color = RED;//祖父节点染红
				node = grandpa;//指向祖父节点，继续算法
			}
			else//叔节点为黑（为黑或为空）
			{
				/*情况二：叔节点为黑，当前节点为父节点左孩子*/
				if(node == parent->leftChild)
				{
					index = RB_Rotate_Right(parent, index);//以父节点为轴，右旋
					temp = parent;
					parent = node;
					node = temp;//当前节点指向旋转前的父节点，继续算法
				}
				//注意：情况二必然导致情况三；但情况三未必经过情况二而来

				/*情况三：叔节点为黑，当前节点为父节点右孩子*/
				parent->color =	BLACK;//父节点染黑
				grandpa->color = RED;//祖父节点染红
				index = RB_Rotate_Left(grandpa, index);//以祖父节点为轴，左旋；继续算法
			}
		}
	}
	index->color = BLACK;//root节点必然为黑色，没有为什么。
	return index;//返回root节点
}

/*左旋函数*/
static RB_Tree RB_Rotate_Left(RB_Tree node, RB_Tree index)
{
	RB_Tree right = node->rightChild;//right为节点的右孩子

	/*节点右孩子的左支变为节点的右枝*/
	if(node->rightChild = right->leftChild)
	{
		right->leftChild->parent = node;
	}
	right->leftChild = node;

	/*节点的父母变为节点右孩子的父母，节点成为右孩子（right）的左孩子*/
	if(right->parent = node->parent)
	{
		if(node == node->parent->leftChild)
		{
			node->parent->leftChild = right;
		}
		else//node == node->parent->rightChild
		{
			node->parent->rightChild = right;
		}
	}
	/*若节点原为root节点，则其右孩子成为新的root点*/
	else
	{
		index = right;
	}
	node->parent = right;

	return index;
}

/*右旋函数*/
static RB_Tree RB_Rotate_Right(RB_Tree node, RB_Tree index)
{
	RB_Tree left = node->leftChild;//left为节点的左孩子
	
	/*节点左孩子的右枝成为节点的左枝*/
	if(node->leftChild = left->rightChild)
	{
		left->rightChild->parent = node;
	}
	left->rightChild = node;

	/*节点的父母成为节点左孩子的父母,节点成为左孩子（left）的右孩子*/
	if(left->parent = node->parent)
	{
		if(node == node->parent->leftChild)
		{
			node->parent->leftChild = left;
		}
		else//node == node->parent->rightChild
		{
			node->parent->rightChild = left;
		}
	}
	/*若节点原为root节点，则其右孩子成为新的root点*/
	else
	{
		index = left;
	}
	node->parent = left;

	return index;
}

/*先序输出*/
void PreOrder(RB_Tree tree)
{
	if(tree != NULL)
	{
		printf("%d ",tree->key);
		PreOrder(tree->leftChild);
		PreOrder(tree->rightChild);
	}
}

/*中序输出*/
void InOrder(RB_Tree tree)
{
	if(tree != NULL)
	{
		InOrder(tree->leftChild);
		printf("%d ",tree->key);
		InOrder(tree->rightChild);
	}
}

/*后序输出*/
void PostOrder(RB_Tree tree)
{
	if(tree != NULL)
	{
		PostOrder(tree->leftChild);
		PostOrder(tree->rightChild);
		printf("%d(%d) ",tree->key,tree->color);
	}
}

int main(void)
{
	int array[10] = {58,36,51,23,159,236,95,75,66,1};
	int i;
	RB_Tree tree = NULL;
	for(i = 0; i < 10; i++)
	{
		tree = RB_Insert(array[i],tree);

		printf("先序输出：");
		PreOrder(tree);
		printf("\n中序输出：");
		InOrder(tree);
		printf("\n后序输出：");
		PostOrder(tree);
		printf("\n\n");
	}


	return 0;
}
