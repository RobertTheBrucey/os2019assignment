/*****************************************************************************
* File:      Scheduler.h
* Author:    Sean Ashton * Student ID: 
* Unit:      COMP2006 Operating Systems
* Purpose:   Simulate a CPU Scheduler
* Reference: None.
* Comments:
* Requires:
* Created:   10/04/2019 * Last Modified: 28/04/2019
*****************************************************************************/
#ifndef SCHEDULER_H
#define SCHEDULER_H 1
#endif
#define FALSE 0
#define TRUE !FALSE

/* Data structures */
typedef struct MutexVars {
    pthread_mutex_t mutex;
    int num_tasks;
    int total_waiting_time;
    int total_turnaround_time;
} MutexVars;

typedef struct CPU {
    int id;
    pthread_t *task_ptr;
    pthread_t thisThread;
    MutexVars *mutexVars;
    Logger* logger;
    ReadyQueue* queue;
} CPU;

typedef struct Tasker {
    ReadyQueue* queue;
    char* fileName;
    Logger* logger;
} Tasker;
