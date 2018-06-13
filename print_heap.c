/******************************************************************************************
*                                                                                         *
*   print_heap.c                                                                          *
*                                                                                         *
******************************************************************************************/
// CSE 374 Homework 6
// Team: Vaughn Johnson, Winter Meng
// print_heap visually displays the heap.
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
*   Procedure: print_heap                                                                  *
*                                                                                          *
*   Print a formatted listing on file f showing the blocks on the free list               *                                          *
*                                                                                         *
******************************************************************************************/


void print_heap(FILE * f) {
    free_list_type * newptr;
    free_list_type * lastptr;
    newptr = freelistptr;
    lastptr = NULL;

    while (newptr != NULL) {
        fprintf(f, "MemoryAddress = [%lu], ", (uintptr_t) newptr->address);
        fprintf(f, "MemorySize    = [%lu], ", (uintptr_t) newptr->size);
        fprintf(f, "CellAddress   = [%lu], ", (uintptr_t) newptr);
        fprintf(f, "NextPointer   = [%lu], ", (uintptr_t) newptr->nextNode);
        fprintf(f, "PrevPointer   = [%lu], ", (uintptr_t) newptr->prevNode);

        if (newptr->prevNode == lastptr) {
            fprintf(f, "PrevPointer Correct.\n");
        } else {
            fprintf(f, "PrevPointer Error.\n");
        }

        lastptr = newptr;
        newptr = newptr->nextNode;
    }
}
