/* 
* Author:		xusongqi@live.com
* 
* Created Time: 2014年04月28日 星期一 10时15分23秒
* 
* FileName:     RB_Tree.c
* 
* Description:  删除调节出现bug：
*						第一组实验数据正常
*						第二组实验数据删除6出错：丢失key[1,2]
*						第三组实验数据删除1，key大量丢失
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
static RB_Tree RB_Insert_Rebalance(RB_Tree node, RB_Tree index);	//插入再平衡函数
RB_Tree RB_Delete(int key, RB_Tree index);				//删除函数
static RB_Tree RB_Delete_Rebalance(RB_Tree node, RB_Tree parent, RB_Tree index);	//删除再平衡函数
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

RB_Tree RB_Delete(int key, RB_Tree index)
{
	RB_Tree child,	//当前操作节点的子节点
			parent, //当前操作节点的父节点
			old,	//待删节点
			left,	//当前操作节点的左孩子
			node;	//当前操作节点
	_Bool color;

	/*查找待删节点*/
	if(!(node = RB_Search_Assist(key, index, NULL)))
	{
		//若没找到，则返回
		printf("NOT SUCH A KEY！\n");
		return index;
	}
	//找到待删结点
	old = node;

	/*若待删结点左右孩子都存在*/
	if(node->leftChild && node->rightChild)
	{
		/*查找待删结点的右枝的最小节点*/
		node = node->rightChild;
		//以下循环可以简化为while(node = node->leftChild);
		while(left = node->leftChild)
		{
			node = left;
		}
		child = node->rightChild;
		parent = node->parent;
		color = node->color;

		/*将替代节点（node）从树上取下*/
		if(child)//当前节点的右孩子的父亲等于当前节点的父亲
		{
			child->parent = parent;
		}
		//node的父节点对自己的指针指向node的右孩子
		if(parent)//若父节点存在：只是为了格式统一.....
		{
			if(node == parent->leftChild)
			{
				parent->leftChild = child;
			}
			else
			{
				parent->rightChild = child;
			}
		}
		else
		{
			index = child;
		}
		//当node为old的右孩子时有效，并将child设为当前的node的右孩子（即原来node的位置，现在为NULL）
		if(node->parent == old)
		{
			parent = node;
		}

		/*将取下的node节点覆盖掉待删节点（old）*/
		node->parent = old->parent;
		node->color  = old->color;
		node->leftChild = old->leftChild;
		node->rightChild = old->rightChild;
		//若old的parent存在
		if(old->parent)
		{
			//old为左子
			if(old->parent->leftChild == old)
			{
				old->parent->leftChild = node;
			}
			//old为右子
			else
			{
				old->parent->rightChild = node;
			}
		}
		//若old的parent不存在
		else
		{//即old原来为root节点，现在node成为新的root点
			index = node;
		}
		old->leftChild->parent = node;
		//需要判断一下rightChild是否为NULL,否则会内存出错
		if(old->rightChild)
		{
			old->rightChild->parent = node;
		}
	}//[old的左右孩子都存在]结束

	/*若待删结点的左右孩子至少有一个为空*/
	else//(!node->left)||(!node->right)
	{
		//左孩子为空
		if(!node->leftChild)
		{
			child = node->rightChild;
		}
		//右孩子为空
		else if(!node->rightChild)
		{
			child = node->leftChild;
		}
		parent = node->parent;
		color = node->color;

		/*将待删结点删掉*/
		if(child)	//若孩子节点存在
		{
			child->parent = parent;
		}
		if(parent)
		{
			if(parent->leftChild == node)
			{
				parent->leftChild = child;
			}
			else//parent->rightChild == node
			{
				parent->rightChild = child;
			}
		}
		else//old为原root，现在node成为新的root。该情况仅限树中存在old与child....
		{
			index = child;
		}
	}//[else:待删结点的孩子至少有一个为空]结束
	/*释放待删结点*/
	free(old);
	old = NULL;
	/*若删掉的节点颜色为黑色,需要进入插入再平衡函数调节*/
	if(color == BLACK)
	{
		index = RB_Delete_Rebalance(child, parent, index);
	}

	return index;
}

