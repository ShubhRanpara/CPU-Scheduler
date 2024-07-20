#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// Process structure
struct process
{
    int pid;             // process ID
    int burst_time;      // burst time
    int priority;        // priority
    int arrival_time;    // arrival time
    int start_time;      // start time
    int finish_time;     // finish time
    int waiting_time;    // waiting time
    int turnaround_time; // turnaround time
    int remaining_time;  // remaining time for Shortest Remaining Time First and Longest Remaining Time First
    int completed;       // Flag to indicate if the process has completed execution
    int preempted;       // Flag to indicate if the process was preempted during execution
};

// Function prototypes
void fcfs(struct process *processes, int n, char *outputFilename);
void sjf(struct process *processes, int n, char *outputFilename);
void rr(struct process *processes, int n, int quantum, char *outputFilename);
void priority(struct process *processes, int n, char *outputFilename);
void srtf(struct process *processes, int n, char *outputFilename);
void lrtf(struct process *processes, int n, char *outputFilename);
// void print_gantt_chart(struct process *processes, int n);
void compute_avg_times(struct process *processes, int n, float *avg_turnaround_time, float *avg_waiting_time);

// Function to read input from file and create an array of processes
struct process *read_input(char *filename, int *n)
{
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL)
    {
        printf("\n --> Error: Unable to open input file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(input_file, "%d", n);
    // *n -> number of processes mention in the file
    struct process *processes = (struct process *)malloc(*n * sizeof(struct process));

    for (int i = 0; i < *n; i++)
    {
        fscanf(input_file, "%d %d %d %d", &processes[i].pid, &processes[i].arrival_time, &processes[i].burst_time, &processes[i].priority);
        processes[i].start_time = -1;                          // initialize to -1
        processes[i].finish_time = -1;                         // initialize to -1
        processes[i].waiting_time = -1;                        // initialize to 0
        processes[i].turnaround_time = -1;                     // initialize to 0
        processes[i].remaining_time = processes[i].burst_time; // initialize remaining time
        processes[i].completed = 0;
        processes[i].preempted = 0;
    }

    fclose(input_file);
    return processes;
}

// Function to compute average turnaround time and average waiting time
void compute_avg_times(struct process *processes, int n, float *avg_turnaround_time, float *avg_waiting_time)
{
    *avg_turnaround_time = 0;
    *avg_waiting_time = 0;
    for (int i = 0; i < n; i++)
    {
        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        *avg_turnaround_time += processes[i].turnaround_time;
        *avg_waiting_time += processes[i].waiting_time;
    }
    *avg_turnaround_time /= n;
    *avg_waiting_time /= n;
}

// Function to simulate the First-Come First-Served (FCFS) CPU scheduling algorithm
void fcfs(struct process *processes, int n, char *outputFilename)
{
    FILE *fpout = fopen(outputFilename, "w");
    if (fpout == NULL)
    {
        printf("\n --> Error: Unable to open output file %s\n", outputFilename);
        exit(1);
    }

    // Print Gantt chart
    fprintf(fpout, "Gantt Chart:\n");

    printf("\n --> Running First-Come First-Served (FCFS) CPU scheduling algorithm...\n");
    int current_time = 0;
    for (int i = 0; i < n; i++)
    {
        if (current_time < processes[i].arrival_time)
        {
            for (int idle = 1; idle <= processes[i].arrival_time - current_time; idle++)
            {
                fprintf(fpout, "| Idle ");
            }
            current_time = processes[i].arrival_time;
        }
        printf(" Processing P%d...\n", processes[i].pid);

        processes[i].finish_time = current_time + processes[i].burst_time;
        current_time = processes[i].finish_time;

        for (int k = 1; k <= processes[i].burst_time; k++)
        {
            fprintf(fpout, "| P%d ", processes[i].pid);
        }
    }
    fprintf(fpout, "\n\n");
    printf("\n --> All processes have finished executing.\n");
    float avg_turnaround_time, avg_waiting_time;
    compute_avg_times(processes, n, &avg_turnaround_time, &avg_waiting_time);
    printf("\n --> Average turnaround time: %.2f\n", avg_turnaround_time);
    printf(" --> Average waiting time: %.2f\n", avg_waiting_time);

    // Get the output in output file
    fprintf(fpout, "Average turn-around time: %.2f\n", avg_turnaround_time);
    fprintf(fpout, "Average waiting time: %.2f\n\n", avg_waiting_time);
    fclose(fpout);
}

// Function to simulate the Shortest Job First (SJF) CPU scheduling algorithm
void sjf(struct process *processes, int n, char *outputFilename)
{
    FILE *fpout = fopen(outputFilename, "w");
    if (fpout == NULL)
    {
        printf("\n --> Error: Unable to open output file %s\n", outputFilename);
        exit(1);
    }

    // Print Gantt chart
    fprintf(fpout, "Gantt Chart:\n");

    printf("\n --> Running Shortest Job First (SJF) CPU scheduling algorithm...\n");
    int current_time = 0, completed = 0;
    struct process *shortest_job = NULL;
    while (completed < n)
    {
        shortest_job = NULL;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                if (shortest_job == NULL || processes[i].burst_time < shortest_job->burst_time)
                {
                    shortest_job = &processes[i];
                }
            }
        }
        if (shortest_job == NULL)
        {
            current_time++;
            fprintf(fpout, "| Idle ");
        }
        else
        {
            for (int i = 1; i <= shortest_job->burst_time; i++)
            {
                fprintf(fpout, "| P%d ", shortest_job->pid);
                shortest_job->remaining_time--;
                current_time++;
            }
            printf("Processing P%d...\n", shortest_job->pid);
            shortest_job->finish_time = current_time + 1;
            completed++;
        }
    }
    fprintf(fpout, "\n\n");
    printf("\n --> All processes have finished executing.\n");

    float avg_turnaround_time, avg_waiting_time;
    compute_avg_times(processes, n, &avg_turnaround_time, &avg_waiting_time);
    printf("\n --> Average turnaround time: %.2f\n", avg_turnaround_time);
    printf(" --> Average waiting time: %.2f\n", avg_waiting_time);

    // Get the output in output file
    fprintf(fpout, "Average turn-around time: %.2f\n", avg_turnaround_time);
    fprintf(fpout, "Average waiting time: %.2f\n\n", avg_waiting_time);
    fclose(fpout);
}

