/******************************************************************************************
*                                                                                         *
*   get_mem_stats.c                                                                       *
*                                                                                         *
******************************************************************************************/
// CSE 374 Homework 6
// Team: Vaughn Johnson, Winter Meng
// get_mem_stats stores 3 stats of the memory management program
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include "mem.h"
#include "mem_impl.h"

/******************************************************************************************
*                                                                                         *
*   Procedure: get_mem_stats                                                              *
*                                                                                         *
*    Store statistics about the current state of the memory manager in the three integer   *
*    variables whose addresses are given as arguments                                      *
*                                                                                          *
******************************************************************************************/
// the total size of all memory allocated from the system.
// the total amount of free memory
// the number of blocks in the free list

void get_mem_stats(uintptr_t* total_size,
    uintptr_t* total_free,
    uintptr_t* n_free_blocks) {

    uintptr_t total_free_memory;
    uintptr_t total_free_records;
    free_list_type * searchptr;



    total_free_memory = 0;
    total_free_records = 0;

    searchptr = freelistptr;

    while (searchptr != NULL) {
        total_free_memory += searchptr->size;
        total_free_records++;
        searchptr = searchptr->nextNode;
    }

    (*total_size)    = totalSystemMemory;
    (*total_free)    = total_free_memory;
    (*n_free_blocks) = total_free_records;
}
