// Maxim Diab & Thor Snyder
// CSC 139
// Spring 2026
// Scheduler Round Robin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *head = NULL;
static int tid_counter = 0;

//Function for adding tasks to the end of the list for Round Robin
static void insert_tail(struct node **head, Task *task) {
    //New Node Malloc
    struct node *newNode = malloc(sizeof(struct node));
    if(!newNode) {
        perror("malloc failure");
        exit(1);
    }
    
    //set newNode to the incoming task
    newNode->task = task;
    newNode->next = NULL;
    
    //Check for empty list
    if(*head == NULL) {
        *head = newNode;
        return;
    }
    
    //Move to the end of the list to add the task
    struct node *cur = *head;
    while(cur->next != NULL) cur = cur->next;
    cur->next = newNode;
}


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
    insert_tail(&head, newTask);
}

//Very simple ternary operator for picking the next task for Round Robin
Task *pickNextTask() {
    return head ? head->task : NULL;
}

//Function for scheduling the next task
void schedule()
{
    int time = 0;
    int n = tid_counter;

    if (n == 0)
    {
        printf("empty list\n");
        return;
    }

    //Set time variables to 0
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;

    //Allocate memory for start time and storing burst values
    int *first_start = malloc(sizeof(int) * n);
    int *original_burst = malloc(sizeof(int) * n);
    if (!first_start || !original_burst)
    {
        perror("malloc failure");
        free(first_start);
        free(original_burst);
        exit(1);
    }

    //Set all start times to -1
    for (int i = 0; i < n; i++) first_start[i] = -1;
    
    //Save all the original burst data
    for (struct node *p = head; p != NULL; p = p->next) {
        original_burst[p->task->tid] = p->task->burst;
    }
    
    while (head != NULL)
    {
        Task *t = pickNextTask();
        if (t == NULL) break;

        //Set start time if not already set
        if (first_start[t->tid] == -1) first_start[t->tid] = time;

        //Set slice to full burst or the QUANTUM value set in cpu.h
        int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;

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
            int waiting = turnaround - original_burst[t->tid];

            //Set total execution metrics
            total_turnaround += (double)turnaround;
            total_response += (double)response;
            total_waiting += (double)waiting;

            //Free memory
            deleteTask(&head, t);
            free(t->name);
            free(t);
        }
        else
        {
            // Requeue to the tail
            deleteTask(&head,t);
            insert_tail(&head,t);
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
    free(original_burst);
}
