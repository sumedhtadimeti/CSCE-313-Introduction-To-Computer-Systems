/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"
#include <bits/stdc++.h> 
#include <sys/time.h>
#include <sys/wait.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
//#include "server.cpp"
using namespace std;


int main(int argc, char *argv[]){
    int pid = fork();
    if (pid==0){
        
        cout<<"Starting Task 4------------------------------------------"<<endl;
        cout<<"                                                         "<<endl;
        cout<<"Server has been generated \n"<<endl;
        char *args[] = {"./server",NULL};
        execvp(args[0],args);
        
        cout<<"                                                         "<<endl;
        cout<<"End of Task 4------------------------------------------"<<endl;
        cout<<"                                                         "<<endl;
        
        
    }
    
    else{
    
    int p1;
    double t;
    int e;
    bool part1 = false;
    int code;   
    int max = MAX_MESSAGE;
    // string filename = "2.csv";
    //char* FileName = "11.csv";
    bool part2 = false;
    bool run = false;
    char* FileName;
    string filename;
  while ((code = getopt(argc, argv, "p:t:e:m:f:c")) != -1)
  switch (code) {
    case 'p': //works
      p1= atoi(optarg);
      part1=true;
      break;
    case 't': //works
      t= atoi(optarg);
      break;
     case 'e': //works
      e= atoi(optarg);
      break;
      
      case 'f':
      //filename = atoi(optarg);
      FileName = optarg;
      filename = optarg;
      part2 = true;
      break;
      case 'm':
	  max = atoi (optarg);
	  break;
	  case 'c':
      run = true;
      break;
      case '?':
      break;
      default:
      abort();
  }
    
    
    
    

    
    int n = 100;    // default number of requests per "patient"
   	int p = 15;		// number of patients
    srand(time_t(NULL));

    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
/*
    // sending a non-sense message, you need to change this
    /*char buf [MAX_MESSAGE];
    char x = 55;
    chan.cwrite (&x, sizeof (x));
    int nbytes = chan.cread (buf, MAX_MESSAGE);
 */
 
    //closing the channel    
  
  
  
    // Request a data point (example from class)
    if (part1==true){
    datamsg d (p1,t,e);
    chan.cwrite (&d, sizeof(d));
    double data;
    chan.cread((char*)&data, sizeof(double));
    cout<<"The requested data point: "<<data<<endl;
    // end of requesting a single data point 

  
  // Task 1 - getting all the data points of the 1.csv
     
   //Creating timer
   //Refrence: https://www.geeksforgeeks.org/measure-execution-time-with-high-precision-in-c-c/

  cout<<"Starting Task 1------------------------------------------"<<endl;
  cout<<"                                                         "<<endl;
  
   struct timeval start, end;
   gettimeofday(&start, NULL); //starting the timer
   
   
  
  
   ofstream Datafile;
   Datafile.open("x1.csv");
   cout<<"Transfering the data points for person one..."<<endl;
   for(double i=0; i<59.996; i=i+.004){
       
       datamsg ecg1(1,i,1); // gets the ecg1 points
       datamsg ecg2(1,i,2); // gets the ecg2 points
       
       //cwrite and cread for ecg1;
       chan.cwrite(&ecg1, sizeof(datamsg));
       double dataecg1;
       chan.cread((char*)&dataecg1,sizeof(double));
     
       chan.cwrite(&ecg2, sizeof(datamsg));
       double dataecg2;
       chan.cread((char*)&dataecg2,sizeof(double));
       
      // cout<<i<<","<<dataecg1<<","<<dataecg2<<endl;
       Datafile<<i<<","<<dataecg1<<","<<dataecg2<<endl;
   }
   Datafile.close();
   cout<<"The transfer of the data points is over"<<endl;
   
   gettimeofday(&end, NULL); // ending the timer
   
   double time_taken; 
  
    time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
    time_taken = (time_taken + (end.tv_usec -  
                              start.tv_usec)) * 1e-6; 
  
  
  
   cout << "Time taken for requesting data points: " << fixed 
         << time_taken << setprecision(6); 
    cout << " sec" << endl;

   
    // End of Task 1
    cout<<"                                                         "<<endl;
    cout<<"End of Task 1------------------------------------------"<<endl;
    cout<<"                                                         "<<endl;
    }
    if (part2==true){
   struct timeval start1, end1;
   gettimeofday(&start1, NULL); //starting the timer for task 2
  
    // Task 2 - Requesting Files
    cout<<"Starting Task 2------------------------------------------"<<endl;
    cout<<"                                                         "<<endl;
   cout<<"This is fine name "<<FileName<<endl;
    // the first step is to find the lenght of the file
    //filemsg fake(0,0); // special file message
    string filename = "12.csv";
    char* buf = new char[sizeof(filemsg)+filename.size()+1];
    filemsg* fake = ((filemsg*)buf);
    fake->length = 0;
    fake->offset = 0;
    fake ->mtype = FILE_MSG;
    strcpy(buf+sizeof(filemsg), filename.c_str());
    chan.cwrite(buf, sizeof(filemsg)+filename.size()+1);
    char buf2[max];
    chan.cread(buf2, max);
    __int64_t flength;
    flength = *( __int64_t*)buf2;
    cout<<"This is the length: "<<flength<<endl;  
    
    double rounds = ceil (flength/max);
    //cout <<"This is the number of rounds needed: "<< rounds<<" to complete the request"<<endl;
    
    int Offset =0;
    int Length = max;
    FILE* copyfile;
    //char path[50] = "r1.csv";
    copyfile = fopen("/home/ubuntu/environment/PA2/received/return.csv","w");
    
    //ofstream myfile;
    //myfile.open ("1return.csv");
    
    for (double i =1; i<=rounds;i++){
    //cout<<"i: "<<i<<endl;
    filemsg real = filemsg(Offset, Length);
    
    char* bufferA = new char[sizeof(filemsg)+filename.size()+1];
    
   *(filemsg*)bufferA = real;
    strcpy(bufferA+sizeof(filemsg),FileName) ;
    chan.cwrite(bufferA,sizeof(filemsg)+filename.size()+1);
    //char* bufferB = new char[MAX_MESSAGE];
    char bufferB [max];
    chan.cread(bufferB, max);
    //cout<<bufferB<<endl;
    //myfile<<bufferB<<endl;
    fwrite(bufferB, sizeof(char), sizeof(bufferB),copyfile);
    Offset = Offset + max;
   
    if(flength - Offset < max){
     //cout<<"yes"<<endl;
     Length = flength - Offset;
    }
    // cout << Offset << " , " << Length << endl;
     
    }
   // myfile.close();
    //fclose(copyfile);
    double time_taken1; 
  
    time_taken1 = (end1.tv_sec - start1.tv_sec) * 1e6; 
    time_taken1 = (time_taken1 + (end1.tv_usec -  
                              start1.tv_usec)) * 1e-6; 
  
  
  
  // cout << "Time taken for requesting a file: " << fixed 
    //     << time_taken << setprecision(6); 
    //cout << " sec" << endl;

   cout<<"                                                         "<<endl;    
   cout<<"End of Task 2--------------------------------------------"<<endl;
   cout<<"                                                         "<<endl;
   }
   
if(run == true){  
  

   
   cout<<"Starting Task 3------------------------------------------"<<endl;
   cout<<"                                                         "<<endl;
   
    
    // Task 3 - Requesting a New Channel
    MESSAGE_TYPE t3 = NEWCHANNEL_MSG;
    char* h = new char [sizeof(t3)+filename.size()+1];
    //(MESSAGE_TYPE*)h=&t3;
    chan.cwrite (&t3, sizeof(MESSAGE_TYPE));
    char*name = new char [sizeof(t3)+filename.size()+1];
    chan.cread(name, sizeof(name));
    FIFORequestChannel chan2 (name, FIFORequestChannel::CLIENT_SIDE);
    cout<<"A New Channel Has Been Created"<<endl;
    //testing new channel
   
    datamsg d1 (3,.012,1);
    chan2.cwrite (&d1, sizeof(datamsg));
    double zero;
    chan2.cread((char*)&zero, sizeof(double));
    //cout<<"Data request 1: "<<data1<<endl;
    
    
    datamsg d2 (9,.020,2);
    chan2.cwrite (&d2, sizeof(d2));
    double data2;
    chan2.cread((char*)&data2, sizeof(double));
    cout<<"New channel data request 1 (9,.020,2): "<<data2<<endl;
    
    datamsg d3 (12,12.004,1);
    chan2.cwrite (&d3, sizeof(d3));
    double data3;
    chan2.cread((char*)&data3, sizeof(double));
    cout<<"New channel data request 2 (12,12.004,1): "<<data3<<endl;
    
    datamsg d4 (7,32.004,2);
    chan2.cwrite (&d4, sizeof(d4));
    double data4;
    chan2.cread((char*)&data4, sizeof(double));
    cout<<"New channel data request 3 (7,32.004,2): "<<data4<<endl;
    
   cout<<"                                                         "<<endl;    
   cout<<"End of Task 3------------------------------------------"<<endl;
   cout<<"                                                         "<<endl;

    //Task 5 - Closing Channels
  
    cout<<"Starting Task 5------------------------------------------"<<endl;
    cout<<"                                                         "<<endl;
    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE)); // closes the first channel
    chan2.cwrite(&m, sizeof (MESSAGE_TYPE)); // closes the second channel
    cout<<"The channels have been closed"<<endl;
    
    cout<<"                                                         "<<endl; 
    cout<<"End of Task 5------------------------------------------"<<endl;
} 

    }
    
    wait(0);
    
}
