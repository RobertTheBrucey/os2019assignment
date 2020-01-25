Sean Ashton OS Assignment readme

Date created: 10/04/2019

Date last modified: 01/05/2019

Compilation instructions:
To compile the scheduler run 'make' or 'make scheduler'
To compile the task list generator run 'make generator'

Purpose: Simulate a Multi-Processor Scheduler

Files in project:
Generator.c - Task file generator utility
Logger.c - Mutex capable logger
Logger.h - Header for Logger.c
Makefile - Make file, includes clean and generator targets
ReadyQueue.c - Mutex capable Queue for tasks
ReadyQueue.h - Header for ReadyQueue
Scheduler.c - Main Program file
Scheduler.h - Header for Scheduler.c 
readme.txt - This file
testHarness.sh - Runs all files in TestData at Queue Size 1 and 10

To Do:
None

Test Files:
TestData/ - Test files for various inputs

Functionality:
Take an input file of tasks, and queue length and simulate a FCFS scheduler

Known bugs
None

Additional functionality:
None
