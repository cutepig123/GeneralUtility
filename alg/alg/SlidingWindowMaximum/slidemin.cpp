#include "stdlib.h"
#include "stdio.h"
#include "algorithm"
struct pairs {
    int value;
    int death;
};
void
minwindow(int const *in, int *out, int n, int k)
{
    int i;
    struct pairs * ring;
    struct pairs * minpair;
    struct pairs * end;
    struct pairs * last;
    ring = (pairs*)malloc(k * sizeof *ring);
    if (!ring) exit(EXIT_FAILURE);
    end  = ring + k;
    last = ring;
    minpair = ring;
    minpair->value = in[0];
    minpair->death = k;
    out[0] = in[0];
    for (i=1;i<n;i++) {
        if (minpair->death == i) {
            minpair++;
            if (minpair >= end) minpair = ring;
        }
        if (in[i] <= minpair->value) {
            minpair->value = in[i];
            minpair->death = i+k;
            last = minpair;
        } else {
            while (last->value >= in[i]) {
                if (last == ring) last = end;
                --last;
            }
            ++last;
            if (last == end) last = ring;
            last->value = in[i];
            last->death = i+k;
        }
        out[i] = minpair->value;
    }
    free(ring);
}
class PriQueue
{
	int k_;
	pairs ring[100];
	pairs * minpair;
	struct pairs * end;
    struct pairs * last;
	int i_;
public:
	explicit PriQueue(int k)
	:k_(k),minpair(0),i_(0),end(ring + k),last (ring)
	{
		 
	}
	int EnqueueAndGetMin(int v)
	{
		if(!minpair)
		{
			minpair = ring;
		    minpair->value = v;
		    minpair->death = k_;
		}
		else
		{
			if (minpair->death == i_) {
            minpair++;
            if (minpair >= end ) minpair = ring;
			}
			if (v <= minpair->value) {
				minpair->value = v;
				minpair->death = i_+k_;
				last = minpair;
			} else {
				while (last->value >= v) {
					if (last == ring) last = end;
					--last;
				}
				++last;
				if (last == end) last = ring;
				last->value = v;
				last->death = i_+k_;
			}
		}
		i_++;
        return minpair->value;
	}
};
void
minwindow2(int const *in, int *out, int n, int k)
{
	PriQueue q(k);
	for (int i=0; i<n; i++)
	{
		out[i] = q.EnqueueAndGetMin(in[i]);
	}
}
void print(int const *in, int n)
{
	for(int i=0; i<n;i++)
		printf("%d ",in[i]);
	printf("\n");
}
//////////////memthod3///////////////////////
/* struct to hold the actual data */
typedef struct node
{
    int data;
    struct node *next;
    struct node *nextMin;
}Node;
/* Queue head */
typedef struct
{
    Node *head;
    Node *tail;
    Node *min;
}Queue;
/* Initialize the queue */
inline void initQueue(Queue *q)
{
    q->tail = q->head = q->min = NULL;
}
/*check for queue empty */
inline int isEmpty(Queue *q)
{
    return !q || q->head == NULL;
}
/* Deallocates the queue */
void endQueue(Queue *q)
{
    if(isEmpty(q))
        return;
    Node *h = q->head;
    Node *tmp;
    while(h)
    {
        tmp = h;
        h = h->next;
        free(tmp);
    }
}
/* inserts element at the end */
void enqueue(Queue *q, int el)
{
    if(!q)
        return;
    Node *node = (Node*)calloc(sizeof(Node), 1);
    //assert(node != NULL);
    node->data = el;
    if(!q->head) /* if Q empty */
    {
        q->min = q->head = q->tail = node;
    }
    else
    {
		//removing all elements of A greater than or equal to x,
		//appending x to A

		if(q->min->data >= el) /*Update current min ( then q has only one element)*/
        {
            q->min = node;
        }
        else
        {
			Node *min = q->min;
			while(min)
            {
                if(!min->nextMin || min->nextMin->data >= el)
                {
                    min->nextMin = node;
                    break;
                }
                min = min->nextMin;
            }
        }
        q->tail->nextMin = q->tail->next = node; /* attach at the end */
        q->tail = node; /* move tail */
    }
}
/*pops the element at the beginning */
int dequeue(Queue *q, int *el)
{
    if(isEmpty(q))
        return -1;
    *el = q->head->data;
    Node *tmp = q->head;
    q->head = q->head->next; /* move head */

	//c: removing the initial element of A if it is being removed from the window. 
    if(q->min == tmp) /* if head is the min, move the min */
    {
        q->min = tmp->nextMin;
    }
    free(tmp);
    return 0;
}
/* Returns the minimum element in Q */
inline int findMin(Queue *q, int *el)
{
    if(isEmpty(q))
        return -1;
    *el = q->min->data;
    return 0;
}
/* prints the queue to stdout */
void printQueue(Queue *q)
{
    if(isEmpty(q))
        return;
    Node *h = q->head;
    while(h)
    {
        printf("%d ", h->data);
        h = h->next;
    }
    printf("\n");
}
////////////////////////////////////////////
int main()
{
	int in[] = {1,3,2,5,4,3,2,1,3,5,8,6,3};
	#define	N sizeof(in)/sizeof(in[0]) 
	int out[N]={0};
	int k = 3;
	printf("result of minwindow():\n");
	minwindow(in, out,N,k);
	print(out,N);
	printf("result of minwindow2():\n");
	std::fill(out,out+N,0);
	minwindow2(in, out,N,k);
	print(out,N);
	printf("result of Queue:\n");
	Queue q;
    initQueue(&q);
	int i;
	for(i=0;i<N;i++)
	{
		if(i>=k)
		{
			int j;
			dequeue(&q,&j);
		}
		enqueue(&q, in[i]);
		int v;
		findMin(&q, &v);
		printf("%d ",v);
	}
	endQueue(&q);
}
