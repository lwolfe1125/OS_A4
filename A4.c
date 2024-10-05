#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define QUANTUM 4

typedef struct{
	char id[3];
	int arrival; 
	int burst;
	int wait;	
}Task;

int* byBurst(Task tasks[50], int count)
{
	int running = 1; 
	Task sorted[50];

	/* Copying the array & adjusting the arrival value for indexing */
	memcpy(sorted, tasks, sizeof(Task)*50);

	for(int i = 0; i < count; i++) sorted[i].arrival = i;

	/* Sorting the tasks by burst length */
	while(running)
	{
		running = 0;
		for (int i = 1; i < count; i++)
		{
			Task curr = sorted[i-1];
			Task next = sorted[i]; 

			if(curr.burst > next.burst)
			{
				sorted[i-1] = next;
				sorted[i] = curr;
				running = 1;
			}
		}
	}

	static int order[50];

	for(int i = 0; i < count; i++)
	{
		order[i] = sorted[i].arrival;
	}

	return order;
}

void FCFS(Task tasks[50], int count)
{
	/* Opening the output file */
	FILE *out = fopen("Output.txt", "w");

	fprintf(out, "FCFS:\n");

	int time = 0;

	/* For each task in queue */
	for(int i = 0; i < count; i++)
	{
		fprintf(out, "%s %d ", tasks[i].id, time); /* Adding start details to file */
		tasks[i].wait = time - tasks[i].arrival; /* Calculating wait time */

		time = time + tasks[i].burst; /* Incrementing time */
		fprintf(out, "%d\n", time); /* Printing end time to file */
	}	

	/* Printing the wait details for each task, summing wait times */
	float sum = 0;
	for(int i = 0; i < count; i++)
	{
		fprintf(out, "Waiting Time %s: %d\n", tasks[i].id, tasks[i].wait);
		sum = sum + tasks[i].wait;
	}

	fprintf(out, "Average Waiting Time: %.2f\n\n", (sum/count));
	fclose(out);
}

void RoundRobin(Task tasks[50], int count)
{
	FILE *out = fopen("Output.txt", "a"); /* Opening the output file */
	
	int running = 1;
	int time = 0;

	fprintf(out, "RR:\n"); /* Printing title */

	/* While there are tasks to run */
	while(running) 
	{
		running = 0;
		for(int i = 0; i < count; i++)
		{
			/* The task needs to run */
			if(tasks[i].burst > 0)
			{
				fprintf(out, "%s %d", tasks[i].id, time); /* Printing task ID & start time */

				tasks[i].wait = tasks[i].wait + (time - tasks[i].arrival); /* Incrementing wait time for the task */
			
				/* Progressing time */
				if(tasks[i].burst >= QUANTUM) time = time + QUANTUM;
				else time = time + tasks[i].burst;
				
				tasks[i].burst = tasks[i].burst - QUANTUM;
				tasks[i].arrival = time;

				fprintf(out, " %d\n", time); /* Printing end time */
			
				if(tasks[i].burst > 0) running = 1; /* If there needs to be another round */	
			}

		}
	}

	/* Printing & summing wait times */

	float sum = 0; 

	for(int i = 0; i < count; i++)
	{
		fprintf(out, "Waiting Time %s: %d\n", tasks[i].id, tasks[i].wait);
		sum = sum + tasks[i].wait;
	}

	/* Printing average wait time */
	fprintf(out, "Average Waiting Time: %.2f\n\n", (sum/count));

	fclose(out);
}

void NSJF(Task tasks[50], int count)
{
	FILE *out = fopen("Output.txt", "a");
		
	/* Get the order of tasks by burst length */
	int *order = byBurst(tasks, count);

	int time = 0;
	int task = 0;
	int shortest = 0;
	int recent = 0;

	fprintf(out, "NSJF:\n");

	/* While all the tasks have not yet ran */
	while(task < count)
	{
		Task SJ = tasks[order[shortest]];

		/* If a shorter job hasn't arrived */
		if(time < SJ.arrival)
		{
			/* Print details for the most recent arrival */
			fprintf(out, "%s %d", tasks[recent].id, time);

			/* Incrementing */
			tasks[recent].wait = time - tasks[recent].arrival;
			time = time + tasks[recent].burst;
			task++;
			recent++;

			fprintf(out, " %d\n", time);
		}

		else
		{
			/* If the shortest task has already been ran, skip it */
			if(recent > order[shortest]) shortest++;

			else
			{
			       	/* Print shortest job details */
				fprintf(out, "%s %d", SJ.id, time);

				tasks[order[shortest]].wait = time - SJ.arrival;

				/* increment */
				task++;
				shortest++;
				time = time + SJ.burst;

				fprintf(out, " %d\n", time);
			}
		}		
	}

	float sum = 0;

	for(int i = 0; i < count; i++) 
	{
		fprintf(out, "%s Wait Time: %d\n", tasks[i].id, tasks[i].wait);
		sum = sum + tasks[i].wait; 
	}

	fprintf(out, "Average Wait Time: %.2f\n\n", (sum/count));

	fclose(out);
}

