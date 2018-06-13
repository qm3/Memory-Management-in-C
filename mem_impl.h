/******************************************************************************************
*                                                                                         *
*   mem_impl.h                                                                            *
*                                                                                         *
******************************************************************************************/

// Team: Vaughn Johnson, Winter Meng
// mem_impl.h is the header file for the freelist structure of the program.


#include <inttypes.h>

#ifndef mem_impl_h
#define mem_impl_h

// the 16-byte we are going to secretly allocate before the user block.
#define HEADER_SIZE        0x0010
// the minimum amount of memory the client will allocate
#define MIN_MEMORY_SIZE       0x0001


// free list structure

typedef struct free_list_struct {
    uintptr_t size;
    char *address;
    struct free_list_struct * prevNode;
    struct free_list_struct * nextNode;
}free_list_type;

// 16-bytes header specification: help for typecast.

typedef struct memHeaderStruct {
    uintptr_t size;
}memHeaderType;

#ifdef primary
       free_list_type * freelistptr = NULL;
       uintptr_t totalSystemMemory  = 0;
#else
extern free_list_type * freelistptr;
extern uintptr_t totalSystemMemory;
#endif

void insertMemoryBlock(char* address, uintptr_t size);
void check_heap();
void print_heap(FILE * f);


#endif
