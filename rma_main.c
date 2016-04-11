/*
 
 Rate Monotonic Analysis
 Alexander Noel
 ECE 4680- lab 7
 
 This program will solve rate monotonic analysis for the problem given in lab7
 
 */

#include "RMA.h"


#include <stdlib.h>
#include <stdio.h>
/*
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
*/



int main(int argc, char** argv){
	
	int i;
	rma_entry** table;
	
	int num_tasks = 6;
	int num_resources = 2;
	int overhead = 153;
	
	int period[6] = {2560,40960,1280000,1000000,61440,1024000};
	int runtime[6] = {1300,4700,3000,23000,9000,38300};
	int res1time[6] = {200,200,200,300,150,300};
	int res2time[6] = {0,0,0,0,3000,6000};
	
	int* restime = malloc(sizeof(int) * 2);
	
	table = (rma_entry**)malloc(sizeof(rma_entry*) * num_tasks);
	
	for (i = 0; i < num_tasks; i++) {
		restime[0] = res1time[i];
		restime[1] = res2time[i];
		
		table[i] = new_entry(runtime[i], period[i], restime, num_resources);
	}
	
	
	table_sort(table, num_tasks);
	//qsort((void*)table,num_tasks, sizeof(rma_entry*),(compfn)comp_func);
	
	
	
	calc_blocking(table,num_tasks);
	print_table(table,num_tasks);
	printf("\n\n");
	is_schedulable(table, num_tasks, overhead);
	
	return 0;
}