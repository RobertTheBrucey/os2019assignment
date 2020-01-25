/*****************************************************************************
* File:      Logger.c
* Author:    Sean Ashton * Student ID: 14866636
* Unit:      COMP2006 Operating Systems
* Purpose:   Log to file with mutual exclusion
* Reference: None.
* Comments:
* Requires:
* Created:   11/04/2019 * Last Modified: 28/04/2019
*****************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Logger.h"

/*****************************************************************************
* Name: initLogger
* Purpose: Initialise a logger to log events
* Imports: Pointer to logger struct
* Exports: return code
*****************************************************************************/
int initLogger( Logger* logger )
{
    int returnCode = -1;
    if ( logger != NULL )
    {
        pthread_mutex_init(&logger->mutex, NULL);
        logger->logfile = "simulation_log";
        logger->out = NULL;
        returnCode = 0;
    }
    return returnCode;
}

/*****************************************************************************
* Name: writeLog
* Purpose: write a log string to the simulation log file
* Imports: logger, string to write
* Exports: return code
*****************************************************************************/
int writeLog( Logger* logger, char *string )
{
    int returnCode = -1;
    if ( logger == NULL) { returnCode = 1; }
    else if ( string == NULL ) { returnCode = 2; }
    else
    {
        /* Mutex for writing to file */
        pthread_mutex_lock( &logger->mutex );

        if ( logger->out == NULL )
        {
            logger->out = fopen( logger->logfile, "a" );
        }
        fprintf( logger->out, "%s", string );
        /* Flush file IO before releasing mutex lock */
        fflush( logger->out );

        pthread_mutex_unlock( &logger->mutex );
    }
    return returnCode;
}
