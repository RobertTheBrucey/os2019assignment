/*****************************************************************************
* File:      Scheduler.c
* Author:    Sean Ashton * Student ID: 
* Unit:      COMP2006 Operating Systems
* Purpose:   Simulate a CPU Scheduler
* Reference: None.
* Comments:
* Requires:
* Created:   10/04/2019 * Last Modified: 01/05/2019
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "Logger.h"
#include "ReadyQueue.h"
#include "Scheduler.h"

#define CPUCOUNT 3
#define LOGSTART "Statistics for CPU-%d:\nTask %d\nArrival time: %s\n\
Service time: %s\n"
#define LOGEND "Statistics for CPU-%d:\nTask %d\nArrival time: %s\n\
Completion time: %s\n"
#define TASKEND "Number of tasks put into the Ready-Queue: %d\nTerminate at \
time %s\n"
#define TASKSTART "Statistics for TASK:\nTask %d; Burst %d\nArrival time: %s\
\n"
#define SCHEDEND "Number of tasks: %d\nAverage waiting time: %d seconds\n\
Average turn around time: %d seconds\n"
#define READLEN 100

/* Forward Declarations */
void *task( void* );
void *cpu( void* );
int isInteger( char* string );

/*****************************************************************************
* Name: main
* Purpose: main
* Imports: command line arguments
* Exports: error code
*****************************************************************************/
int main( int argc, char** argv )
{
    int ii;
    char string[READLEN];
    /* Unknown error set */
    int returnCode = -1;
    /* Read command line arguments */
    if (argc != 3)
    {
        printf( "Incorrect number of arguments, Usage:\n" );
        printf( "scheduler inputFile queueLength\n" );
    }
    else
    {
        if ( atoi(argv[2]) < 1 || atoi(argv[2]) > 10 )
        {
            printf( "queueLength must be between 1 and 10\n" );
        }
        else
        {
            /* Initialise Ready-Queue, 3x CPUs and Log Writer */
            ReadyQueue *queue = (ReadyQueue*)malloc(sizeof(ReadyQueue));
            initQueue( queue );
            queue->maxNodes = atoi( argv[2] );

            Logger *logger = (Logger*)malloc(sizeof(Logger));
            initLogger( logger );

            /* Initialise shared variables num_tasks, total_waiting_time and
               total_turnaround_time to be shared with CPUs init to 0 */
            MutexVars *sharedVars = (MutexVars*)malloc(sizeof(MutexVars));
            pthread_mutex_init( &sharedVars->mutex, NULL );
            sharedVars->num_tasks = 0;
            sharedVars->total_waiting_time = 0;
            sharedVars->total_turnaround_time = 0;


            /* Task thread */
            pthread_t *taskPtr = (pthread_t*)malloc(sizeof(pthread_t));
            Tasker *tasker = (Tasker*)malloc(sizeof(Tasker));
            tasker->queue = queue;
            tasker->fileName = argv[1];
            tasker->logger = logger;

            /* CPU threads */
            CPU *cpus[CPUCOUNT];
            for ( ii = 0; ii < CPUCOUNT; ii++ )
            {
                cpus[ii] = (CPU*)malloc(sizeof(CPU));
                cpus[ii]->id = (ii+1);
                cpus[ii]->task_ptr = taskPtr;
                cpus[ii]->mutexVars = sharedVars;
                cpus[ii]->logger = logger;
                cpus[ii]->queue = queue;
            }

            writeLog( logger, "Simulation starting.\n" );
            /* Spawn task and cpu threads and join them */
            pthread_create( taskPtr, NULL, task, (void*)tasker );
            for ( ii = 0; ii < CPUCOUNT; ii++ )
            {
                pthread_create( &cpus[ii]->thisThread, NULL, cpu,
                                                        (void*)cpus[ii] );
            }
            /* Join task thread to wait for task() to terminate, then
                signal cpu() threads to terminate */
            pthread_join( *taskPtr, NULL );
            for ( ii = 0; ii < CPUCOUNT; ii++ )
            {
                cpus[ii]->task_ptr = NULL;
            }
            /* Wake up all cpu() threads now that task() has terminated */
            pthread_cond_broadcast( &queue->sigCPU );
            /* Join and wait */
            for ( ii = 0; ii < CPUCOUNT; ii++ )
            {
                pthread_join( cpus[ii]->thisThread, NULL );
            }
            /* Print terminating statistics */
            if ( sharedVars->num_tasks == 0 )
            {
                sprintf( string,
                            "No tasks were read, statistics unavailable\n" );
            }
            else
            {
                sprintf( string, SCHEDEND, sharedVars->num_tasks,
                    sharedVars->total_waiting_time/sharedVars->num_tasks,
                    sharedVars->total_turnaround_time/sharedVars->num_tasks );
            }
            writeLog( logger, string );
            /* Close log and free all allocated memory */
            if ( logger->out != NULL )
            {
                fclose( logger->out );
                logger->out = NULL;
            }
            freeQueue( queue );
            free( queue );
            free( logger );
            free( sharedVars );
            free( taskPtr );
            free( tasker );
            for ( ii = 0; ii < CPUCOUNT; ii++ )
            {
                free( cpus[ii] );
            }
        }
    }
    return returnCode;
}

