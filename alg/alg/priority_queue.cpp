#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
    assert(node != NULL);
    node->data = el;

    if(!q->head) /* if Q empty */
    {
        q->min = q->head = q->tail = node;
    }
    else
    {

     

        if(q->min->data >= el) /*Update current min */
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

void printQueueByPrio(Queue *q)
{
    if(isEmpty(q))
        return;

    Node *h = q->min;

    while(h)
    {
        printf("%d ", h->data);
        h = h->nextMin;
    }
    printf("\n");
}

int main()
{
    Queue q;

    initQueue(&q);

    int choice;
    int el;

    int n;

    scanf("%d", &n);

    while(n--)
    {
        scanf("%d", &el);
        enqueue(&q, el);
    }


    while(1)
    {
        printf("\n\t0: enqueue\n");
        printf("\t1: dequeue\n");
        printf("\t2: findMin\n");
        printf("\t3: printQ\n");
        printf("\t4: exit\n");
        printf("\t5: printQ by prio\n");
        printf("\tEnter your choice: ");

        scanf("%d", &choice);

        switch(choice)
        {
            case 0:
                printf("\n\tEnter Element:");
                scanf("%d", &el);
                enqueue(&q, el);
                break;

            case 1:
                if(dequeue(&q, &el) == 0)
                    printf("%d\n", el);
                else
                    printf("\nQueue empty\n");

                break;

            case 2:
                if(findMin(&q, &el) == 0)
                    printf("%d\n", el);
                else
                    printf("\nQueue empty\n");
                break;

            case 3:
                printQueue(&q);
                break;
			
			case 5:
                printQueueByPrio(&q);
                break;

            case 4:
                return 0;
        }
    }

    endQueue(&q);
}

