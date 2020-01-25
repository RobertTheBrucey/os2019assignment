/*****************************************************************************
* File:      Logger.h
* Author:    Sean Ashton * Student ID: 
* Unit:      COMP2006 Operating Systems
* Purpose:   Log to file with mutual exclusion
* Reference: None.
* Comments:
* Requires:
* Created:   11/04/2019 * Last Modified: 28/04/2019
*****************************************************************************/
#ifndef LOGGER_H
#define LOGGER_H 1
#endif
#define FALSE 0
#define TRUE !FALSE

/* Data Structures */
typedef struct Logger {
    pthread_mutex_t mutex;
    char *logfile;
    FILE *out;
} Logger;

/* Function Prototypes */
int initLogger( Logger* );
int writeLog( Logger*, char* );
