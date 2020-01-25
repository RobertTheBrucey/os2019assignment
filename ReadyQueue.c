/*****************************************************************************
* File:      ReadyQueue.c
* Author:    Sean Ashton * Student ID: 14866636
* Unit:      COMP2006 Operating Systems
* Purpose:   A Queue of Tasks for scheduler simulation
* Reference: Sean Ashton UCP Assignment was used as a basis for this file
* Comments:
* Requires:
* Created:   10/04/2019 * Last Modified: 28/04/2019
*****************************************************************************/
#include <pthread.h>
#include <stdlib.h>
#include "ReadyQueue.h"

/*****************************************************************************
* Name: initQueue
* Purpose: Initialise a new queue
* Imports: Pointer to a ReadyQueue
* Exports: returnCode
* Comments:
*****************************************************************************/
int initQueue( ReadyQueue *queue )
{
    int returnCode = 1;
    if ( queue != NULL )
    {
        queue->head = NULL;
        queue->tail = NULL;
        queue->maxNodes = 10;
        queue->numNodes = 0;
        /* Initialise mutex and signals for cpu/task synchronization */
        pthread_mutex_init( &queue->mutex, NULL );
        pthread_cond_init( &queue->sigTask, NULL );
        pthread_cond_init( &queue->sigCPU, NULL );
        returnCode = 0;
    }
    return returnCode;
}

/*****************************************************************************
* Name: freeQueue
* Purpose: remove and free any remaining tasks
* Imports: queue to free
* Exports: none
*****************************************************************************/
void freeQueue( ReadyQueue* queue )
{
    QueueNode* temp;
    if ( queue != NULL )
    {
        /* Aquire lock before making changes */
        pthread_mutex_lock( &queue->mutex );
        while (queue->head != NULL)
        {
            temp = queue->head;
            queue->head = temp->next;
            free(temp->data);
            free(temp);
        }
        pthread_mutex_unlock( &queue->mutex );
    }
}

/*****************************************************************************
* Name: enqueue
* Purpose: Insert a task into the ready queue
* Imports: Pointer to a ReadyQueue and pointer to task to insert
* Exports: returnCode, 0 for success, 2 for unknown error,
            3 for invalid queue, 4 for invalid task
* Comments: This is the only method to insert a Task due to it being a
*            critical section for ReadyQueue
*****************************************************************************/
int enqueue( ReadyQueue* queue, Task* inTask )
{
    QueueNode* newNd;
    int returnCode = 2;
    if ( queue == NULL ) { returnCode = 3; }
    else if ( inTask == NULL ) { returnCode = 4; }
    else
    {
        /* Critical section start */
        pthread_mutex_lock( &queue->mutex );
        /* Wait for space in the queue before inserting, this is guarateed to
            succeed as the CPUs will eventually pull a task and signal task*/
        while (!( queue->numNodes < queue->maxNodes ))
        {
            pthread_cond_wait( &queue->sigTask, &queue->mutex );
        }

        newNd = (QueueNode*)malloc( sizeof( QueueNode ) );
        newNd->data = inTask;
        newNd->next = NULL;

        if ( queue->head == NULL ) /* Insert first element */
        {
            queue->head = newNd;
            queue->tail = newNd;
        }
        else /* Insert Subsequent elements */
        {
            queue->tail->next = newNd;
            queue->tail = newNd;
        }
        queue->numNodes++;
        /* Exit Critical Section */

        returnCode = 0;
        /* Signal CPUs to grab a task and unlock mutex */
        pthread_cond_signal( &queue->sigCPU );
        pthread_mutex_unlock ( &queue->mutex );
    }
    return returnCode;
}

/*****************************************************************************
* Name: dequeue
* Purpose: Remove a task from the ready queue
* Imports: Pointer to a ReadyQueue and pointer to pointer for task export
* Exports: returnCode, 0 for success, 1 for empty queue, 2 for unknown error,
            3 for invalid queue.
* Comments: This is the only method to remove a Task due to it being a
*            critical section for ReadyQueue
*****************************************************************************/
int dequeue( ReadyQueue* queue, Task** outTask )
{
    QueueNode *tempNode;
    int returnCode = 2;
    if ( queue == NULL ) { returnCode = 3; }
    else
    {
        /* Enter Critical Section */
        pthread_mutex_lock( &queue->mutex );
        /* Wait for a task to pull, as this is not guarateed to mean there is
            a task if task() has terminated */
        if ( queue->numNodes <= 0 )
        {
            pthread_cond_wait( &queue->sigCPU, &queue->mutex );
        }
        /* In the event there is no task after signal, return to caller */
        if ( queue->numNodes <= 0 )
        {
            returnCode = 1;
        }
        else
        {

            /* 1 node in queue */
            if ( queue->head == queue->tail )
            {
                *outTask = queue->head->data;
                free( queue->head );
                queue->head = NULL;
                queue->tail = NULL;
                queue->numNodes = 0;
            }
            else /* More than 1 node in queue */
            {
                *outTask = queue->head->data;
                tempNode = queue->head;
                queue->head = tempNode->next;
                free( tempNode );
                queue->numNodes--;
            }
            returnCode = 0;
            /* Exit Critical Section */
        }
        /* Signal task() that a task has been removed (space in queue) */
        pthread_cond_signal( &queue->sigTask );
        pthread_mutex_unlock( &queue->mutex );
    }
    return returnCode;
}
