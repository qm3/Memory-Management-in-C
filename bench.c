
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include "mem.h"
#include "mem_impl.h"

// use only while testing.
#undef testMode

// This is where we keep track of all the memory addresses we allocated
// so that we can free them later.
typedef struct memRecordStruct {
    void* memAddress;
    int memSize;
} memRecordType;

// how many of the records above we are going to have
#define MAX_MEM_RECORDS 10000

// memory-record table
static memRecordType * memRecord;
// how many records in the table
static int numRecords;

// total amount of memory acquired from system.
static int totalMemorySize;

static void displayStats(double cpu_time_used, int lastPercent);




/******************************************************************************************
 *
 * Procedure: main
 *                                                                                       *
 *	Command line options:
 *
 *	-n: ntrials
 *	-g: pctget
 *	-p: pctlarge
 *	-s: small_limit
 *	-l: large_limit
 *	-r: random_seed
 *
 *	ntrials: total number of getmem plus freemem calls to randomly perform during this test. Default 10000.
 *	pctget: percent of the total getmem/freemem calls that should be getmem. Default 50.
 *	pctlarge: percent of the getmem calls that should request "large" blocks with a size greater than small_limit. Default 10.
 *	small_limit: largest size in bytes of a "small" block. Default 200.
 *	large_limit: largest size in bytes of a "large" block. Default 20000.
 *	random_seed: initial seed value for the random number generator. Default: some more-or-less random number such as the the system time-of-day clock (or bytes read from /dev/urandom if you're feeling adventurous).
 *                                                                                         *
 ******************************************************************************************/

