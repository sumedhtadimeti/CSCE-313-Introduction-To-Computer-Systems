#include "BuddyAllocator.h"
#include <iostream>
#include <vector>
using namespace std;
#include <math.h>  
#include <cmath>

// Linked List Constructor and function definitions

LinkedList::LinkedList(int size){
  block_size = size;
  head = NULL;
}



void LinkedList::insert(BlockHeader* b){
  // Case 1 - empty list 
  if(!head) {
        b->next = nullptr;
    }
    else{
        b->next = head;
    }
    head = b;
    b->used = true;
    ++list_length;
}


void LinkedList::remove(BlockHeader* b){
   BlockHeader* curr = head;
    
    if(curr == b){
        if(curr->next != nullptr) { //if our current blockheader we are looking at is not pointing to nullptr then we know we have a populated list.
            head = curr->next; //we are going to make head point to the next object in our list
            --list_length; //remove item
            b->used = false; //free up the block in the list space
        }
        else {
            head = nullptr;
            list_length = 0;
        }
    }
    
    else {
        for(uint i = 1; i < list_length; i++){  //if b isn't the head of our list we must go look for it using this loop
            if(curr->next == b){ //look through list until we can find b.
                if(curr->next->next != nullptr) {  
                    curr->next = curr->next->next; //remove and fix pointer math.
                    --list_length;
                    b->used = false;
                }
                else{ //else b is located somewhere else in the list or the list will be empty.
                    curr->next = nullptr;
                    --list_length;
                    b->used = false;
                }
            }
            else{
                curr = curr->next;
            }
        }
    }
  
}

// BuddyAllocator 

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
  basic_block_size = nextPowerOf2(_basic_block_size);
  total_memory_size = nextPowerOf2(_total_memory_length);
   // cout<<basic_block_size<<endl;
  
  
  
  top = new char[total_memory_size]; //allocates memory for the total size 
  BlockHeader* pointer = (BlockHeader*) top; // you have to do type castin b/c you can not allocate space for a poitner
  pointer -> used = true;
  pointer -> next = nullptr;
  pointer -> size = total_memory_size;
 //FreeList.resize((int)((log2(_total_memory_length/ _basic_block_size))+1));
  //FreeList.at(FreeList.size()-1).insert(pointer);
  populateList(FreeList);
  FreeList[getIndex(pointer->size)].insert(pointer); // adds the blocker header to the free list
  //int resize = (log2(total_memory_size)-log2(basic_block_size));
  //reeList.resize(resize);
  
  //FreeList.at(resize-1).insert(pointer);
  
  
  /*
  
  // calculate the length of a the vector
  freeListLength = int(log2((_basic_block_size*2*multiple)/(basic_block_size))) +1; // this is the number of elements the vector will have
  FreeList.resize(freeListLength);
  
  // calculate the Linked List to add to the vector
  int nextBlock = total_memory_size/_basic_block_size*multiple*2;
  FL = new LinkedList[nextBlock];
  LinkedList Sumedh;
  FreeList.push_back(Sumedh);
  
  */
}
BuddyAllocator::~BuddyAllocator(){
	delete []top;
	
}
void BuddyAllocator::populateList(vector<LinkedList>& fl)
{
    int block_size = basic_block_size;
    while(block_size <= total_memory_size)
    {
      FreeList.push_back(LinkedList(block_size));
      block_size *= 2;
    }
}



// SOURCE: https://www.geeksforgeeks.org/smallest-power-of-2-greater-than-or-equal-to-n/
int BuddyAllocator:: nextPowerOf2(int n)  
{  
    int p = 1;  
    if (n && !(n & (n - 1)))  
        return n;  
  
    while (p < n)  
        p <<= 1;  
      
    //cout<<"This is the p: "<<p<<endl;
    return p;  
}  

int BuddyAllocator::getIndex(int size){
  size = nextPowerOf2(size);
  for (int i=0;i<FreeList.size();++i){
    
    if(FreeList[i].getSize() == size)
          return i;
  }
  return -1;
}

// Have to do the get buddy function
	char* BuddyAllocator::getbuddy (char * addr){
	  
	  BlockHeader* temp = (BlockHeader*) addr;  //find the address of the buddy block
    return ((addr - top)^temp->size) + top; //return the address of the given buddy block to the user.
	  
	} 
	
	// Have to do the arebuddies function
		bool BuddyAllocator:: arebuddies (char* block1, char* block2){
		  return ((getbuddy(block2)==block1) && (getbuddy(block2)==block1));		}
		
		//Have to do the merge function
	char* BuddyAllocator::merge(char* block1, char* block2) {
    
    BlockHeader* h1 = NULL;
    BlockHeader* h2 = NULL;
    
    if(block2 > block1) { 
        h1 = (BlockHeader*) block1; //if block2 is larger then that is ok and we will assign respective values! 
        h2 = (BlockHeader*) block2;
    }
    else{
        h1 = (BlockHeader*) block2; //else we need to make the first header always larger than the second one.
        h2 = (BlockHeader*) block1;
    }
    if(!arebuddies(block1,block2)) { //if they aren't buddies then we don't want the blocks.
        return NULL;
    }
    else if(h1->size != h2->size){ //if the sizes are not equal then they cannot be buddies and we do not want that pair as well.
        return NULL;
    }
    else {
        FreeList[getIndex(h1->size)].remove(h1); //remove the two blocks so that we can properly remove them from the free list and allocate them.
        FreeList[getIndex(h1->size)].remove(h2);
    
        h1->size *= 2;
        
        char* block = (char*) h1;
        return block;
    }
}

	// Have to do the split function
  char* BuddyAllocator::split (char* block){
	  
	  BlockHeader* header =(BlockHeader*) block;
	  header->used=true;
	  int og = header ->size;
	  header->size = header->size / 2;
	  
	  //creation of the new block
	  char* block2 = block + header->size;
	  BlockHeader* NB = (BlockHeader*)(block2);
	  header->used=true;
	  NB -> size = header -> size;
	  
	  FreeList[getIndex(og)].remove(header);
	  FreeList[getIndex(NB->size)].insert(NB);
	  FreeList[getIndex(NB->size)].insert(header);
	  
	  return (char*)header;
	}
	
	
	
	
	
	
