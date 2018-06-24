#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#include "simulation.h"

const static int TIME_SLICE = 5;


enum thread_type get_thread_type(char * typeString){
    int priorityInt = atoi(typeString);
    enum thread_type t;

    switch (priorityInt) {
        case 0:
            t = SHORT;
            break;
        case 1:
            t = MEDIUM;
            break;
        case 2:
            t = LONG;
            break;
        case 3:
            t = IO;
            break;
        default:
            break;
    }
    return t;
}

enum priority get_priority(char * priorityString){
    int priorityInt = atoi(priorityString);
    enum priority p;

    switch (priorityInt) {
        case 0:
            p = HIGH;
            break;
        case 1:
            p = MED;
            break;
        case 2:
            p = LOW;
            break;
        default:
            break;
    }
    return p;
}

struct Process * new_process(char * tokens[]){
    struct Process *process = (struct Process *) malloc(sizeof(struct Process));
    int processTime;
    int io_odds;

    processTime = atoi(tokens[LENGTH]);
    io_odds = atoi(tokens[IO_ODDS]);

    process->name = malloc(sizeof(tokens[NAME]));
    process->name = tokens[NAME];
    process->prio = get_priority(tokens[PRIORITY]);
    process->ttype = get_thread_type(tokens[THREAD_TYPE]);
    process->io_odds = io_odds;
    process->timeRemaining = processTime;

    return process;
}