// Function to simulate the Round Robin (RR) CPU scheduling algorithm
// This algorithm still under development
void rr(struct process *processes, int n, int quantum, char *outputFilename)
{
    FILE *fpout = fopen(outputFilename, "w");
    if (fpout == NULL)
    {
        printf("\n --> Error: Unable to open output file %s\n", outputFilename);
        exit(1);
    }

    // Print Gantt chart
    fprintf(fpout, "Gantt Chart:\n");

    printf("\n --> Running Round Robin (RR) CPU scheduling algorithm with quantum %d...\n", quantum);
    int current_time = 0, completed = 0;
    int *remaining_time = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        remaining_time[i] = processes[i].burst_time;
    }
    while (completed < n)
    {
        int flag = 1; // to check idle condition
        for (int i = 0; i < n; i++)
        {
            if (remaining_time[i] > 0 && processes[i].arrival_time <= current_time)
            {
                flag = 0;
                if (remaining_time[i] > quantum)
                {
                    for (int k = 1; k <= quantum; k++)
                    {
                        fprintf(fpout, "| P%d ", processes[i].pid);
                    }
                    printf("Processing P%d...\n", processes[i].pid);
                    remaining_time[i] -= quantum;
                    current_time += quantum;
                }
                else
                {
                    for (int k = 1; k <= processes[i].remaining_time; k++)
                    {
                        fprintf(fpout, "| P%d ", processes[i].pid);
                    }
                    printf("Processing P%d...\n", processes[i].pid);
                    current_time += remaining_time[i];
                    processes[i].finish_time = current_time;
                    remaining_time[i] = 0;
                    completed++;
                }
            }
        }
        if (flag)
        {
            current_time++;
            fprintf(fpout, "| Idle ");
        }
    }
    fprintf(fpout, "\n\n");

    printf("\n --> All processes have finished executing.\n");
    // print_gantt_chart(processes, n);
    float avg_turnaround_time, avg_waiting_time;
    compute_avg_times(processes, n, &avg_turnaround_time, &avg_waiting_time);
    printf("\n --> Average turnaround time: %.2f\n", avg_turnaround_time);
    printf(" --> Average waiting time: %.2f\n", avg_waiting_time);
    free(remaining_time);

    // Get the output in output file
    fprintf(fpout, "Average turn-around time: %.2f\n", avg_turnaround_time);
    fprintf(fpout, "Average waiting time: %.2f\n\n", avg_waiting_time);
    fclose(fpout);
}

