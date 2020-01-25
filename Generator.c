/*****************************************************************************
* File:      Generator.c
* Author:    Sean Ashton * Student ID: 14866636
* Unit:      COMP2006 Operating Systems
* Purpose:   Generate a task list for use with the scheduler simulation
* Reference: None.
* Comments:
* Requires:
* Created:   26/04/2019 * Last Modified: 28/04/2019
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*****************************************************************************
* Name: main
* Purpose: main
* Imports: command line arguments
* Exports: error code
*****************************************************************************/
int main( int argc, char** argv )
{
    int ii, randInt;
    /* Unknown error set */
    int returnCode = -1;
    /* Read command line arguments */
    if (argc != 3)
    {
        printf("Incorrect number of arguments, Usage:\n");
        printf("generator numTasks maxCPUBurst\n");
    }
    else
    {
        FILE *out = fopen( "taskList.txt", "w" );
        /* Seed random generator */
        srand(time(NULL));
        if (out != NULL)
        {
            /* Generate each line, indexed starting at 1 */
            for (ii = 1; ii <= atoi(argv[1]); ii++)
            {
                randInt = (rand() % atoi(argv[2])) + 1;
                fprintf( out, "%d %d\n", ii, randInt );
            }
            returnCode = 0;
            fclose( out );
        }
        else
        {
            printf( "Problem opening output file taskList.txt" );
            returnCode = 1;
        }
    }
    return returnCode;
}