int tokenize(int schedulerType){
    struct Process * processes[255];
    char * tokens[30];
    char buffer[255];
    FILE *fp;

    fp = fopen("processes.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    printf("\nRunning tokenizer for configuration file...\n");

    int processNumber = 0;
    while (fgets(buffer, 255, fp) != NULL){
        buffer[strcspn(buffer, "\r\n")] = 0; //remove unwanted characters

        // tokenize the incoming processes
        int i = 0;
        tokens[i] = strtok(buffer, " ");
        while(tokens[i]){
            tokens[++i] = strtok(NULL, " ");
        }

        // create the new process
        processes[processNumber] = new_process(tokens);
        processNumber++;

    }

    /*
    // TODO Diagnostic tools, can delete
    int priorityRunningTime[3] = {0};
    int typeRunningTime[4] = {0};
    int totalTimeForProcessing = 0;
    for (int u = 0; u<processNumber;u++){
        totalTimeForProcessing = totalTimeForProcessing+processes[u]->timeRemaining;
        priorityRunningTime[processes[u]->prio] = priorityRunningTime[processes[u]->prio] +processes[u]->timeRemaining;
        typeRunningTime[processes[u]->ttype] = typeRunningTime[processes[u]->ttype] +processes[u]->timeRemaining;
    }
    print_priorityRunTime(priorityRunningTime);
    print_typeRunTime(typeRunningTime);
    print_runTimeTotals(priorityRunningTime, typeRunningTime);
    */

    // use the selected scheduler algorithm
    switch (schedulerType){
        case 1:
            scheduler_round_robin(processes, processNumber);
            break;
        case 2:
            scheduler_priority_round_robin(processes, processNumber);
            break;
        case 3:
            scheduler_shortest_job_first(processes, processNumber);
            break;
        case 4:
            scheduler_shortest_time_left_first(processes, processNumber);
            break;
        default:
            scheduler_round_robin(processes, processNumber);
            break;
    }

    return 0;
}

int scheduler_round_robin(struct Process * processes[], int numProcesses){
    struct Process * p = NULL;
    struct Process * beginningPtr = *processes;
    int priorityRunningTime[3] = {0};
    int typeRunningTime[4] = {0};
    int ioTime = 0;
    int ioChance;
    time_t t;
    srand((unsigned) time(&t)); //init random number generator

    printf("\n=== Round Robin Scheduler ===\n");

    int programsLeft = numProcesses;
    while (programsLeft > 0){
        for (int i = 0; i< numProcesses; i++){
            p = processes[i];

            if (p->timeRemaining>0) {
                ioTime = 0;
                ioChance = rand() % 100;

                if (ioChance < p->io_odds) {
                    ioTime = rand() % TIME_SLICE; //determine length of time to run extra
                }

                // determine how long process will run extra, and add to stats
                if ( ioTime == 0) { // no io, full time slice
                    priorityRunningTime[p->prio] = priorityRunningTime[p->prio] + TIME_SLICE;
                    typeRunningTime[p->ttype] = typeRunningTime[p->ttype] + TIME_SLICE;

                    p->timeRemaining = p->timeRemaining - TIME_SLICE;
                }
                else { // only io time
                    priorityRunningTime[p->prio] = priorityRunningTime[p->prio] + ioTime;
                    typeRunningTime[p->ttype] = typeRunningTime[p->ttype] + ioTime;

                    p->timeRemaining = p->timeRemaining - ioTime;
                }

                // determine if the program is complete or not
                if (p->timeRemaining <= 0) {
                    programsLeft = programsLeft - 1;
                }
            }
        }
    }

    print_priorityRunTime(priorityRunningTime);
    print_typeRunTime(typeRunningTime);
    printf("\nPriority Total: %i\nType Total: %i\n", priorityRunningTime[0]+priorityRunningTime[1]+priorityRunningTime[2], typeRunningTime[0]+typeRunningTime[1]+typeRunningTime[2]+typeRunningTime[3]);
}

int scheduler_priority_round_robin(struct Process * processes[], int numProcesses){
    struct Process * p = NULL;
    struct Process * beginningPtr = *processes;
    int priorityRunningTime[3] = {0};
    int typeRunningTime[4] = {0};
    int ioTime = 0;
    int ioChance;
    bool done;
    int loopCount = 0;
    time_t t;
    srand((unsigned) time(&t)); //init random number generator

    printf("\n=== Priority Round Robin Scheduler ===\n");
    int currentPriorityLevel = 0;
    int programsLeft[5] = {0};

    for (int c = 0; c< numProcesses; c++){
        programsLeft[processes[c]->prio] = programsLeft[processes[c]->prio]+1;
    }

    while (!done && programsLeft[currentPriorityLevel] > 0) {
        for (int i = 0; i < numProcesses; i++) {
            p = processes[i];

            if (p->prio == currentPriorityLevel && p->timeRemaining > 0) {
                ioTime = 0;
                ioChance = rand() % 100;
                if (ioChance < p->io_odds) {
                    ioTime = rand() % TIME_SLICE; //determine length of time to run extra
                }

                // determine how long process will run extra, and add to stats
                if ( ioTime == 0) {
                    priorityRunningTime[p->prio] = priorityRunningTime[p->prio] + TIME_SLICE;
                    typeRunningTime[p->ttype] = typeRunningTime[p->ttype] + TIME_SLICE;

                    p->timeRemaining = p->timeRemaining - TIME_SLICE;
                }
                else {
                    priorityRunningTime[p->prio] = priorityRunningTime[p->prio] + ioTime;
                    typeRunningTime[p->ttype] = typeRunningTime[p->ttype] + ioTime;

                    p->timeRemaining = p->timeRemaining - ioTime;
                }

                // determine if the program is complete or not
                if (p->timeRemaining <= 0) {
                    programsLeft[p->prio] = programsLeft[p->prio] - 1;

                    if (programsLeft[currentPriorityLevel] <= 0){
                        currentPriorityLevel = currentPriorityLevel+1;

                        if ( currentPriorityLevel > LOW)
                            done = true;
                    }
                }
            }
        }
    }

    print_priorityRunTime(priorityRunningTime);
    print_typeRunTime(typeRunningTime);
    print_runTimeTotals(priorityRunningTime, typeRunningTime);
}

int scheduler_shortest_job_first(struct Process * processes[], int numProcesses){
    struct Process * temp = NULL;
    struct Process * beggingPtr = *processes;
    int ioTime = 0;
    int ioChance = 0;
    int priorityRunningTime[3] = {0};
    int typeRunningTime[4] = {0};

    printf("\n=== Shortest Job First Scheduler ===\n");

    // sort the jobs
    for (int i = 0; i < numProcesses; ++i)
    {
        for (int j = i + 1; j < numProcesses; ++j)
        {
            if (processes[i]->timeRemaining > processes[j]->timeRemaining)
            {
                temp =  processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    // go back to the beginning
    *processes = beggingPtr;

    for ( int i = 0; i< numProcesses; i++){
        while ( processes[i]->timeRemaining>0){
            ioTime = 0;
            ioChance = rand() % 100;
            if (ioChance < processes[i]->io_odds) {
                ioTime = rand() % TIME_SLICE; //determine length of time to run extra
            }
            // determine how long process will run extra, and add to stats
            if ( ioTime == 0) {
                priorityRunningTime[processes[i]->prio] = priorityRunningTime[processes[i]->prio] + TIME_SLICE;
                typeRunningTime[processes[i]->ttype] = typeRunningTime[processes[i]->ttype] + TIME_SLICE;

                processes[i]->timeRemaining = processes[i]->timeRemaining - TIME_SLICE;
            }
            else {
                priorityRunningTime[processes[i]->prio] = priorityRunningTime[processes[i]->prio] + ioTime;
                typeRunningTime[processes[i]->ttype] = typeRunningTime[processes[i]->ttype] + ioTime;

                processes[i]->timeRemaining = processes[i]->timeRemaining - ioTime;
            }
        }
    }

    print_priorityRunTime(priorityRunningTime);
    print_typeRunTime(typeRunningTime);
}

int scheduler_shortest_time_left_first(struct Process * processes[], int numProcesses){
    struct Process * temp = NULL;
    struct Process * beginningPtr = *processes;
    struct Process * p = NULL;
    struct Process * curr = NULL;
    int knownProcesses = 0;
    int ioTime = 0;
    int ioChance = 0;
    int priorityRunningTime[3] = {0};
    int typeRunningTime[4] = {0};
    int programsCompleted = 0;
    bool foundProcess = false;

    printf("\n=== Shortest Time Remaining Scheduler ===\n");

    while (programsCompleted < numProcesses){
        *processes = beginningPtr;

        // search for the first non negative time remaining
        p = processes[0];
        int pLoc = 0;
        while (p->timeRemaining<=0){
            p = processes[pLoc];
            pLoc = pLoc+1;
        }

        // we assume we are getting a new job every time slice
        for ( int k = 0; k< knownProcesses-1; k++) {
            if (processes[k]->timeRemaining > 0 ){
                curr = processes[k];

                // find the shortest job with non-zero time remaining
                if ( curr->timeRemaining >0 && curr->timeRemaining < p->timeRemaining)
                    p = curr;
            }
        }

        if ( p->timeRemaining > 0){
            ioTime = 0;
            ioChance = rand() % 100;
            if (ioChance < (int)p->io_odds) {
                ioTime = rand() % TIME_SLICE; //determine length of time to run extra
            }

            // determine how long process will run extra, and add to stats
            if ( ioTime == 0) {
                priorityRunningTime[p->prio] = priorityRunningTime[p->prio] + TIME_SLICE;
                typeRunningTime[p->ttype] = typeRunningTime[p->ttype] + TIME_SLICE;

                p->timeRemaining = p->timeRemaining - TIME_SLICE;
            }
            else {
                priorityRunningTime[p->prio] = priorityRunningTime[p->prio] + ioTime;
                typeRunningTime[p->ttype] = typeRunningTime[p->ttype] + ioTime;

                p->timeRemaining = p->timeRemaining - ioTime;
            }

            // determine if the program is complete or not
            if (p->timeRemaining <= 0){
                //printf("complete program %i, %s, %i \n", programsCompleted, p->name, p->timeRemaining);
                programsCompleted = programsCompleted + 1;
            }

            // get a new job from the processes list
            if (knownProcesses<numProcesses-1)
                knownProcesses = knownProcesses+1;
        }
    }

    *processes = beginningPtr;
    //print_allProcess(processes, numProcesses);

    print_priorityRunTime(priorityRunningTime);
    print_typeRunTime(typeRunningTime);
}

void print_allProcess( struct Process * processes[], int numProcesses ){
    for ( int i = 0; i< numProcesses; i++){
        printf("Process #%i, %s, %i\n", i, processes[i]->name, processes[i]->timeRemaining);
    }
}

void sort_ascending(struct Process * processes[], int numProcesses) {
    struct Process * temp = NULL;

    for (int i = 0; i < numProcesses; ++i)
    {
        for (int j = i + 1; j < numProcesses; ++j)
        {
            if (processes[i]->timeRemaining > processes[j]->timeRemaining)
            {
                temp =  processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }
}

void print_runTimeTotals(int priorityRunningTime[], int typeRunningTime[]){
    printf("\nPriority Total: %i\nType Total: %i\n",
           priorityRunningTime[0]+priorityRunningTime[1]+priorityRunningTime[2],
           typeRunningTime[0]+typeRunningTime[1]+typeRunningTime[2]+typeRunningTime[3]);
}

void print_typeRunTime(int typeRunTime[]){
    printf("\nType 0 run time: %i\nType 1 run time: "
           "%i\nType 2 run time: %i\nType 3 run time: %i\n",
           typeRunTime[0], typeRunTime[1], typeRunTime[2], typeRunTime[3] );
}

void print_priorityRunTime(int prioRunTime[]){
    printf("Priority 0 run time: %i\nPriority 1 run time: "
           "%i\nPriority 2 run time: %i\n",
           prioRunTime[0], prioRunTime[1], prioRunTime[2] );
}

int main(int argc, char * argv[]) {
    printf("%i, %s", argc, argv[0]);
    char buf[2];
    //strcpy(buf, args[2]);
    int schedule = atoi(argv[1]);

    printf("\nScheduler chosen: %i\n", schedule);
    tokenize(schedule);

    return 0;
}