/* 
* Author:		xusongqi@live.com
* 
* Created Time: 2014年03月21日 星期五 20时19分53秒
* 
* FileName:     BSTree.c
* 
* Description:  
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

typedef struct BSTree{
	int	key;
	struct BSTree * leftChild;
	struct BSTree * rightChild;
}BSTree;

_Bool insertNode(BSTree **root, int key);//插入
void createBode();//创建
_Bool selectNode(BSTree * node, int key);//查找
_Bool deleteNode(BSTree ** pnode, int key);//删除
void preorder(BSTree *node);//先序
void inorder(BSTree *node);//中序
void postorder(BSTree *node);//后续
BSTree * findMin();

//创建节点
void createNode(BSTree **node)
{
	*node = (BSTree *)malloc(sizeof(BSTree));
	(*node)->leftChild = NULL;
	(*node)->rightChild = NULL;
	(*node)->key = -1;
}

//插入节点
_Bool insertNode(BSTree **node, int key)
{
	//先判定是否空指针,是则开辟空间
	if(*node == NULL)
	  createNode(node);

	//再进行插入操作
	if((*node)->key == -1)	//空节点，直接插入当前位置
	  (*node)->key = key;

	else if(key < (*node)->key)	//key小于该节点值，递归左孩子
	  insertNode(&((*node)->leftChild), key);

	else if(key > (*node)->key)	//key大于该节点值，递归右孩子
	  insertNode(&((*node)->rightChild),key);
	
	else	//key重复，返回错误值
	  return false;
	
	return true;
}

//查找节点
_Bool selectNode(BSTree * node, int key)
{
	if(node == NULL)
	{
		puts("NOT SUCH A NODE\n");
		return true;
	}

	else if(key == node->key)
	  return true;

	else if(key < node->key)
	  selectNode(node->leftChild,key);

	else if(key > node->key)
	  selectNode(node->rightChild,key);
}

//删除结点
_Bool deleteNode(BSTree ** pnode,int key)
{
	BSTree *tmpNode = NULL;

	if(NULL == pnode || NULL == *pnode)	//指针无目标或树为空
	  return false;

	//查找目标节点
	else if(key < (*pnode)->key)
	  return deleteNode(&(*pnode)->leftChild, key);//&（*pnode）中的&使之重新构成二级指针
	else if(key > (*pnode)->key)
	  return deleteNode(&(*pnode)->rightChild, key);

	//已找到目标节点，即key = pnode->key
	else
	{
		if((*pnode)->leftChild && (*pnode)->rightChild) //目标节点有两个儿子
		{
			tmpNode = findMin((*pnode)->rightChild);
			(*pnode)->key = tmpNode->key;
			return deleteNode(&(*pnode)->rightChild, (*pnode)->key);//递归的删除该已变为枝点的叶子节点
		}

		else if((*pnode)->leftChild) //目标节点只有左儿子
		{
			tmpNode = (*pnode)->leftChild;
			(*pnode)->key = tmpNode->key;
			return deleteNode(&(*pnode)->leftChild, (*pnode)->key);//递归的删除该已变为枝点的叶子节点
		}

		else if((*pnode)->rightChild) //目标节点只有右儿子
		{
			tmpNode = (*pnode)->rightChild;
			(*pnode)->key = tmpNode->key;
			return deleteNode(&(*pnode)->rightChild, (*pnode)->key);//递归的删除该已变为枝点的叶子节点
		}
		
		else	//目标节点没有儿子，包括叶子节点以及没有儿子的root点
		{
			free(*pnode);
			(*pnode) = NULL;
		}
	}

	return true;
}

//先序遍历
void preorder(BSTree *node)
{
	if(node != NULL)
	{
		printf("%d ", node->key);
		preorder(node->leftChild);
		preorder(node->rightChild);
	}
}

//中序遍历
void inorder(BSTree *node)
{
	if(node != NULL)
	{
		inorder(node->leftChild);
		printf("%d ", node->key);
		inorder(node->rightChild);
	}
}

//后序遍历
void postorder(BSTree *node)
{
	if(node != NULL)
	{
		postorder(node->leftChild);
		postorder(node->rightChild);
		printf("%d ", node->key);
	}
}

BSTree * findMin(BSTree * node)
{
	if(node->leftChild != NULL)
	  return findMin(node->leftChild);
	else
	  return node;
}

int main(void)
{
	int array[10] = {35,24,12,1,37,98,97,66,67,27};
	int choose;
	BSTree *root;
	int del;
	int i;

	//初始化
	root=(BSTree*)malloc(sizeof(BSTree));
	root->leftChild=NULL;
	root->rightChild=NULL;
	root->key=-1;

	//createNode(root);
	for(i = 0; i < 10; i++)
	{ insertNode(&root,array[i]);
	inorder(root);//调试语句
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
				   deleteNode(&root,del);
				   break;
			case 3:preorder(root);
				   puts("");
				   break;
			case 4:inorder(root);
				   puts("");
				   break;
			case 5:postorder(root);
				   puts("");
				   break;
			default:puts("WRONG CHOOSE\n\n");
					sleep(1);
		}
	}
}

