1. Name: Winter Meng, Vaughn Johnson
2. Winter Meng: getmem.c, print_heap.c
   Vaughn Johnson: freemem.c, check_heap.c
   We developed bench.c, mem_utils.c and the header files together.
3. Our freelist is arranged as double-linked lists(with backward and forward links between memory records). We maintain this list sorted by the memory block addresses. We did both of these things so that when memory is freed, we could quickly merge adjacent records both the previous and the next as quickly as possible. It also allows check_heap() to quickly determine if blocks are out-of-order or 'touching'. We maintain a single pointer to the beginning of the freelist. We also store the memory block address and memory block size in each record. When memory is allocated, we either remove or split the blocks from the list and potentially allocate additional system memory and add a new block to the freelist. When memory is freed, we either add or merge records in the freelist.
4. The doubly linked list is a definite performance improvement. We further optimized getmem() to use a best-fit algorithm. We choose the smallest block that still meets the size requested by the client. 
   We may have satisfied the requirements of the second
extra-credit assignment proposal by using a double-linked list as it allows us to merge cells without any additional searching.
5. We tested 1000, 10000, 100000, 1000000, 10000000 trials of run. At first the program execute within 1 second, but as the trial number goes up, the CPU time that allocates the memory blocks increases, at roungly about 10000 per 0.1 second. During the program, there is no significant lag or hiccups that terminates the program, and even up to 1000000 trials we are still able to control the running time within 7 seconds. Therefore
we conclude that there is pretty convincing efficiency and speed in our program.
   We also made several tests on -g(pctget) using different numbers. As we tweaked from the Default 50 to less numbers(20, 30, 40...), the total memory we acquired from the system gradually decrease, which matches our expectation since the decrease on that parameter indicates lesser chance of allocating memory. 
6. Resources we consulted: class notes, implementations on Kerningham "C" textbook on malloc and free.# Memory-Management-in-C
