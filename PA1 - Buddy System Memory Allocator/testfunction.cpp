#include <iostream>
#include "BuddyAllocator.h"
#include <iostream>
#include <vector>
using namespace std;
#include <math.h>  
using namespace std;

int nextPowerOf2(int n)  
{  
    int p = 1;  
    if (n && !(n & (n - 1)))  
        return n;  
  
    while (p < n)  
        p <<= 1;  
      
    return p;  
}  

int main(){
    int length = 1;
    cout<< nextPowerOf2(length+ sizeof(BlockHeader));
}