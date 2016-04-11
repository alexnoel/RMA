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


//Calculates the max blocking and pushthrough
void calc_blocking(rma_entry** table, int n_entries);


//Returns 1 if the table passes and is schedulable for the given indices of ikl.
//returns 0 otherwise.
int kl_test(rma_entry** table, int n_entries, int oh, int k, int l, int i);


//Returns 1 if the table passes for a certain value.
//Returns 0 otherwise.
//prints the values of k and l for which the tbale passed and is schedulable.
int i_test(rma_entry** table, int n_entries, int oh, int i);


int is_schedulable(rma_entry** table, int n_entries, int oh);

void print_table(rma_entry** table, int n_entries);