void PSJF(Task tasks[50], int count)
{
	FILE* out = fopen("Output.txt", "a"); /* Opening the file */

	fprintf(out, "PSJF:\n"); /* Printing the header */

	int running = 1; 
	int time = 0;

	Task *current = &tasks[count];
	strcpy(current->id, "f");

	while(running)
	{
		running = 0;
		Task *ready[50];
		int queued = 0;


		//Adding tasks to the ready queue
		for(int i = 0; i < count; i++)
		{
			//Queueing tasks that have arrived & have bursts left
			if(tasks[i].arrival <= time && tasks[i].burst > 0)
			{
				ready[queued] = &tasks[i];
				queued++;
				running = 1;
			}
		}
		
		//Picking the shortest burst in the ready queue
		Task *next = ready[0];
		for(int i = 1; i < queued; i++)
		{
			if(ready[i]->burst < next->burst) next = ready[i];	
		}

		//For the first task
		if(strcmp(current->id, "f") == 0)
		{
			//Setting as current & printing details
			current = next;
			fprintf(out, "%s %d ", current->id, time);

			//Setting up the wait time
			current->wait = time - current->arrival;
		}

		//The final task
		else if(next->burst <= 0)
		{
			fprintf(out, "%d\n", time);
		}

		//For Preempting
		else if(strcmp(next->id, current->id) != 0 || current->burst <= 0)
		{
			//Switching tasks, printing details to file
			fprintf(out, "%d\n", time);
			
			current->arrival = time;

			current = next;
			fprintf(out, "%s %d ", current->id, time);
			current->wait = current->wait + time - current->arrival;
		}
		
		current->burst--;	
		time++;
	}

	float sum = 0;

	//Printing & summing the wait times 
	for(int i = 0; i < count; i++)
	{
		fprintf(out, "Waiting Time %s: %d\n", tasks[i].id, tasks[i].wait);
		sum = sum + tasks[i].wait;
	}

	fprintf(out, "Average Waiting Time: %.2f\n", (sum/count));

	fclose(out);
}

int main()
{
	/* Setup */
	FILE *in = fopen("TaskSpec.txt", "r");
	
	Task tasks[50];

	/* Error catching */
	if(in == NULL)
	{
		printf("Error: file not found");
		return 1; 
	}

	int i = 0;
	char* line = malloc(sizeof(char)*15);

	/*Reading the file & creating tasks*/
	while(fgets(line, sizeof(char)*15, in))
	{
		/*Breaking each line into tokens & associating them w/ the relevant attributes*/
		char* token = strtok(line, ",");
		strcpy(tasks[i].id, token);

		token = strtok(NULL, ",");
		tasks[i].arrival = atoi(token);

		token = strtok(NULL, ","); 
		tasks[i].burst = atoi(token);

		tasks[i].wait = 0;

		/*Index increment*/
		i++;
	}

	/*Closing the file*/
	fclose(in);

	/* Creating a copy for each function to manipulate */
	Task fcfs[50]; 
	Task rr[50];
	Task nsjf[50];
	Task psjf[50];
	memcpy(fcfs, tasks, sizeof(Task)*50);
	memcpy(rr, tasks, sizeof(Task)*50);
	memcpy(nsjf, tasks, sizeof(Task)*50);
	memcpy(psjf, tasks, sizeof(Task)*50);

	/* Calling the scheduling functions */
	FCFS(fcfs, i);
	RoundRobin(rr, i);
	NSJF(nsjf, i);
	PSJF(psjf, i);
}
