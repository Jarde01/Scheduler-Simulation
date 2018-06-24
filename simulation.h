//
// Created by Jeremy on 2018-06-23.
//

#ifndef A3_SIMULATION_H
#define A3_SIMULATION_H

enum SchedulerType{
    ROUND_ROBIN, PRIO_ROUND_ROBIN, SJF, STCF
};

struct ProcessElement{
    void * item;
    char * toString;
}p_element;

enum NewTaskElements{
    NAME, THREAD_TYPE, PRIORITY, LENGTH, IO_ODDS
};

enum priority {
    HIGH, MED, LOW
};

enum thread_type{
    SHORT, MEDIUM, LONG, IO
};

struct Process {
    char * name;
    enum thread_type ttype;
    enum priority prio;
    //char * priorityString;
    int timeRemaining;
    double io_odds;
};

enum thread_type get_thread_type(char * typeString);
enum priority get_priority(char * priorityString);
struct Process * new_process(char * tokens[]);
int tokenize(int args);
int scheduler_round_robin(struct Process * processes[], int numProcesses);
int scheduler_priority_round_robin(struct Process * processes[], int numProcesses);
int scheduler_shortest_job_first(struct Process * processes[], int numProcesses);
int scheduler_shortest_time_left_first(struct Process * processes[], int numProcesses);

void sort_ascending(struct Process * processes[], int numProcesses);
void print_allProcess( struct Process * processes[], int numProcesses );
void print_typeRunTime(int typeRunTime[]);
void print_priorityRunTime(int prioRunTime[]);
void print_runTimeTotals(int priorityRunningTime[], int typeRunningTime[]);

#endif //A3_SIMULATION_H