/*****************************************************************************
* Name: task
* Purpose: read tasks from file and add them to queue
* Imports: pointer to cpu memory struct
* Exports: none
* Comments:
*****************************************************************************/
void *task( void *voidQueue )
{
    Tasker *tasker = (Tasker*)voidQueue;
    ReadyQueue *queue = tasker->queue;
    char *fileName = tasker->fileName;
    char *sPtr;
    char sTask1[READLEN];
    char sTask2[READLEN];
    char arriveTime[READLEN];
    char string[READLEN];
    char string2[READLEN];
    Task *task;
    FILE *in;
    int done = FALSE;
    int ii, errCode, nRead;
    int taskCount = 0;
    if ( fileName != NULL )
    {
        in = fopen( fileName, "r" );
        if ( in != NULL )
        {
            while ( !done ) /* Continue while there are more tasks */
            {
                if ( fgets( sTask1, READLEN, in ) == NULL )
                {
                    done = 2; /* No tasks read this loop */
                }
                else if ( fgets( sTask2, READLEN, in ) == NULL )
                {
                    done = 1; /* One task read */
                }
                for ( ii = 0; ii < ( 2 - done ); ii++ )
                {
                    if ( ii == 0 )
                    {
                        sPtr = sTask1;
                    }
                    else
                    {
                        sPtr = sTask2;
                    }
                    /* Input validation, skip invalid lines */
                    nRead = sscanf( sPtr, "%s %s", string, string2 );
                    if ( isInteger( string ) && isInteger( string2 ) &&
                                                                nRead == 2)
                    {
                        task = (Task*)malloc(sizeof(Task));
                        sscanf( sPtr, "%d %d", &task->id, &task->cpu_burst );
                        task->arriveTime = time(NULL);

                        errCode = enqueue( queue, task );
                        taskCount++;
                        if ( errCode != 0 )
                        {//Has error codes to handle
                            printf( "Queue error %d\n", errCode );
                        }

                    /*Log to file*/
                        strftime( arriveTime, READLEN, "%H:%M:%S",
                                    localtime(&(task->arriveTime)) );
                        sprintf( string, TASKSTART,
                                    task->id, task->cpu_burst, arriveTime );
                        writeLog( tasker->logger, string );
                    }
                    else
                    {
                        writeLog( tasker->logger,
                                    "Invalid line in input file\n" );
                    }
                }
            }
            fclose( in ); /* Close file now that reading is complete */
        }
        else
        {
            printf( "Unable to open input file\n" );
        }
    }
    else
    {
        printf( "Unable to open input file\n" );
    }
    /* End of computation log */
    time_t rawtime = time( NULL );
    strftime( arriveTime, READLEN, "%H:%M:%S",
                localtime(&rawtime) );
    sprintf( string, TASKEND, taskCount, arriveTime );
    writeLog( tasker->logger, string );
    return NULL;
}

/*****************************************************************************
* Name: cpu
* Purpose: Simulate a CPU by taking a task and sleeping for cpu burst time
* Imports: cpu_r struct, shared variables
* Exports: none
* Comments: Should be done
*****************************************************************************/
void *cpu( void* CPUPtr )
{
    CPU *cpu_r = (CPU*)CPUPtr;
    MutexVars *sharedVars = cpu_r->mutexVars;
    Task *task_r = NULL;
    Task **task_rr = &task_r;
    char string[READLEN];
    char arriveTime[READLEN];
    char formatTime[READLEN];
    double temp_time = 0.0;
    int totalTasks = 0;
    int errCode;

    while (cpu_r->task_ptr != NULL) /* While task() exists */
    {
        /* Thread sync is handled in the dequeue method */
        errCode = dequeue( cpu_r->queue, task_rr );
        if ( errCode == 0 )
        {
            temp_time = difftime( time(NULL), task_r->arriveTime );
            /* Log after taking task from queue */
            strftime( arriveTime, READLEN, "%H:%M:%S",
                        localtime(&task_r->arriveTime) );
            time_t rawtime = time( NULL );
            strftime( formatTime, READLEN, "%H:%M:%S", localtime(&rawtime) );

            sprintf( string, LOGSTART, cpu_r->id, task_r->id, arriveTime,
                    formatTime );
            writeLog( cpu_r->logger, string );

            /* Critical section for shared variable updates */
            pthread_mutex_lock( &sharedVars->mutex );
            sharedVars->num_tasks++;
            sharedVars->total_waiting_time += (int)temp_time;
            pthread_mutex_unlock( &sharedVars->mutex );

            /* Simulate compute */
            sleep( task_r->cpu_burst/10 );

            /* Log after completion of task */
            temp_time = difftime( time(NULL), task_r->arriveTime );
            strftime( arriveTime, 50, "%H:%M:%S",
                        localtime(&task_r->arriveTime) );
            rawtime = time( NULL );
            strftime( formatTime, 50, "%H:%M:%S", localtime(&rawtime) );
            sprintf( string, LOGEND, cpu_r->id, task_r->id, arriveTime,
                    formatTime );
            writeLog( cpu_r->logger, string );

            /* Free task memory after completion */
            free( task_r );
            task_r = NULL;

            totalTasks++;
            /* Critical section for shared variable updates */
            pthread_mutex_lock( &sharedVars->mutex );
            sharedVars->total_turnaround_time += (int)temp_time;
            pthread_mutex_unlock( &sharedVars->mutex );
        }
        else
        {
            /* printf( "CPU %d Failed to get a task\n", cpu_r->id ); */
        }
    }
    /* Finished compute, log statistics for this CPU */
    sprintf( string, "CPU-%d terminates after servicing %d tasks\n",
                cpu_r->id, totalTasks );
    writeLog( cpu_r->logger, string );
    return NULL;
}

/*****************************************************************************
* Name: isInteger
* Purpose: Check if the given input string is an integer or not
* Imports: String to check
* Exports: boolean
* Comments
*****************************************************************************/
int isInteger( char* string )
{
    int number = TRUE;
    int ii = 0;
    while ( number && string[ii]!='\0' )
    {
        if ( isdigit( string[ii] ) == 0 )
        {
            number = FALSE;
        }
        ii++;
    }
    return number;
}
