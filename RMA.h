//RMA.h

#include <stdio.h>
#include <stdlib.h>


#define TASK_NAME_LEN 10

/**************************************************
 
 Structure that contains relevant scheduling information to a single task.
 Variables:
	R:				task runtime
	T:				Task Period
	num_resources:	total number of resources used in the system.
 
	res_t:	(resource_time)
			an array of the amount of time using each resource.
			element 0 in the array is the amount of time using
			resource 0, etc.
			If this task does not use a resource, its corresponding
			time of use should be 0.
	max_direct: Array with similar structure to res_t. Each index
			contains the maximum amount of time a resource can be directly
			blocked for this task.
	pushthrough: Maximum amount of pushthrough blocking for a certain
			resource this task can encounter. Each array index
			corresponds to a specific resource.
	B:		Maximum total amount of blocking this task can encounter
			for a resource. Maximum of the maximum direct blocking and
			maximum pushthrough blocking.
	total_blocking: a sum of the blocking caused by each resource 
			in the system.
 
 ****************************************************/
typedef struct rma_entry_t{
	int R;
	int T;
	
	int num_resources;
	
	int* res_t;
	int* max_direct;
	int* pushthrough;
	int* B;
	
	int total_blocking;
	
	//included in future implementations
	//char* name;
	
} rma_entry;


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
rma_entry* new_entry(int R, int T, int* res_t, int num_resources);


/**************************************************
 
 Frees a task entry.
 
 ****************************************************/
void delete_entry(rma_entry* ent);


/**************************************************
 
 Sorts the table by priority: the task with the lowest
 period is assigned the highest priority
 Table must be sorted before doing RMA analysis
 
 ****************************************************/
void table_sort(rma_entry** table, int n_entries);


/**************************************************
 
 Calculates the maximum blocking time in the system for 
 each of the resources in the system. 
 Blocking must be calcuated before doing RMA analysis.
 
 ****************************************************/
void calc_blocking(rma_entry** table, int n_entries);



/**************************************************
 
 Returns 1 if the system is schedulable for ALL tasks.
 Returns 0 if the system is not schedulable.
 
 n_entries is the number of tasks in the total system
 The variable oh is the overhead in the system
 
 ****************************************************/
int is_schedulable(rma_entry** table, int n_entries, int oh);


/**************************************************
 
 Prints a table showing all tasks, thier runtimes and
 periods, and the amount of time they use each resource
 available to the system, and the amount of time each
 task might be blocked because of each resource.
 
 ****************************************************/
void print_table(rma_entry** table, int n_entries);



