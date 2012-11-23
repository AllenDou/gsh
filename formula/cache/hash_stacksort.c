#define PARENT(i) (i)/2
#define LEFT(i) 2*(i)
#define RIGHT(i) 2*(i)+1

#define HASHTABLESIZE 2807303

#define HEAPSIZE 10
#define A 0.6180339887
#define M 16384 //m=2^14

#include <stdio.h>
#include <stdlib.h>

typedef struct hash_node
{
		int data;
		int count;
		struct hash_node* next;
}HASH_NODE;

HASH_NODE* hash_table[HASHTABLESIZE];

HASH_NODE* creat_node(int data)
{

		HASH_NODE* node = (HASH_NODE*)malloc(sizeof(HASH_NODE));

		if (NULL == node)
		{
				printf("malloc node failed!/n");
				exit(EXIT_FAILURE);

		}

		node->data = data;
		node->count = 1;
		node->next = NULL;
		return node;
}

/**
  44. * hash 函数采用乘法散列法
  45. * h(k)=int(m*(A*k mod 1))
  46. */
int hash_function(int key)
{
		double result = A * key;
		return (int)(M * (result - (int)result));
}

void insert(int data)
{

		int index = hash_function(data);

		HASH_NODE* pnode = hash_table[index];
		while (NULL != pnode)
		{
				// 以存在 data,则 count++
				if (pnode->data == data)
				{
						pnode->count += 1;
						return;
				}
				pnode = pnode->next;

		}

		// 建立一个新的节点,在表头插入
		pnode = creat_node(data);
		pnode->next = hash_table[index];
		hash_table[index] = pnode;
}

/**
  74. * destroy_node 释放创建节点产生的所有内存
  75. */
void destroy_node()
{
		HASH_NODE* p = NULL;
		HASH_NODE* tmp = NULL;
		int i;
		for (i = 0; i < HASHTABLESIZE; ++i)
		{
				p = hash_table[i];
				while (NULL != p)
				{
						tmp = p;
						p = p->next;
						free(tmp);
						tmp = NULL;


				}
		}
}

typedef struct min_heap
{
		int count;
		int data;
}MIN_HEAP;
MIN_HEAP heap[HEAPSIZE + 1];


/**
  101. * min_heapify 函数对堆进行更新,使以 i 为跟的子树成最大堆
//102. */
void min_heapify(MIN_HEAP* H, const int size, int i)
{
		int l = LEFT(i);
		int r = RIGHT(i);
		int smallest = i;


		if (l <= size && H[l].count < H[i].count)


				smallest = l;
		if (r <= size && H[r].count < H[smallest].count)
				smallest = r;
		if (smallest != i)
		{
				MIN_HEAP tmp = H[i];
				H[i] = H[smallest];
				H[smallest] = tmp;
				min_heapify(H, size, smallest);

		}
}

/**
 * build_min_heap 函数对数组 A 中的数据建立最小堆
 */
void build_min_heap(MIN_HEAP* H, int size)
{
		int i;
		for (i = size/2; i >= 1; --i)
				min_heapify(H, size, i);
}

/**
 * traverse_hashtale 函数遍历整个 hashtable,更新最小堆
 */
void traverse_hashtale()
{
		HASH_NODE* p = NULL;
		int i;
		for (i = 0; i < HASHTABLESIZE; ++i)
		{
				p = hash_table[i];
				while (NULL != p)
				{

						//// 如果当前节点的数量大于最小堆的最小值,则更新堆
						if (p->count > heap[1].count)


						{
								heap[1].count = p->count;
								heap[1].data = p->data;
								min_heapify(heap, HEAPSIZE, 1);
						}
						p = p->next;


				}
		}
}

int main()
{
		// 初始化最小堆
		int i;
		for (i = 1; i <= 10; ++i)
		{
				heap[i].count = -i;
				heap[i].data = i;
		}
		build_min_heap(heap, HEAPSIZE);

		FILE* fp = fopen("data.txt", "r");
		int num;
		while (!feof(fp))
		{
				fscanf(fp, "%d", &num);
				insert(num);
		}
		fclose(fp);

		traverse_hashtale();

		for (i = 1; i <= 10; ++i)
		{


				printf("%d\t%d\n", heap[i].data, heap[i].count);
		}
		return 0;
}

