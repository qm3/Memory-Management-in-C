/******************************************************************************************
*                                                                                         *
*   freemem.c                                                                             *
*                                                                                         *
******************************************************************************************/
// Team: Vaughn Johnson, Winter Meng
// Freemem releases the memory from the heap.


#undef  primary        // use the extern version of shared variable.



#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include "mem.h"
#include "mem_impl.h"

/******************************************************************************************
*                                                                                         *
*   Procedure: freemem                                                                      *
*                                                                                           *
*   Return the block of storage at location p to the pool of available free storage.      *
*                                                                                         *
******************************************************************************************/

void freemem(void* p) {
    // the memory address of the user block without the header.
    char * userMemory;
    // the memory address of the full user block, including the header.
    char * fullMemory;
    // pointer to the header
    memHeaderType * headerptr;
    // This is the memory block that was given to the user.
    userMemory = p;
    // This is the full memory block including the header.
    fullMemory = (userMemory - HEADER_SIZE);
    headerptr = (memHeaderType *) fullMemory;

    insertMemoryBlock(fullMemory, headerptr->size);
}
