// getmem returns a pointer to a new block of memory storage.
// use the public version of shared variable freelistptr.
#define  primary


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include "mem.h"
#include "mem_impl.h"

#define DEFAULT_BLOCK_SIZE 0x4000

// the maximum extra space that we will
// give away free rather than splitting the block.
#define BLOCK_THRESHOLD       0x0040



static void expandFreeList(uintptr_t size);
static char* getMemBlock(free_list_type * memoryptr, uintptr_t size);

/*******************************************************************************
*   Procedure: getmem
              *
*
              *
*   Return a pointer to a new block of storage with at least size bytes of memor
    y.        *
    **********/

void* getmem(uintptr_t size) {
    // we are looking for a best-fit block.
    // This is the best size we found so far
    int bestSize;

    // the memory address of the user block without the header.
    char * userMemory;

    // the memory address of the full user block, including the header.
    char * fullMemory;

    // the ptr we use to search
    free_list_type * searchptr;

    // the best ptr we found so far
    free_list_type * bestptr;

    // pointer to the header
    memHeaderType * headerptr;

    // Before we allocate anything, always increase by 16 for the header.
    size += HEADER_SIZE;

    // do forever
    while (1) {
        // if the free list is empty, allocate a new block from the system.

        if (freelistptr == NULL) {
            expandFreeList(size);
            continue;
        }

        // check to see if there is an appropriate size of block.

        searchptr = freelistptr;
        bestptr = NULL;
        bestSize = 0;

        while (searchptr != NULL) {
            // if we found a large-enough block
            if (searchptr->size >= size) {
                // if this is the first block
                // we found or if the block is smaller
                // than the block

                // we already foundtr->size < bestSize)){

                if ((bestSize == 0) || (searchptr->size < bestSize)) {
                    bestptr = searchptr;
                    bestSize = searchptr->size;
                }
            }
            searchptr = searchptr->nextNode;
        }

        // if no block was large enough,
        // allocate another block from the system.


        if (bestptr == NULL) {
            expandFreeList(size);
            continue;
        }

        //    At this point, we will use the block referenced by best pointer.

        //  Remove it from the freelist.

        break;
    }

    // we have found our block.


    fullMemory = getMemBlock(bestptr, size);
    userMemory = (fullMemory + HEADER_SIZE);

    headerptr = (memHeaderType *) fullMemory;
    // save in our header the size of the block
    // so that we can read it when the block if freed
    headerptr->size = size;
    return (userMemory);
}

/*******************************************************************************\
    ***********
*                                                                               \
              *
*   Procedure: expandFreeList                                                   \
              *
*                                                                               \
              *
********************************************************************************\
    **********/

static void expandFreeList(uintptr_t size) {
    int blockSize;
    free_list_type * newptr;

    blockSize = size;

    if (blockSize < DEFAULT_BLOCK_SIZE) {
        blockSize = DEFAULT_BLOCK_SIZE;
    }

    // allocate a new freelist node.

    newptr = malloc(sizeof(free_list_type));
    // allocate a new block of memory.

    newptr->address = malloc(blockSize);
    newptr->size = blockSize;
    totalSystemMemory += blockSize;

    insertMemoryBlock(newptr->address, newptr->size);
}

/*******************************************************************************\
    ***********
*                                                                               \
              *
*   Procedure: getMemBlock                                                      \
              *

    We have found the memory block in the free list that we want to use.
    We might return the entire block to the user, or we might want to split this\
     block
    and only return part of it.

*                                                                               \
              *
********************************************************************************\
    **********/

static char* getMemBlock(free_list_type * memoryptr, uintptr_t size) {
    char * fullMemory;
    free_list_type * prevptr;
    free_list_type * nextptr;

    // the address we are going to return to the user.
    fullMemory = memoryptr->address;

    // Split the cell if necessary.


    // if the memory size is too large, then we're going to split it.
    if (memoryptr->size > (size + BLOCK_THRESHOLD)) {
        // By splitting we mean deducting
        // some of the memory space but the previous link

        // but some of the previous block will remain.

        memoryptr->address += size;
        memoryptr->size    -= size;
    // we are not going to split the memory and
    // directly delete it from the list

    } else {
        // this is the previous cell.
        prevptr = memoryptr->prevNode;

        // Go back to the cell before us,
        // and change its next pointer to skip us
        // (this cell).

        // if we are the first node in the list
        if (prevptr == NULL) {
            // the freelistptr itself points to the nextNode.
            freelistptr = memoryptr->nextNode;
        } else {
            prevptr->nextNode = memoryptr->nextNode;
        }

        // this is the next cell.
        nextptr = memoryptr->nextNode;

        // Go forward the cell after us, and change its previous
        // pointer to skipus(this cell).

        // if there is another cell after us
        if (nextptr != NULL) {
            nextptr->prevNode = memoryptr->prevNode;
        }
        free(memoryptr);
    }

    return (fullMemory);
}
