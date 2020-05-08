#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	// think about what else should be included as member variables
	int size;  // size of the block
	BlockHeader* next; // pointer to the next block
    bool used; // Checks to see if the Memory Block is free or not
	
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeaders. 
private:
	BlockHeader*head = NULL;	// you need a head of the list
	int block_size;
public:

//LinkedList(){
//head = NULL;
//block_size = 128;
//}
	int list_length = 0;
	void insert (BlockHeader* b);
	void remove (BlockHeader* b);
	int getSize() { return block_size; }
	BlockHeader* getHead() { return head; }
	LinkedList(int size);
};

class BuddyAllocator{
private:
	/* declare more member variables as necessary */
	vector<LinkedList> FreeList;
	int freeListLength; // this is the size of the FL
	int basic_block_size;
	int total_memory_size;
	LinkedList* FL;
	char* top;

private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	
	char* getbuddy (char* addr); 
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (char* block1, char* block2);
	// checks whether the two blocks are buddies are not
	// note that two adjacent blocks are not buddies when they are different sizes

	char* merge (char* block1, char* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	char* split (char* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected




public:
	int getIndex(int size); // gets the index of the vector that has LL of size size
	int nextPowerOf2(int n); // return the closest higher power of 2 number
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. 
	*/ 

	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	char* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* a); 
	/* Frees the sectisson of physical memory previously allocated 
	   using alloc(). */ 
   
	void printlist ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function prints how many free blocks of each size belong to the allocator
	at that point. It also prints the total amount of free memory available just by summing
	up all these blocks.
	Aassuming basic block size = 128 bytes):

	[0] (128): 5
	[1] (256): 0
	[2] (512): 3
	[3] (1024): 0
	....
	....
	 which means that at this point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
	 void populateList(vector<LinkedList>& fl);
};

#endif 
