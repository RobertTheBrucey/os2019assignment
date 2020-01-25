/*****************************************************************************
* File:      ReadyQueue.h
* Author:    Sean Ashton * Student ID: 14866636
* Unit:      COMP2006 Operating Systems
* Purpose:   Header file for ReadyQueue.c
* Reference: None.
* Comments:
* Requires:
* Created:   10/04/2019 * Last Modified: 28/04/2019
*****************************************************************************/
#ifndef READYQUEUE_H
#define READYQUEUE_H 1
#endif
#define FALSE 0
#define TRUE !FALSE

/* Task */
typedef struct Task {
    int id;
    int cpu_burst;
    time_t arriveTime;
} Task;

/* Ready Queue */
typedef struct QueueNode {
    struct QueueNode* next;
    struct Task* data;
} QueueNode;

typedef struct ReadyQueue {
    struct QueueNode* head;
    struct QueueNode* tail;
    int maxNodes;
    int numNodes;
    pthread_mutex_t mutex;
    pthread_cond_t sigTask;
    pthread_cond_t sigCPU;
} ReadyQueue;

/* Function Prototypes */
int enqueue( ReadyQueue*, Task* );
int dequeue( ReadyQueue*, Task** );
int initQueue ( ReadyQueue* );
void freeQueue( ReadyQueue* );
