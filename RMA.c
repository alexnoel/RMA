#include "RMA.h"

typedef int (*compfn)(const void*, const void*);

//qsort comparison function
int comp_func(rma_entry** a, rma_entry** b){
	
	
	if((*a)->T  < (*b)->T){
		return -1;
	}else if((*a)->T > (*b)->T){
		return 1;
	}
	return 0;
}


//auxiliary MAX function

int max(int a, int b){
	if (a > b) return a;
	return b;
}


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
	
	
	for (i = 0; i < num_resources; i++) {
		for (j = 0; j < n_entries; j++) {
			
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
		table[i]->total_blocking = 0;
		for (j = 0; j < num_resources; j++) {
			table[i]->total_blocking += table[i]->B[j];
		}
	}
	
	
}


//Returns 1 if the table passes and is schedulable for the given indices of ikl.
//returns 0 otherwise.
int kl_test(rma_entry** table, int n_entries, int oh, int k, int l, int i){
	
	int val;
	int period_window;
	
	int j;
	
	int run_sum = 0;
	for (j = 0; j < i-1; j++) {
		int l_t = (((l+ 1) * (table[j]->T))/table[j]->T); //the plus one acts as a ceiling
		if ((((l+ 1) * (table[j]->T)) % table[j]->T)) {
			l_t ++;
		}
		//printf("l_t: %d\n",l_t);
		val = (table[j]->R + oh) * l_t;
		run_sum += val;
	}
	
	run_sum += table[i]->R + table[i]->total_blocking + oh;
	
	period_window = (l+ 1) * table[k]->T;
	//printf("\t\t%d:\t%d  <=  %d\n",l,val,period_window);
	if (run_sum <= period_window) {
		return 1;
	}else{
		return 0;
	}
}


//Returns 1 if the table passes for a certain value.
//Returns 0 otherwise.
//prints the values of k and l for which the tbale passed and is schedulable.
//tests all valid k,l value pairs for a particular value of i.
int i_test(rma_entry** table, int n_entries, int oh, int i){
	
	int  l_max;
	int k,l;
	
	//printf("%d:\n",i);
	for (k = 0; k < i + 1; k++) {
		//printf("\t%d:\n",k);
		l_max = table[i]->T/table[k]->T; //integer division takes care of the floor
										 //operation
		for (l = 0; l < l_max; l++) {
			if(kl_test(table, n_entries, oh, k, l, i)){
				printf("Test with %d tasks PASSED with values (k,l): (%d,%d)\n",i,k,l);
				return 1;
			}
		}
	}
	printf("Test with %d tasks FAILED.\n",i);
	return 0;
	
}


int is_schedulable(rma_entry** table, int n_entries, int oh){
	int i;
	
	for (i = 0; i < n_entries; i++) {
		if(i_test(table, n_entries, oh, i) == 0){
			printf("EXITING.\n");
			return 0;
		}
	}
	printf("SUCCESSFUL. Tasks are schedulable.\n");
	return 1;
}


void print_table(rma_entry** table, int n_entries){
	
	int i,j;
	
	//header
	printf("name  \t|runtime\t|period  \t");
	for (i = 0; i < table[0]->num_resources; i++) {
		printf("|res %d \t\t|max_dir %d\t|push %d\t\t|tot_blocking %d\t", i,i,i,i);
	}
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
		printf("\n");
	}
	
	
}

