#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>
#include <dirent.h>
#include "my402list.h"
#include "cs402.h"




typedef struct tokenBucketPacket {
    
    int packetId;
    
    long double pInterArrivalTime;
    long double tokInterArrivalTime;
    int tokenReq;
    long double serviceTime;
    int B;
    FILE *fp;
    
    long double sysArrivalTime; 
    long double q1Enter;
    long double q1Leave;
    long double q2Enter;
    long double q2Leave;
    long double s1Enter;
    long double s1Leave;
    long double s2Enter;
    long double s2Leave;
    long double sysExitTime;
    
} Packet;




/**********Global Variables **********/
sigset_t set;

pthread_t packet,token,server1Thread,server2Thread;
pthread_t cntrlCHandler;

struct sigaction act,act1,act2; 
void *handler(),interrupt();

int serverPFlag=0;
int pid=0,Q1=0,Q2=0;
int number_packets=20,number_tokens=10;
long double r=1.5,mu=0.35,lambda=1.0;
long double tokenArrivalRate=((1.0/1.5)*1000000);
long double packetArrivalRate=1000000;
long double serviceRate=((1.0/0.35)*1000000);
int tokensRequired=3;
char tfile[200];
int flag=0;
char *buf;
int mode=0;
FILE *fp;
int ctrl_c=0;

/************Generate Statistics******************/
long double totalInterArrivalTime=0.0,totalServiceTime=0.0,totalSysTime=0.0,totalSysTimeSqr1=0.0,totalSysTimeSqr2=0.0;
 long double totalPTimeAtQ1=0.0, totalPTimeAtQ2=0.0;
 long double totalEmulationTime=0.0;
 long double pTimeinS1;
 long double pTimeinS2;
 long double totalTokens=0.0;
 long double tokensdropped=0.0;
 long double drop=0.0;
long double serverp=0.0,totalp=0.0;
int len1,len2,pctr;
long double prevPtime=0.0;

long double emustart,emuend,ptime;
struct timeval tv;
int tokenbucket=0;
int nopackets=0;
int shutdown=1;
int notokens = 0;
int noserver1=0;
int noserver2=0;

/*****List********/
My402List q1;
My402List q2;

/********Mutex**********/
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void *parrival();
void *tokenBucket ();
void *server();
