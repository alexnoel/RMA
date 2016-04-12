#include "RMA.h"

typedef int (*compfn)(const void*, const void*);

//private functions

/**************************************************
 
 Comparison function for use in qsort.
 
 ****************************************************/
int comp_func(rma_entry** a, rma_entry** b){
	
	
	if((*a)->T  < (*b)->T){
		return -1;
	}else if((*a)->T > (*b)->T){
		return 1;
	}
	return 0;
}

/**************************************************
 
 Max of two integers. Returns the larger of the two integer
 inputs.
 
 ****************************************************/

int max(int a, int b){
	if (a > b) return a;
	return b;
}

/**************************************************
 
 Returns 1 if the system is schedulable with i tasks and 
 given value pair of (k,l).
 Returns 0 if the system is not schedulable under those conditions.

 Assumes the values k and l are valid for this system.
 
 The variable oh is the overhead in the system.
 
 ****************************************************/


int kl_test(rma_entry** table, int n_entries, int oh, int k, int l, int i){
	
	int val;
	int period_window;
	int l_t; //intermediate value that makes computation easier.
			 //the cieling part of the formula
	
	int j;
	
	int run_sum = 0;
	
	//main sum in RMA formula
	for (j = 0; j < i-1; j++) {
		l_t = (((l+ 1) * (table[j]->T))/table[j]->T); //the plus one acts as a ceiling
		if ((((l+ 1) * (table[j]->T)) % table[j]->T)) {   //but only if there is a remainder
			l_t ++;
		}
		//printf("l_t: %d\n",l_t);
		val = (table[j]->R + oh) * l_t;
		run_sum += val;
	}
	
	//adds the blocking from task i
	run_sum += table[i]->R + table[i]->total_blocking + oh;
	
	//calculate the time allowable for the tasks to run for one period.
	period_window = (l+ 1) * table[k]->T;
	
	//check if it is schedulable
	if (run_sum <= period_window) {
		return 1;
	}else{
		return 0;
	}
}




/**************************************************
 
 Returns 1 if the system is schedulable with i tasks.
 returns 0 if the system is not schedulable.
 
 If schedulable, this function prints which value pair of
 (k,l) succeeded.
 
 Tests all valid pairs of (k,l) for the given value i.
 
 The variable oh is the overhead in the system.
 
 ****************************************************/

int i_test(rma_entry** table, int n_entries, int oh, int i){
	
	int  l_max;
	int k,l;
	
	//for every valid value fo k
	for (k = 0; k < i + 1; k++) {
		
		//maximum valid value of l is dependent on the period of task i and k
		l_max = table[i]->T/table[k]->T; //integer division takes care of the floor
		//operation
		for (l = 0; l < l_max; l++) {
			//test schedulability for current (k,l) pair
			if(kl_test(table, n_entries, oh, k, l, i)){
				//as soon as we find a successful pair, we can move on. We know the
				//system is schedulable for this value of i
				printf("Test with %d tasks PASSED with values (k,l): (%d,%d)\n",i,k,l);
				return 1;
			}
		}
	}
	
	//if we made it this far, the system failed for every combination of (k,l).
	//System is not schedulable.
	printf("Test with %d tasks FAILED.\n",i);
	return 0;
	
}




//Implementation of public functions


/**************************************************
 
 Creates a new entry in the table.
 Variables:
	R:				task runtime
	T:				Task Period
	num_resources:	total number of resources used in the system.
 
	res_t:	an array of the amount of time using each resource.
			element 0 in the array is the amount of time using 
			resource 0, etc.
			If this task does not use a resource, its corresponding
			time of use should be 0.
 
 ****************************************************/

rma_entry* new_entry(int R, int T, int* res_t, int num_resources){
	
	int* resource_time;
	int i;
	
	rma_entry* n = (rma_entry*)malloc(sizeof(rma_entry));
	n->R = R;
	n->T = T;
	
	n->num_resources = num_resources;
	
	//copy the entries of the user space reaource time list
	//into a dynamically allocated array to avoid  stack errors
	resource_time = (int*) malloc(sizeof(int) * num_resources);
	
	for (i = 0; i < num_resources; i++) {
		resource_time[i] = res_t[i];
	}
	n->res_t = resource_time;
	
	n->max_direct = (int*)malloc(sizeof(int) * num_resources);
	n->pushthrough = (int*)malloc(sizeof(int) * num_resources);
	n->B = (int*)malloc(sizeof(int) * num_resources);
	
	return n;
}



/**************************************************
 
 Frees a task entry.
 
 ****************************************************/
