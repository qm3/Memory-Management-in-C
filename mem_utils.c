/******************************************************************************************
*                                                                                         *
*   mem_utils.c                                                                           *
*                                                                                         *
******************************************************************************************/
// Team: Vaughn Johnson, Winter Meng

// mem_utils.c is checks possible problems of the program.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include <assert.h>
#include "mem_impl.h"

static void mergeMemoryBlocks(free_list_type * prevptr,
    free_list_type * currentptr);

/******************************************************************************************
*                                                                                         *
*   Procedure: expandFreeList                                                              *
*                                                                                          *
*   inserting a block of memory into the freelist.                                          *
*                                                                                          *
******************************************************************************************/


void insertMemoryBlock(char* address, uintptr_t size) {
    free_list_type * newptr;
    // the ptr we use to search
    free_list_type * searchptr;
    // the ptr before the searchptr
    free_list_type * lastptr;

    // allocate a new freelist node.

    newptr = malloc(sizeof(free_list_type));
    newptr->address = address;
    newptr->size = size;

    // Find the appropriate insertion point in the freelist.

    searchptr = freelistptr;
    lastptr = freelistptr;

    while (searchptr != NULL) {
        // if this is the insertion point. (To keep the addresses sorted.)
        if (searchptr->address > newptr->address) {
            break;
        }
        lastptr = searchptr;
        // check out the next node and repeat.
        searchptr = searchptr->nextNode;
    }

    // We want to insert new pointer just before searchptr.


    // Adjust all of the forward pointers.

    // this means we will become the first node in the list.
    if (searchptr == lastptr) {
        // this happens when the list is completely empty.
        if (searchptr == NULL) {
            newptr->nextNode = NULL;
            newptr->prevNode = NULL;
            freelistptr = newptr;
        } else {
            newptr->nextNode = searchptr;
            freelistptr = newptr;
        }
    } else if (lastptr == NULL) {
        newptr->nextNode = freelistptr;
        freelistptr = newptr;
    } else {
        newptr->nextNode = searchptr;
        lastptr->nextNode = newptr;
    }

    // Adjust all of the previous(backward) pointers.

    if (searchptr == NULL) {
        newptr->nextNode = NULL;
        newptr->prevNode = lastptr;
    } else {
        // Our next new pointer is going
        // to be the previous pointer's old pointer.
        newptr->prevNode = searchptr->prevNode;
        searchptr->prevNode = newptr;
    }

    // Merge this cell with its previous cell if necessary.

    mergeMemoryBlocks(newptr->prevNode, newptr);

    // Merge this cell with its next cell if necessary.

    mergeMemoryBlocks(newptr, newptr->nextNode);
}

/******************************************************************************************
*                                                                                         *
*   Procedure: mergeMemoryBlocks                                                          *
*                                                                                         *
******************************************************************************************/

static void mergeMemoryBlocks(free_list_type * prevptr,
    free_list_type * nextptr) {
    free_list_type * lastptr;

    // Merge this cell with its previous cell.


    if ((prevptr != NULL) && (nextptr != NULL)) {
        // adjacent contiguous memory blocks
        if ((prevptr->address + prevptr->size) == nextptr->address) {
            // merge the address and size field from the previous cell.

            nextptr->address = prevptr->address;
            nextptr->size = (prevptr->size + nextptr->size);
            // Adjust the forward link to the cell we are deleting.

            lastptr = prevptr->prevNode;
            if (lastptr == NULL) {
                freelistptr = nextptr;
            } else {
                lastptr->nextNode = nextptr;
            }
            // Adjust the backward link to the cell we are deleting.

            nextptr->prevNode = lastptr;
            // Deallocate the cell.

            free(prevptr);
        }
    }
}

/******************************************************************************************
*                                                                                         *
*   Procedure: check_heap                                                                 *
*                                                                                         *
******************************************************************************************/

void check_heap() {
    char * nextMemAddress;
    free_list_type * newptr;
    free_list_type * lastptr;

    // print_heap(stdout);


    newptr = freelistptr;
    lastptr = NULL;

    while (newptr != NULL) {
        assert((newptr->size >= MIN_MEMORY_SIZE)\
            && ("Block size smaller than minimum allocated."));

        if (lastptr != NULL) {
            assert((newptr->address >= lastptr->address)\
                && ("Addresses out of order."));

            nextMemAddress = (lastptr->address + lastptr->size);

            assert((nextMemAddress != newptr->address) && ("Touching Error."));

            assert((nextMemAddress <= newptr->address) && ("Overlap Error."));
        }

        assert((newptr->prevNode == lastptr) && ("PrevPointer Error."));

        lastptr = newptr;
        newptr = newptr->nextNode;
    }
}
