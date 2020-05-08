#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
#include <thread>
#include <getopt.h>
using namespace std;

FIFORequestChannel* create_new_channel(FIFORequestChannel* mainchan){
    char name [1024];
    MESSAGE_TYPE m = NEWCHANNEL_MSG;
    mainchan-> cwrite(&m, sizeof(m));
    mainchan -> cread (name, 1024);
    FIFORequestChannel* newchan = new FIFORequestChannel (name, FIFORequestChannel::CLIENT_SIDE);
    return newchan; 
}


void patient_thread_function(int n, int pno, BoundedBuffer*request_buffer){
    /* What will the patient threads do? */
    datamsg d (pno, 0.0, 1);
    double resp = 0; 
    for (int i =0;i<n;i++){
        request_buffer->push((char*)&d,sizeof(datamsg));
        d.seconds += .004;
    }
}


void file_thread_function(string fname, BoundedBuffer* request_buffer, FIFORequestChannel* chan, int mb){
    //1. create the file
    string recvfname = "recv/"+fname;
    //make it as long as the orignial length 
    char buf [1024];
    filemsg f(0,0);
    memcpy(buf, &f, sizeof(f));
    strcpy(buf+sizeof(f),fname.c_str());
    chan->cwrite(buf,sizeof(f)+fname.size()+1); //include null 
    __int64_t filelength;
    chan ->cread(&filelength,sizeof(filelength));
    FILE* fp = fopen (recvfname.c_str(),"w");
    fseek(fp, filelength, SEEK_SET);
    fclose(fp);
     //2. generate all the file messages
     filemsg* fm = (filemsg*)buf;
     __int64_t remlen = filelength;
     
     while (remlen >0){
         fm->length = min (remlen, (__int64_t) mb);
         request_buffer -> push (buf, sizeof(filemsg)+fname.size()+1);
         fm->offset += fm -> length;
         remlen -= fm -> length;
     }
}
void worker_thread_function(FIFORequestChannel* chan, BoundedBuffer* request_buffer, HistogramCollection* hc, int mb){
    char buf [1024];
    double resp =0;
    char recvbuf [mb];
    while(true){
        request_buffer ->pop(buf, 1024);
        MESSAGE_TYPE* m = (MESSAGE_TYPE *)buf;
       
        if(*m == DATA_MSG){
            chan->cwrite(buf, sizeof(datamsg));
            chan -> cread(&resp, sizeof(double));
            hc-> update (((datamsg*)buf)->person,resp);
            
        } 
        else if (*m ==QUIT_MSG){
            chan -> cwrite(m, sizeof(MESSAGE_TYPE));
            delete chan;
            break;
        }
        else if (*m== FILE_MSG){
            filemsg* fm = (filemsg*)buf;
            string fname = (char*)(fm+1);
            int sz = sizeof(filemsg)+ fname.size()+1;
            chan -> cwrite (buf,sz);
            chan -> cread(recvbuf, mb);
            string recvfname = "recv/"+ fname;
            FILE* fp = fopen(recvfname.c_str(),"r+"); // opens in read and write mode
            fseek(fp,fm->offset ,SEEK_SET);
            fwrite(recvbuf,1,fm->length,fp);
            fclose(fp);
            
        }
    }
}

int main(int argc, char *argv[])
{
    int n = 1000;    //default number of requests per "patient"
    int p = 10;     // number of patients [1,15]
    int w = 500;    //default number of worker threads
    int b = 20; 	// default capacity of the request buffer, you should change this default
	int m = MAX_MESSAGE; 	// default capacity of the message buffer
    string mstrign;
    string fname;
    string f;
    
    int code = -1; // for getopt()
    while ((code = getopt(argc, argv, "m:n:b:w:p:f:")) != -1){
    switch (code) {
        case 'm': 
            m = atoi(optarg);
            break;
        case 'n': 
            n = atoi(optarg);
            break;
        case 'p': 
            p = atoi(optarg);
            break;
        case 'b':
            b= atoi(optarg);
            break;
        case 'w':
	        w = atoi (optarg);
	        break;
	   case 'f':
	        f = optarg;
	        if(f!="test"){
	        fname = f + ".csv";
	        }
	        if(f=="test"){
	        fname = f + ".txt";
	        }
	        break;
     
  }
}
    
    int pid = fork();
    if (pid == 0){
		// modify this to pass along m
		string m2 = to_string(m);
        if (m==0){ // IF M FLAG IS TRIGGERED
            execl ("server", "server","-m",m2.c_str(), (char *)NULL);
        }
        if(m!=0){ // IF M FLAG IS NOT TRIGGERED
            execl ("server", "server", (char *)NULL);
        }
    }
    
	FIFORequestChannel* chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
    BoundedBuffer request_buffer(b);
	HistogramCollection hc;
	
	// making histograms and adding to the histogram collection hc
	
	for (int i=0;i<p;i++){
	    Histogram*h = new Histogram (10, -2.0, 2.0);
	    hc.add(h);
	    
	}
	
	// making w worker channels
    FIFORequestChannel* wchans [w];
    for (int i=0;i<w;i++){
        wchans[i]= create_new_channel(chan); // creates the channel
    }
    
	
	
    struct timeval start, end;
    gettimeofday (&start, 0);
    
    //FOR HISTOGRAM
    /* Start all threads here */
    // Create the worker threads
    thread workers [w];
    for(int i=0;i<w;i++){ 
        workers[i]= thread(worker_thread_function, wchans[i], &request_buffer, &hc,m); 
        
    }
    
    if (fname == ""){ // THIS IS FOR WHEN THE -F IS NOT CALLED 
    thread patient [p];
    for (int i=0;i<p;i++){
        patient[i]= thread(patient_thread_function,n, i+1, &request_buffer); // creates the channel
    }
    for (int i=0;i<p;i++){ 
        patient[i].join(); // creates the channel
    }
    }
 
    if (fname != ""){ // THIS IS FOR WHEN THE -F IS CALLED 
    thread filethread (file_thread_function, fname, &request_buffer, chan, m);
    filethread.join(); 
    cout <<"Patient threads/file thread finished!"<<endl;
    }
    
    
	
 // FOR HISTOGRAM
	// Join all threads here
    for (int i=0;i<w;i++){
        MESSAGE_TYPE q = QUIT_MSG;
        request_buffer.push((char*)&q,sizeof(q));// creates the channel
    }
    // join the worker threads
    for (int i=0;i<w;i++){
        workers[i].join(); // creates the channel
    }
    
    cout <<"Worker threads finished!"<<endl;
	
    gettimeofday (&end, 0);
    // print time diffrence 
    //timediff(start, end);  
    
    // print the results
	hc.print ();
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;
    
 
    
    // cleaning the main chanell
    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}
 