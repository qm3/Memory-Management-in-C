/******************************************************************************************
*                                                                                         *
*   mem.h                                                                                 *
*                                                                                         *
******************************************************************************************/
// Team: Vaughn Johnson, Winter Meng
// mem.h is the header file for the memory management package.

#ifndef MEM_H_
#define MEM_H_

void* getmem(uintptr_t size);

void freemem(void* p);

void get_mem_stats(uintptr_t* total_size,
    uintptr_t* total_free,
    uintptr_t* n_free_blocks);

void print_heap(FILE * f);

#endif
