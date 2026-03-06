#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *head = NULL;
static int tid_counter = 0;

void add(char *name, int priority, int burst)
{
    Task *newTask = malloc(sizeof(Task));
    if (newTask == NULL)
    {
        perror("malloc failure");
        exit(1);
    }

    newTask->name = strdup(name);
    if (newTask->name == NULL)
    {
        perror("strup failure");
        free(newTask);
        exit(1);
    }

    newTask->tid = tid_counter++;
    newTask->priority = priority;
    newTask->burst = burst;

    insert(&head, newTask);
}

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

void schedule()
{
    int time = 0;
    int n = tid_counter;

    if (n == 0)
    {
        perror("empty task list");
        exit(1);
    }

    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;

    int *first_start = malloc(sizeof(int) * n);
    if (!first_start)
    {
        perror("malloc failure");
        exit(1);
    }

    for (int i = 0; i < n; i++)
        first_start[i] = -1;

    while (head != NULL)
    {
        Task *t = pickNextTask();
        if (t == NULL)
            break;

        if (first_start[t->tid] == -1)
        {
            first_start[t->tid] = time;
        }

        int original_burst = t->burst;
        int slice = t->burst;

        run(t, slice);

        time += slice;
        t->burst -= slice;

        if (t->burst == 0)
        {
            int turnaround = time;
            int response = first_start[t->tid];
            int waiting = turnaround - original_burst;

            total_turnaround += (double)turnaround;
            total_response += (double)response;
            total_waiting += (double)waiting;

            delete(&head, t);
            free(t->name);
            free(t);
        }
        else
        {
            // Round Robin Stuff
        }
    }

    double avg_turnaround = total_turnaround / (double)n;
    double avg_waiting = total_waiting / (double)n;
    double avg_response = total_response / (double)n;

    printf("The average turnaround time = %.2f\n", avg_turnaround);
    printf("The average waiting time = %.2f\n", avg_waiting);
    printf("The average response time = %.2f\n", avg_response);

    free(first_start);
}