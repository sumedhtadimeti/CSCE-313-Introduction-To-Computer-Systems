#include <iostream>
#include <vector> //Blank A
using namespace std;

class node {
 public://Blank B	
 	int val;
 	node* next;
};
 
void create_LL(vector<node*>& mylist, int node_num){
    mylist.assign(node_num, NULL);

    //create a set of nodes
    for (int i = 0; i < node_num; i++) {
    
       mylist[i]= new node(); // you have to create the new node before you can use it (Blank C)
        mylist[i]->val = i; // This is 15
        mylist[i]->next = NULL; // This is l6
    }

    //create a linked list
    for (int i = 0; i < node_num-1; i++) { //it has to be nod_num-1 becasue it points to a random mem address
        mylist[i]->next = mylist[i+1]; // This is line 21
    }
}

int sum_LL(node* ptr) {
    int ret = 0;
    while(ptr) {
        ret += ptr->val; // This is line 28
        ptr = ptr->next; // This is line 29 
    }
    return ret;
}

int main(int argc, char ** argv){
    const int NODE_NUM = 3;
    vector<node*> mylist;

    create_LL(mylist, NODE_NUM);
    int ret = sum_LL(mylist[0]); 
    cout << "The sum of nodes in LL is " << ret << endl;

    //Step4: delete nodes
    //Blank D
    for (int i=0; i<NODE_NUM;i++){ // This deletes the nodes
        delete mylist[i];
    }
}