// Function to simulate the Priority Scheduling CPU scheduling algorithm
void priority(struct process *processes, int n, char *outputFilename)
{
    FILE *fpout = fopen(outputFilename, "w");
    if (fpout == NULL)
    {
        printf("\n --> Error: Unable to open output file %s\n", outputFilename);
        exit(1);
    }

    // Print Gantt chart
    fprintf(fpout, "Gantt Chart:\n");

    printf("Running Priority Scheduling CPU scheduling algorithm...\n");
    int current_time = 0, completed = 0;
    while (completed < n)
    {
        int min_priority = INT_MAX, index = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && !processes[i].completed && processes[i].priority < min_priority)
            {
                min_priority = processes[i].priority;
                index = i;
            }
        }
        if (index == -1)
        {
            fprintf(fpout, "| Idle ");
            current_time++;
            continue;
        }

        for (int k = 1; k <= processes[index].burst_time; k++)
        {
            fprintf(fpout, "| P%d ", processes[index].pid);
        }
        printf("Processing P%d...\n", processes[index].pid);
        processes[index].start_time = current_time;
        processes[index].finish_time = current_time + processes[index].burst_time;
        processes[index].waiting_time = processes[index].start_time - processes[index].arrival_time;
        processes[index].turnaround_time = processes[index].finish_time - processes[index].arrival_time;
        current_time = processes[index].finish_time;
        processes[index].completed = 1;
        completed++;
    }

    fprintf(fpout, "\n\n");
    printf("\n --> All processes have finished executing.\n");
    // print_gantt_chart(processes, n);
    float avg_turnaround_time, avg_waiting_time;
    compute_avg_times(processes, n, &avg_turnaround_time, &avg_waiting_time);
    printf("\n --> Average turnaround time: %.2f\n", avg_turnaround_time);
    printf(" --> Average waiting time: %.2f\n", avg_waiting_time);

    // Get the output in output file
    fprintf(fpout, "Average turn-around time: %.2f\n", avg_turnaround_time);
    fprintf(fpout, "Average waiting time: %.2f\n\n", avg_waiting_time);
    fclose(fpout);
}

// Function to simulate the Shortest Remaining Time First (SRTF) CPU scheduling algorithm
void srtf(struct process *processes, int n, char *outputFilename)
{
    FILE *fpout = fopen(outputFilename, "w");
    if (fpout == NULL)
    {
        printf("\n --> Error: Unable to open output file %s\n", outputFilename);
        exit(1);
    }

    // Print Gantt chart
    fprintf(fpout, "Gantt Chart:\n");

    printf("\n --> Running Shortest Remaining Time First (SRTF) CPU scheduling algorithm...\n");
    int current_time = 0, completed = 0;
    while (completed < n)
    {
        int min_burst_time = INT_MAX, index = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && !processes[i].completed && processes[i].burst_time < min_burst_time)
            {
                min_burst_time = processes[i].burst_time;
                index = i;
            }
        }
        if (index == -1)
        {
            fprintf(fpout, "| Idle ");
            current_time++;
            continue;
        }
        fprintf(fpout, "| P%d ", processes[index].pid);
        printf("Processing P%d...\n", processes[index].pid);
        processes[index].start_time = current_time;
        processes[index].burst_time--;
        current_time++;
        if (processes[index].burst_time == 0)
        {
            processes[index].finish_time = current_time;
            processes[index].waiting_time = processes[index].start_time - processes[index].arrival_time;
            processes[index].turnaround_time = processes[index].finish_time - processes[index].arrival_time;
            processes[index].completed = 1;
            completed++;
        }
    }

    fprintf(fpout, "\n\n");
    printf("\n --> All processes have finished executing.\n");

    float avg_turnaround_time, avg_waiting_time;
    compute_avg_times(processes, n, &avg_turnaround_time, &avg_waiting_time);

    printf("\n --> Average turnaround time: %.2f\n", avg_turnaround_time);
    printf(" --> Average waiting time: %.2f\n", avg_waiting_time);

    // Get the output in output file
    fprintf(fpout, "Average turn-around time: %.2f\n", avg_turnaround_time);
    fprintf(fpout, "Average waiting time: %.2f\n\n", avg_waiting_time);
    fclose(fpout);
}