int main(int argc, char **argv) {
    int c;
    int ntrials;
    int pctget;
    int pctlarge;
    int small_limit;
    int large_limit;
    int random_seed;
    int cindx;
    int rvalue;
    int numBytes;
    int lastPercent;
    char * memAddress;
    clock_t startTime, endTime;
    double cpu_time_used;
    FILE *Fdesc;

#ifdef testMode
    int i;
    uintptr_t total_size;
    uintptr_t total_free;
    uintptr_t n_free_blocks;
#endif

    // allocate memory for our memory table
    memRecord = malloc(MAX_MEM_RECORDS * sizeof(memRecordType));
    numRecords = 0;
    totalMemorySize = 0;


    ntrials = 10000;
    pctget = 50;
    pctlarge = 10;
    small_limit = 200;
    large_limit = 20000;
    random_seed = time(NULL);
    cpu_time_used = 0;


    while ((c = getopt(argc, argv, "n:g:p:s:l:r:")) != -1) {
        switch (c) {
            case 'n':
                ntrials = atoi(optarg);
                break;
            case 'g':
                pctget = atoi(optarg);
                break;
            case 'p':
                pctlarge = atoi(optarg);
                break;
            case 's':
                small_limit = atoi(optarg);
                break;
            case 'l':
                large_limit = atoi(optarg);
                break;
            case 'r':
                random_seed = atoi(optarg);
                break;

            case '?':
                if (optopt == 'n' || optopt == 'g' || optopt == 'p'\
                    || optopt == 'l' || optopt == 's' || optopt == 'r')
                    printf("Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    printf("Unknown option `-%c'.\n", optopt);
                else
                    printf("Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    }

    srand(random_seed);

    printf("ntrials = [%d], pctget = [%d], pctlarge = [%d], "
            "small_limit = [%d], large_limit = [%d], random_seed = [%d]\n",
            ntrials, pctget, pctlarge, small_limit, large_limit, random_seed);

    if ((pctget < 0) || (pctget > 100)) {
        printf("pctget should be between 0 and 100.\n");
        exit(1);
    }

    if ((pctlarge < 0) || (pctlarge > 100)) {
        printf("pctlarge should be between 0 and 100.\n");
        exit(1);
    }

    startTime = clock();

    #ifdef testMode
    /*{
    // basic test 1.
    memRecord[0] = getmem(0x200);
    check_heap();
    freemem(memRecord[0]);
    check_heap();
    */

    // basic test 2.
    /*
    for (i = 0; i < 10; i++) {
      memRecord[i].size = ((rand() % 1000) + 2000);
      memRecord[i].memAddress = getmem(memRecord[i].size);
      printf("Client Address:[%lu], memory size: [%lu]\n", (uintptr_t) memRecord[i].memAddress, (uintptr_t) memRecord[i].size);
      check_heap();
    }
    */

    // basic test 3.
    for (i = 0; i < 10; i++) {
      memRecord[i].memSize = ((rand() % 1000) + 2000);
      memRecord[i].memAddress = getmem(memRecord[i].memSize);
      printf("Test3: Client addresses : [%lu], memory size: [%lu]\n",
  (uintptr_t) memRecord[i].memAddress, (uintptr_t) memRecord[i].memSize);
      check_heap();
    }

    for (i = 0; i < 10; i++) {
      freemem(memRecord[i].memAddress);
      printf("Test3: Client addresses : [%lu]\n", \
        (uintptr_t) memRecord[i].memAddress);
      check_heap();
    }


    get_mem_stats(&total_size, &total_free, &n_free_blocks);
    printf("get_mem_stats returns: total_size:[%lu]"\
        "total_free:[%lu] n_free_blocks:[%lu]\n", \
        total_size, total_free, n_free_blocks);

    return (0);
  }
  #endif

    lastPercent = 0;
    for (cindx = 0; cindx < ntrials; cindx++) {
        rvalue = rand();
        // generate a randome value between 0 and 100
        rvalue %= 100;
        // if this is going to be a getmem
        if (rvalue <= pctget) {
            // if there are still room in the table
            if (numRecords < MAX_MEM_RECORDS) {
                rvalue = rand();
                // generate a random value between 0 and 100
                rvalue %= 100;
                // if this is going to be a large block.
                if (rvalue <= pctlarge) {
                // generate a random number between small limit and large limit

                    rvalue = ((rvalue % (large_limit - small_limit))\
                        + small_limit);
                    // this is going to be a small block
                } else {
                    if (small_limit > 0) {
                        rvalue %= small_limit;
                    } else {
                        rvalue = MIN_MEMORY_SIZE;
                    }
                }

                memRecord[numRecords].memSize = rvalue;

                // save the memory address in the memory table
                memAddress = getmem(rvalue);

                if (memAddress == NULL) {
                    continue;
                }

                // save the memory address in the memory table
                memRecord[numRecords].memAddress = memAddress;

                numBytes = memRecord[numRecords].memSize;
                if (numBytes > 16) {
                    numBytes = 16;
                }
                memset(memAddress, 0xFE, numBytes);

                totalMemorySize += rvalue;
                // update the number of records in the table.
                numRecords++;
            }

        } else {
            if (numRecords > 0) {
            // Select a random memory address record from our table, free it.

                rvalue = rand();
                rvalue %= numRecords;
                freemem(memRecord[rvalue].memAddress);

            // Delete the freed record by moving the
            // last record over it. And shortening the list.

                memcpy(&memRecord[rvalue], &memRecord[numRecords - 1], \
                    sizeof(memRecordType));
                numRecords--;
            }
        }

        endTime = clock();
        cpu_time_used = (endTime - startTime);

        // if the percentage we are
        // at right now has passed the (last percentage + 10)
        if (((100 * cindx) / ntrials) >= (lastPercent + 10)) {
                lastPercent += 10;
                displayStats(cpu_time_used, lastPercent);
        }
    }

    if (lastPercent <= 100) {
        lastPercent += 10;
        displayStats(cpu_time_used, lastPercent);
    }


    free(memRecord);

    Fdesc = fopen("test.txt", "wb");
    if (Fdesc != NULL) {
        print_heap(Fdesc);
        fclose(Fdesc);
    }


    return (0);
}


/******************************************************************************************
 *                                                                                         *
 *   Procedure: displayStats                                                               *
 *                                                                                         *
 ******************************************************************************************/

static void displayStats(double cpu_time_used, int lastPercent) {
    int averageBlockSize;
    int cindx;
    double numSeconds;
    // the total size of all memory allocated from the system.
    uintptr_t total_size;
    // the total amount of free memory
    uintptr_t total_free;
    uintptr_t n_free_blocks;

    numSeconds = (cpu_time_used / CLOCKS_PER_SEC);

    averageBlockSize = 0;
    // The average block size is the sum of all
    // the block sizes over number of records.

    for (cindx = 0; cindx < numRecords; cindx++) {
        averageBlockSize += memRecord[cindx].memSize;
    }

    if (numRecords > 0) {
        averageBlockSize /= numRecords;
    }
    get_mem_stats(&total_size, &total_free, &n_free_blocks);

    printf("Percentage: %d%%\n", lastPercent);

    printf("Total CPU time used by the benchmark test"\
        " so far in seconds: %.4f\n", numSeconds);

    printf("Total amount of storage acquired from the"\
        " underlying system by the memory manager durin"\
        "g the test so far: %lu\n", total_size);

    printf("Total number of blocks on the free storag"
        "e list at this point in the test: %lu\n", n_free_blocks);

    printf("Average number of bytes in the free stora"
        "ge blocks at this point in the test: %.4f\n\n", \
        total_free / (double) n_free_blocks);
}