/*删除再平衡函数*/
static RB_Tree RB_Delete_Rebalance(RB_Tree node, RB_Tree parent, RB_Tree index)
{
	RB_Tree brother;//节点的兄弟

	/*开始循环调整，以下四种情况，当前节点(node)均为黑色*/
	while((!node || node->color == BLACK) && node != index)
	{
		/*当前节点为父节点的左子*/
		if(parent->leftChild == node)
		{
			brother = parent->rightChild;

			/*情况一:兄弟节点为红色
			 *解决办法：父节点变红，兄弟节点变黑，左旋父节点
			 * */
			if(brother->color == RED)
			{
				parent->color = RED;
				brother->color = BLACK;
				index = RB_Rotate_Left(parent, index);
			}
			else//以下是brother颜色为黑的三种情况
			{
				/*情况二：兄弟节点为黑，兄弟节点的两个子节点也为黑
				*解决办法：先将兄弟节点变红，然后根据父节点：
							1.若父节点为黑，父节点成为当前节点，继续算法
							2.若父节点为红，将父节点变黑。树平衡，结束算法
				 * */
				if( (!brother->leftChild || brother->leftChild->color == BLACK) &&
					(!brother->rightChild || brother->rightChild->color == BLACK) )
				{
					brother->color = RED;
					//若父节点为红，变为黑，结束算法
					if(parent->color == RED)
					{
						parent->color = BLACK;
						break;
					}
					node = parent;//父节点成为新的当前节点(父节点原即为黑)
					parent = node->parent;
					continue;//重新进入算法循环
				}
				/*情况三：兄弟节点为黑，兄弟节点的左孩子为红，右孩子为黑
				 *解决办法：将兄弟节点染红，兄弟节点的左孩子染黑，右旋兄弟节点
				 * */
				if( (brother->leftChild && brother->leftChild->color == RED) && 
					(!brother->rightChild || brother->rightChild->color == BLACK))
				{
					brother->color = RED;
					brother->leftChild->color = BLACK;
					index = RB_Rotate_Right(brother, index);
				}
				/*情况四：兄弟节点为黑，兄弟节点的右孩子为红，左孩子颜色任意
				 *解决办法：兄弟节点染成父节点的颜色，父节点染黑，兄弟节点的右孩子染黑，左旋父节点。树平衡，算法结束
				 * */
				brother->color = parent->color;
				parent->color = BLACK;
				brother->rightChild->color = BLACK;//此时该节点原本颜色一定为红色
				index = RB_Rotate_Left(parent, index);
				node = index;
			}//[else:bro为黑]结束
		}//[if:node为父节点左孩子]结束

		/*当前节点为父节点右孩子*/
		else
		{
			brother = parent->leftChild;

			/*情况一：兄弟节点为红
			 *解决办法：父节点染红，兄弟节点染黑，右旋父节点
			 * */
			if(brother->color == RED)
			{
				parent->color = RED;
				brother->color = BLACK;
				index = RB_Rotate_Right(parent, index);
			}
			/*以下三种情况，兄弟节点均为黑色*/
			else
			{
				/*情况二：兄弟节点为黑，兄弟节点的两个字节点也为黑
				*解决办法：先将兄弟节点变红，然后根据父节点：
							1.若父节点为黑，父节点成为当前节点，继续算法
							2.若父节点为红，将父节点变黑。树平衡，结束算法
				 * */
				if( (!brother->leftChild ||brother->leftChild->color == BLACK) && 
					(!brother->rightChild || brother->rightChild->color == BLACK))
				{
					brother->color = RED;
					//若父节点为红，染黑，树即平衡。
					if(parent->color == RED)
					{
						parent->color = BLACK;
						break;
					}
					node = parent;//父节点成为新的当前节点（父节点原来即黑色）
					parent = node->parent;
					continue;//重新进入算法循环
				}
				/*情况三：兄弟节点为黑，兄弟节点的右孩子为红，左孩子为黑
				 *解决办法：兄弟节点变红，兄弟节点的右孩子变黑，左旋兄弟节点
				 * */
				if( (brother->rightChild && brother->rightChild->color == RED) &&
					(!brother->leftChild || brother->leftChild->color == BLACK))
				{
					brother->color = RED;
					brother->rightChild->color = BLACK;
					index = RB_Rotate_Left(brother, index);
				}
				/*情况四：兄弟节点为黑，兄弟节点的左孩子为红，右孩子颜色任意
				 *解决办法：兄弟节点染成父节点颜色，父节点染黑，兄弟节点的左孩子染黑，右旋父节点
				 * */
				brother->color = parent->color;
				parent->color = BLACK;
				brother->color = BLACK;
				index = RB_Rotate_Right(parent, index);
			}//[else:兄弟节点为黑]结束
		}//[else:node为父节点右孩子]结束
	}//[while:删除再平衡算法]结束

	/*将root点的颜色置为黑色*/
	if(node)
	{
		node->color = BLACK;
	}

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
	//int array[10] = {58,36,51,23,159,236,95,75,66,1};
	//int array[12] = {12,1,9,2,0,11,7,19,4,15,18,5};
	int array[10] = {1,2,3,4,5,6,7,8,9,10};
	int i,
		del,
		choose;
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

	while(1)
	{
		printf("1.exit\n");
		printf("2.delete\n");
		printf("3.preorder\n");
		printf("4.inorder\n");
		printf("5.postorder\n");
		printf("0.show all(3.+4.+5.)\n");
		scanf("%d",&choose);

		switch(choose)
		{
			case 1:exit(0);
				   break;
			case 2:printf("which?\n");
				   scanf("%d",&del);
				   tree = RB_Delete(del, tree);
				   break;
			case 3:PreOrder(tree);
				   puts("");
				   break;
			case 4:InOrder(tree);
				   puts("");
				   break;
			case 5:PostOrder(tree);
				   puts("");
				   break;
			case 0:printf("先序输出：");
				   PreOrder(tree);
				   printf("\n中序输出：");
				   InOrder(tree);
				   printf("\n后序输出：");
				   PostOrder(tree);
				   printf("\n\n");
				   break;
			default:puts("WRONG CHOOSE\n\n");
					sleep(1);
		}
	}

	return 0;
}