// Function to simulate the Longest Remaining Time First (LRTF) CPU scheduling algorithm with preemption
void lrtf(struct process *processes, int n, char *outputFilename)
{
    FILE *fpout = fopen(outputFilename, "w");
    if (fpout == NULL)
    {
        printf("\n --> Error: Unable to open output file %s\n", outputFilename);
        exit(1);
    }

    // Print Gantt chart
    fprintf(fpout, "Gantt Chart:\n");

    printf("\n --> Running Longest Remaining Time First (LRTF) CPU scheduling algorithm with preemption...\n");
    int current_time = 0, completed = 0, prev_process_index = -1;
    while (completed < n)
    {
        int max_burst_time = INT_MIN, index = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && !processes[i].completed && processes[i].burst_time > max_burst_time)
            {
                max_burst_time = processes[i].burst_time;
                index = i;
            }
        }
        if (index == -1)
        {
            fprintf(fpout, "| Idle ");
            current_time++;
            continue;
        }
        if (prev_process_index != -1 && index != prev_process_index)
        {
            printf("Preempting P%d...\n", processes[prev_process_index].pid);
            processes[prev_process_index].preempted = 1;
        }

        fprintf(fpout, "| P%d ", processes[index].pid);
        printf("Processing P%d...\n", processes[index].pid);
        processes[index].start_time = (processes[index].start_time == -1) ? current_time : processes[index].start_time;
        processes[index].burst_time--;
        current_time++;
        if (processes[index].burst_time == 0)
        {
            processes[index].finish_time = current_time;
            processes[index].waiting_time = processes[index].start_time - processes[index].arrival_time;
            processes[index].turnaround_time = processes[index].finish_time - processes[index].arrival_time;
            processes[index].completed = 1;
            completed++;
        }
        prev_process_index = index;
    }

    fprintf(fpout, "\n\n");
    printf("\n --> All processes have finished executing.\n");

    float avg_turnaround_time, avg_waiting_time;
    compute_avg_times(processes, n, &avg_turnaround_time, &avg_waiting_time);
    printf("\n --> Average turnaround time: %.2f\n", avg_turnaround_time);
    printf(" --> Average waiting time: %.2f\n", avg_waiting_time);

    // Get the output in output file
    fprintf(fpout, "Average turn-around time: %.2f\n", avg_turnaround_time);
    fprintf(fpout, "Average waiting time: %.2f\n\n", avg_waiting_time);
    fclose(fpout);
}

int main()
{
    char InputFilename[30];
    char OutputFilename[30];
    printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
    printf("|||||               WELCOME TO THE SK CPU SCHEDULER               |||||\n");
    printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");

    int choice, n;

    int quantum = 0;
    do
    {
        printf("\n --> Select a CPU scheduling algorithm:\n");
        printf("1. First-Come First-Served (FCFS)\n");
        printf("2. Shortest Job First (SJF)\n");
        printf("3. Round Robin Scheduling (RR)\n");
        printf("4. Priority Scheduling (P)\n");
        printf("5. Shortest Remaining Time First (SRTF)\n");
        printf("6. Longest Remaining Time First (LRTF)\n");
        printf("0. Exit\n");
        printf("\n --> Enter your choice: ");
        scanf("%d", &choice);

        struct process *processes = NULL;
        if (choice)
        {
            printf("\n --> Enter the input file name (contains all information of processes): ");
            scanf("%s", InputFilename);
            processes = read_input(InputFilename, &n);

            printf("\n --> Enter the output file name where you want to get the cpu sheduling solution: ");
            scanf("%s", OutputFilename);
        }

        switch (choice)
        {
        case 0:
            printf("\n --> Exiting...\n");
            printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
            printf("|||||           THANK YOU FOR USING SK CPU SCHEDULER              |||||\n");
            printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n\n");
            break;
        case 1:
            fcfs(processes, n, OutputFilename);
            free(processes);
            break;
        case 2:
            sjf(processes, n, OutputFilename);
            free(processes);
            break;
        case 3:
            printf("\n --> Enter the quantum time for round robin sheduling: ");
            scanf("%d", &quantum);
            rr(processes, n, quantum, OutputFilename);
            free(processes);
            break;
        case 4:
            priority(processes, n, OutputFilename);
            free(processes);
            break;
        case 5:
            srtf(processes, n, OutputFilename);
            free(processes);
            break;
        case 6:
            lrtf(processes, n, OutputFilename);
            free(processes);
            break;
        default:
            printf("\n --> Invalid choice. Please try again...\n");
            break;
        }
    } while (choice != 0);

    return 0;
}
