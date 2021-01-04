# memory-pool

A simple implementation of the memory pool, which can allocate memory for arbitrary size of object.

# Run result

1. Allocate memory!
sizeof(int) : 4
addr of p_int : 0x7fd211402830
value of p_int : 1

sizeof(string) : 24
addr of p_str : 0x7fd211402838
value of p_str : ##########

sizeof(unsigned long) : 8
addr of p_ul : 0x7fd211402850
value of p_ul : 212312313

sizeof(A_big) : 80
addr of p_a_big : 0x7fd211402980
value of p_a_big : 1231.1000001.200000wooooheyheyhey


2. Check if new chunk is created!
pool.chunk_list.size() : 2


3. Check current memory layout!
=====================Chunk 1=====================
Memory addr starts from : 0x7fd211402830
Total memory size:		 49 byte
Unused memory size:		 40 byte
Available memory size:	 9 byte
@:used cell #:available cell
0 1 2 3 4 5 6
@|@ @ @|@|# #|
==================================================

=====================Chunk 2=====================
Memory addr starts from : 0x7fd211402980
Total memory size:		 225 byte
Unused memory size:		 80 byte
Available memory size:	 145 byte
@:used cell #:available cell
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
@ @ @ @ @ @ @ @ @ @|#  #  #  #  #|
==================================================



4. Deallocate previous memory.

5. Check memory layout after deallocation!
=====================Chunk 1=====================
Memory addr starts from : 0x7fd211402830
Total memory size:		 49 byte
Unused memory size:		 40 byte
Available memory size:	 9 byte
@:used cell #:available cell
0 1 2 3 4 5 6
@|@ @ @|@|# #|
==================================================

=====================Chunk 2=====================
Memory addr starts from : 0x7fd211402980
Total memory size:		 225 byte
Unused memory size:		 0 byte
Available memory size:	 225 byte
@:used cell #:available cell
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
# # # # # # # # # #|#  #  #  #  #|
==================================================


6. Clean memory fragment in memory chunks!
clean memory fragment
clean memory fragment


7. Check memory layout after memory cleaning!
=====================Chunk 1=====================
Memory addr starts from : 0x7fd211402830
Total memory size:		 49 byte
Unused memory size:		 40 byte
Available memory size:	 9 byte
@:used cell #:available cell
0 1 2 3 4 5 6
@|@ @ @|@|# #|
==================================================

=====================Chunk 2=====================
Memory addr starts from : 0x7fd211402980
Total memory size:		 225 byte
Unused memory size:		 0 byte
Available memory size:	 225 byte
@:used cell #:available cell
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
# # # # # # # # # # #  #  #  #  #|
==================================================