void delete_entry(rma_entry* ent){
	
	free(ent->res_t);
	free(ent);
}


/**************************************************
 
 Sorts the table by priority: the task with the lowest
 period is assigned the highest priority
 
 ****************************************************/

void table_sort(rma_entry** table, int n_entries){
	
	qsort((void*)table,n_entries, sizeof(rma_entry*),(compfn)comp_func);
	
	
}




/**************************************************
 
 Calculates the max blocking and pushthrough blocking
 for each resource and each task in the table
 
 ****************************************************/
void calc_blocking(rma_entry** table, int n_entries){
	
	
	int i,j,k;
	int num_resources = table[0]->num_resources;
	
	//calculate for each resource, and each task.
	for (i = 0; i < num_resources; i++) {
		for (j = 0; j < n_entries; j++) {
			
			//calculate max direct blocking
			//only valid if the task directly uses the resource, otherwise 0.
			//if the task uses the resource, the max blocking is the maximum
			//resource usage time of any task with lower priority.
			if(j < n_entries-1){
				if (table[j]->res_t[i] == 0) {
					table[j]->max_direct[i] = 0;
				}else{
					table[j]->max_direct[i] = table[j+1]->res_t[i];
					for (k = j + 1; k < n_entries; k++) {
						if (table[j]->max_direct[i] < table[k]->res_t[i]) {
							table[j]->max_direct[i] = table[k]->res_t[i];
						}
					}
				}
			}else{
				table[j]->max_direct[i] = 0;
				
			}
			
			//calculate max pushtrough blocking
			//the tasks with highest priority and lowest priority have 0 pushthrough
			//all other tasks, the pushtrough is the maximum resource usage time
			//of any task with lower priority.
			
			if (j == 0 || j == n_entries - 1) {
				table[j]->pushthrough[i] = 0;
			}else{
				if (j < n_entries-1) {
					table[j]->pushthrough[i] = table[j+1]->res_t[i];
					for (k = j + 1; k < n_entries; k++) {
						if (table[j]->pushthrough[i] < table[k]->res_t[i]) {
							table[j]->pushthrough[i] = table[k]->res_t[i];
						}
					}
				}
				
			}
		}
	}
	
	for (i = 0; i < n_entries; i++) {
		for (j = 0; j < num_resources; j++) {
			table[i]->B[j] = max(table[i]->max_direct[j], table[i]->pushthrough[j]);
		}
	}
	
	
	//sum the blocking from each resource for each task.
	for (i = 0; i < n_entries; i++) {
		table[i]->total_blocking = 0;
		for (j = 0; j < num_resources; j++) {
			table[i]->total_blocking += table[i]->B[j];
		}
	}
	
	
}



/**************************************************
 
 Returns 1 if the system is schedulable for ALL tasks.
 Returns 0 if the system is not schedulable.
 
 n_entries is the number of tasks in the total system
 The variable oh is the overhead in the system
 
 ****************************************************/


int is_schedulable(rma_entry** table, int n_entries, int oh){
	int i;
	//tests schedulability for each value of i.
	//i represents the number of tasks to try to schedule.
	for (i = 0; i < n_entries; i++) {
		//if we fail once, we know the system is not schedulable.
		if(i_test(table, n_entries, oh, i) == 0){
			printf("EXITING.\n");
			return 0;
		}
	}
	
	//if we make it here, all tasks are schedulable
	printf("SUCCESSFUL. Tasks are schedulable.\n");
	return 1;
}


/**************************************************
 
 Prints a table showing all tasks, thier runtimes and 
 periods, and the amount of time they use each resource
 available to the system, and the amount of time each 
 task might be blocked because of each resource.
 
 ****************************************************/

void print_table(rma_entry** table, int n_entries){
	
	int i,j;
	
	//header
	printf("name  \t|runtime\t|period  \t");
	for (i = 0; i < table[0]->num_resources; i++) {
		printf("|res %d time \t|max_dir %d\t|push %d\t\t|Blocking %d\t", i,i,i,i);
	}
	printf("total blocking");
	printf("\n");
	//body
	for (i = 0; i < n_entries; i++) {
		printf("task %d\t",i);
		
		printf("|%8d\t|%8d\t",table[i]->R,table[i]->T);
		for (j = 0; j < table[i]->num_resources; j++) {
			printf("|%8d\t",table[i]->res_t[j]);
			printf("|%8d\t",table[i]->max_direct[j]);
			printf("|%8d\t",table[i]->pushthrough[j]);
			printf("|%8d\t",table[i]->B[j]);
			
		}
		printf("|%8d\t",table[i]->total_blocking);
		printf("\n");
	}
	
	
}

