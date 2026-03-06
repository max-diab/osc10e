#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

//Initialize head for linked list and task id counter
struct node *head = NULL;
static int tid_counter = 0;

//Function for adding a task
void add(char *name, int priority, int burst)
{
    //New task malloc
    Task *newTask = malloc(sizeof(Task));
    if (newTask == NULL)
    {
        perror("malloc failure");
        exit(1);
    }

    //Gives the task ownership of its name string
    newTask->name = strdup(name);
    if (newTask->name == NULL)
    {
        perror("strdup failure");
        free(newTask);
        exit(1);
    }

    //Assign new task information
    newTask->tid = tid_counter++;
    newTask->priority = priority;
    newTask->burst = burst;

    //Add the new task to the linked list
    insert(&head, newTask);
}

//Returns the task at the head of the list, sets a new head
Task *pickNextTask()
{
    if (head == NULL)
        return NULL;

    struct node *temp = head;
    struct node *next = head->next;

    while (next != NULL)
    {
        if (temp->task->tid > next->task->tid)
        {
            temp = next;
        }
        next = next->next;
    }

    return temp->task;
}

//Function for scheduling the next task
void schedule()
{
    int time = 0;
    int n = tid_counter;

    if (n == 0)
    {
        perror("empty task list");
        exit(1);
    }

    //Set time variables to 0
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;

    //Allocate memory for start time
    int *first_start = malloc(sizeof(int) * n);
    if (!first_start)
    {
        perror("malloc failure");
        exit(1);
    }

    //Set all start times to -1
    for (int i = 0; i < n; i++)
        first_start[i] = -1;

    while (head != NULL)
    {
        Task *t = pickNextTask();
        if (t == NULL)
            break;

        //Set start time if not already set
        if (first_start[t->tid] == -1)
        {
            first_start[t->tid] = time;
        }

        //Store burst information, set slice to full burst for fcfs
        int original_burst = t->burst;
        int slice = t->burst;

        //Run the task
        run(t, slice);

        //Update time information, reduce remaining time for the task (burst)
        time += slice;
        t->burst -= slice;

        if (t->burst == 0)
        {
            //Set task execution metrics
            int turnaround = time;
            int response = first_start[t->tid];
            int waiting = turnaround - original_burst;

            //Set total execution metrics
            total_turnaround += (double)turnaround;
            total_response += (double)response;
            total_waiting += (double)waiting;

            //Free memory
            delete(&head, t);
            free(t->name);
            free(t);
        }
        else
        {
            // Round Robin Stuff
        }
    }

    //Calculate and print the average execution metrics
    double avg_turnaround = total_turnaround / (double)n;
    double avg_waiting = total_waiting / (double)n;
    double avg_response = total_response / (double)n;

    printf("The average turnaround time = %.2f\n", avg_turnaround);
    printf("The average waiting time = %.2f\n", avg_waiting);
    printf("The average response time = %.2f\n", avg_response);

    //Free memory
    free(first_start);
}