char* BuddyAllocator::alloc(int length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
   
  //int pls = length + sizeof(BlockHeader);
  int sizeOfNeededBlock = nextPowerOf2(length + sizeof(BlockHeader));
  //cout<<"SizeofNeededBlock: "<<sizeOfNeededBlock;
  // Case 1 - the needed block is smaller than the Basic Block Size
  if(sizeOfNeededBlock < basic_block_size){
    //cout<<"This is the BBS"<<basic_block_size;
    sizeOfNeededBlock = basic_block_size;
  
    
  }
  
  // Case 2 - the size that is requested is bigger than the amount available
  if (sizeOfNeededBlock > total_memory_size){
    cerr <<"The size that is requested is bigger than the amount of memory available!"<<endl;
    //cout<<" total:" <<total_memory_size;
    //cout<<" size needed: "<<sizeOfNeededBlock;
    return 0;
    
  }
  // Case 3 - the requested size is smaller than the total size availabe
  BlockHeader* temp = nullptr;
  bool showtime = false;
 // cout<<"it works here-------------"<<endl;
  
  for( int i =0; i<FreeList.size();i++){
  // cout<<" FreeList.getsize: "<<FreeList[i].getSize()<<endl;
   // cout<<"This is SN: "<<sizeOfNeededBlock;
    if(FreeList[i].getSize() >= sizeOfNeededBlock && FreeList[i].list_length !=0){
      temp = FreeList[i].getHead(); // temp is now the head of that linked list
      showtime =true;
      break;
     
    }
  }

  if(!showtime){
    
    cerr<<"No available blocks exist for this requsted size"<<endl;
    return 0;
  }
  
  //Split the blocks to reduce the overhead
  //BlockHeader* Sumedh = temp;
 // cout<<temp -> size;
  char* addr = (char*)temp;
  while (temp->size /2 >= sizeOfNeededBlock){
    addr = split(addr);
    temp = (BlockHeader*)addr;
    //temp = Sumedh;
    
    
  }
  
  //Since we found the block we want to allocate, we have to remove it from the free vector
  temp->used= false;
  FreeList[getIndex(temp->size)].remove(temp);
  
  addr = addr + sizeof(BlockHeader); // address of the new block
  
  return addr; // return address of block even if it is a void function
}

int BuddyAllocator::free(char* _a) {
  //Point to the start of the block rather than at the end.
    char* headerPtr = _a - sizeof(BlockHeader);
    BlockHeader* header = (BlockHeader*) headerPtr;
    //find a buddy for the header.
    char* buddyPtr = getbuddy(headerPtr);
    BlockHeader* buddy = (BlockHeader*) buddyPtr;
    //merge blocks until there are no more free buddies.
    while(buddy->used) {
        headerPtr = merge(headerPtr, buddyPtr);
        buddyPtr = getbuddy(headerPtr);
        header = (BlockHeader*) headerPtr;
        buddy = (BlockHeader*) buddyPtr;
    }
    
    header = (BlockHeader*) headerPtr;
    FreeList[getIndex(header->size)].insert(header);
    header->used = true;
    
    return 0;
  
}

void BuddyAllocator::printlist(){
  cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  int64_t total_free_memory = 0;
  for (int i=0; i<FreeList.size(); i++){
   // cout<<"FreeList.size(): "<<FreeList.size()<<endl;
    int blocksize = ((1<<i) * 128); // all blocks at this level are this size
    
    cout << "[" << i <<"] (" << blocksize << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList [i].getHead();
    // go through the list from head to tail and count
    while (b){
      total_free_memory += blocksize;
      count ++;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      //cout<<"WHEN THEY ARE EQUAL b is: "<<b->size<<endl;
       //cout<<"WHEN THEY ARE EQUAL blocksize: "<<blocksize<<endl;
      
      if (b->size != blocksize){
       //cout<<"b is: "<<b->size<<endl;
       //cout<<"blocksize: "<<blocksize<<endl;
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;
    cout << "Amount of available free memory: " << total_free_memory << " byes" << endl;  
  }

  
}

