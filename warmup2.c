#include "tokenbucket.h"
#include "my402list.h"
#include "cs402.h"



int flag1=0;

long double gettime(){

struct timeval tv;
	
	gettimeofday(&tv,NULL);
	return ((tv.tv_sec*1000000) + (tv.tv_usec));
	
}


int flg=0;

void computeTime(Packet *pkt){
	
			
		
		pkt->sysArrivalTime=gettime();
	
		
		usleep(packetArrivalRate);
		
		ptime=gettime();	
			
		if(prevPtime==0) {
			
			pkt->pInterArrivalTime = (ptime - emustart)/1000;
	
			prevPtime = ptime;
			
		}
		else {
			pkt->pInterArrivalTime = (ptime - prevPtime)/1000;
			packetArrivalRate=(pkt->pInterArrivalTime)*1000;
			prevPtime = ptime;
		}
		
		totalInterArrivalTime+=pkt->pInterArrivalTime;
		pctr++;
		pkt->sysArrivalTime=gettime();
		fprintf(stdout,"%012.03Lfms: p%d arrives, needs %d tokens, inter-arrival time= %.03Lfms\n",(ptime-emustart)/1000,pkt->packetId,pkt->tokenReq,pkt->pInterArrivalTime);
		
		
		if(My402ListLength(&q1)!=0){
			My402ListAppend(&q1,pkt);
			pkt->q1Enter=gettime();
			
			fprintf(stdout,"%012.03Lfms: p%d enters, Q1\n",(pkt->q1Enter-emustart)/1000,pkt->packetId);
			
		}
		if(My402ListLength(&q1)==0){
		My402ListAppend(&q1,pkt);
		
		pkt->q1Enter=gettime();
		
		fprintf(stdout,"%012.03Lfms: p%d enters, Q1\n",(pkt->q1Enter-emustart)/1000,pkt->packetId);
		My402ListElem *elem;
		
		if(tokenbucket>=pkt->tokenReq){
			
			elem=My402ListFirst(&q1);
			pkt=(Packet*)elem->obj;
			
			My402ListUnlink(&q1,elem);
			pkt->q1Leave=gettime();
			
			tokenbucket=tokenbucket-pkt->tokenReq;
			
			
			fprintf(stdout,"%012.03Lfms: p%d leaves Q1, time in Q1 = %.03Lfms, token bucket now has %d token\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->q1Leave-pkt->q1Enter)/1000,tokenbucket);
			totalPTimeAtQ1+=(((pkt->q1Leave-pkt->q1Enter))/1000);
			pthread_mutex_lock(&mutex);
			My402ListAppend(&q2,pkt);
			pkt->q2Enter=gettime();
			fprintf(stdout,"%012.03Lfms: p%d enters, Q2\n",(gettime()-emustart)/1000,pkt->packetId);
			pthread_mutex_unlock(&mutex);
			if(!My402ListEmpty(&q2))
					{	pthread_mutex_lock(&mutex);				
						pthread_cond_broadcast(&cv);
						pthread_mutex_unlock(&mutex);
					}
			
			
		}

				
		}
		
		
}
long double prevTokenTime;
int pflag=1;int k=1;
void *tokenArrival (){
	
	int i=1;
	
    pthread_sigmask(SIG_BLOCK,&set,NULL);
   
	while(1){
		
		if(notokens==1){
		
		pthread_mutex_lock(&mutex);
		pthread_cond_broadcast(&cv); 
		pthread_mutex_unlock(&mutex);   
		pthread_exit(NULL);
		
		}
	
	
	totalTokens++;
	//printf("---r: %Lf\n",tokenArrivalRate);
	usleep(tokenArrivalRate);
	
	ptime=gettime();
	if(prevTokenTime==0){
	tokenArrivalRate=ptime-emustart;
	prevTokenTime=ptime;
	}
	else{
	tokenArrivalRate=ptime-prevTokenTime;
	prevTokenTime=ptime;	
	}
	if(tokenbucket<number_tokens){
	
	tokenbucket++;
	fprintf(stdout,"%012.03Lfms: token t%d arrives, token bucket now has %d token\n",(gettime()-emustart)/1000,i,tokenbucket);

	}
	else{
	
	tokensdropped++;
	
	fprintf(stdout,"%012.03Lfms: token t%d arrives, dropped\n",(ptime-emustart)/1000,i);
	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock(&mutex);
	
	}
	
	
	My402ListElem *elem;
	Packet *pkt;
	
	
	if(My402ListLength(&q1)!=0){
		
	elem=My402ListFirst(&q1);
			pkt=(Packet*)elem->obj;
		if(tokenbucket>=pkt->tokenReq){
			
			My402ListUnlink(&q1,elem);
			pkt->q1Leave=gettime();
			
			tokenbucket=tokenbucket-pkt->tokenReq;
			
			fprintf(stdout,"%012.03Lfms: p%d leaves Q1, time in Q1 = %.03Lfms, token bucket now has %d token\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->q1Leave-pkt->q1Enter)/1000,tokenbucket);
			totalPTimeAtQ1+=(((pkt->q1Leave-pkt->q1Enter))/1000);
			
			pthread_mutex_lock(&mutex);
			My402ListAppend(&q2,pkt);
			pkt->q2Enter=gettime();
			fprintf(stdout,"%012.03Lfms: p%d enters, Q2\n",(gettime()-emustart)/1000,pkt->packetId);

			pthread_mutex_unlock(&mutex);
			
			
			//totalPTimeAtQ2+=((pkt->q2Enter-emustart))/1000);
					//	usleep(1000000);
			if(pkt->packetId==number_packets){
			//	printf("entered here!\n");
			serverPFlag=1;
			notokens=1;
			}
			if(!My402ListEmpty(&q2))
					{	
						pthread_mutex_lock(&mutex);				
						pthread_cond_broadcast(&cv);
						pthread_mutex_unlock(&mutex);
					}
			
			
		}

	}
	
	else if(nopackets==1&&My402ListEmpty(&q1)){
		notokens = 1;
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&mutex);
		pthread_exit(NULL);
		}

	i++;
	if(My402ListLength(&q1)==0 && nopackets==1) {
		pthread_exit(NULL);
	}
	
	/*if(noserver1==1&&noserver2==1){
	break;	
	}*/
	
	}
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void *parrival(){
	
	int i;
	//long double prev=0;
	
    pthread_sigmask(SIG_BLOCK,&set,NULL);
	
	/*packet arrival thread*/	
	
	/*Deterministic*/
	
	if(mode==0){
	
	
	
	for (i=0;i<number_packets;i++) {	
		

		
		Packet *pkt;
		pkt= (Packet*) malloc(sizeof(Packet));
		
		
		pkt->packetId=i+1;
		
		/*Set default values for statistics*/
	pkt->sysArrivalTime=0.0; 
    pkt->q1Enter=0.0;
    pkt->q1Leave=0.0;
    pkt->q2Enter=0.0;
    pkt->q2Leave=0.0;
    pkt->s1Enter=0.0;
    pkt->s1Leave=0.0;
    pkt->s2Enter=0.0;
    pkt->s2Leave=0.0;
    pkt->sysExitTime=0.0;
	pkt->serviceTime=(1/mu)*1000;
			
		pkt->pInterArrivalTime= packetArrivalRate/1000;
		
		
		
		pkt->tokInterArrivalTime=tokenArrivalRate/1000;
		pkt->B=number_tokens;
		pkt->tokenReq=tokensRequired;
		
		pkt->serviceTime=serviceRate/1000;
		
		if(pkt->tokenReq>number_tokens){
		usleep(pkt->pInterArrivalTime*1000);
		
		ptime=gettime();
		if(prevPtime==0) {
			
			pkt->pInterArrivalTime = (ptime - emustart)/1000;
			prevPtime = ptime;
			//flg++;
		}
		else {
			pkt->pInterArrivalTime = (ptime - prevPtime)/1000;
			prevPtime = ptime;
		}
		
			drop++;
			if(pkt->packetId==number_packets)
				nopackets=1;
			totalInterArrivalTime+=pkt->pInterArrivalTime;
			pctr++;
			//p1 arrives, needs 3 tokens, inter-arrival time = 503.112ms, dropped
			fprintf(stdout,"%012.03Lfms: p%d arrives, needs %d tokens, inter-arrival time= %Lfms, dropped\n",(gettime()-emustart)/1000,pkt->packetId,pkt->tokenReq,pkt->pInterArrivalTime);
			continue;
		}
		
		
		computeTime(pkt);
		
		//totalServiceTime+=pkt->serviceTime;
		if(pkt->packetId==number_packets)
		nopackets=1;
		
		/*pthread_mutex_lock(&mutex);
		
		pthread_mutex_unlock(&mutex);*/
			
		}
		
	}
	/*Tsfile reading*/
	if(mode==1){
		
			char newbuf[300];
			FILE *fptr;
			fptr=fopen(tfile,"r");
			if (fgets(newbuf, sizeof(newbuf), fptr)!=NULL) {
							newbuf[strlen(newbuf) - 1] = '\0';
						}
		
			int k=1;
			while (fgets(newbuf, sizeof(newbuf), fptr)!=NULL) {
			
			Packet *p=(Packet *)malloc(sizeof(Packet));
			
			p->sysArrivalTime=0.0; 
			p->q1Enter=0.0;
			p->q1Leave=0.0;
			p->q2Enter=0.0;
			p->q2Leave=0.0;
			p->s1Enter=0.0;
			p->s1Leave=0.0;
			p->s2Enter=0.0;
			p->s2Leave=0.0;
			p->sysExitTime=0.0;
			p->serviceTime=(1/mu)*1000;
			
			sscanf(newbuf,"%Lf\t%d\t%Lf",&packetArrivalRate,&tokensRequired,&serviceRate);

			
			p->pInterArrivalTime=packetArrivalRate;
			
		//	totalInterArrivalTime+=p->pInterArrivalTime;
			p->tokenReq=tokensRequired;
			p->serviceTime=(serviceRate);	
			
			
			p->packetId=k;
			k++;
		
		if(p->tokenReq>number_tokens){
			usleep(p->pInterArrivalTime*1000);
		
		
		ptime=gettime();
		if(prevPtime==0) {
			
			p->pInterArrivalTime = (ptime - emustart)/1000;
			prevPtime = ptime;
			
		}
		else {
			p->pInterArrivalTime = (ptime - prevPtime)/1000;
			prevPtime = ptime;
		}
		
			totalInterArrivalTime+=p->pInterArrivalTime;
			pctr++;
			drop++;
			if(p->packetId==number_packets)
				nopackets=1;
			//p1 arrives, needs 3 tokens, inter-arrival time = 503.112ms, dropped
			//totalInterArrivalTime+=p->pInterArrivalTime;
			p->sysArrivalTime=gettime();
			fprintf(stdout,"%012.03Lfms: p%d arrives, needs %d tokens, inter-arrival time= %.03Lfms, dropped\n",(gettime()-emustart)/1000,p->packetId,p->tokenReq,p->pInterArrivalTime);
			continue;
		}
		
			
			p->sysArrivalTime=gettime();
	
		
		usleep(packetArrivalRate*1000);
		
		ptime=gettime();	
			
		if(prevPtime==0) {
			
			p->pInterArrivalTime = (ptime - emustart)/1000;
	
			prevPtime = ptime;
			
		}
		else {
			p->pInterArrivalTime = (ptime - prevPtime)/1000;
			packetArrivalRate=(p->pInterArrivalTime)*1000;
			prevPtime = ptime;
		}
		
		totalInterArrivalTime+=p->pInterArrivalTime;
		pctr++;
		p->sysArrivalTime=gettime();
		fprintf(stdout,"%012.03Lfms: p%d arrives, needs %d tokens, inter-arrival time= %.03Lfms\n",(ptime-emustart)/1000,p->packetId,p->tokenReq,p->pInterArrivalTime);
		//printf("00000503.112ms: p1 arrives, needs 3 tokens, inter-arrival time = 503.112ms");
		
		if(My402ListLength(&q1)!=0){
			My402ListAppend(&q1,p);
			p->q1Enter=gettime();
			//totalPTimeAtQ1+=(((pkt->q1Leave-pkt->q1Enter))/1000);
			fprintf(stdout,"%012.03Lfms: p%d enters, Q1\n",(p->q1Enter-emustart)/1000,p->packetId);
			
		}
		if(My402ListLength(&q1)==0){
		My402ListAppend(&q1,p);
		
		p->q1Enter=gettime();
		//totalPTimeAtQ1+=(((pkt->q1Leave-pkt->q1Enter))/1000);
		fprintf(stdout,"%012.03Lfms: p%d enters, Q1\n",(p->q1Enter-emustart)/1000,p->packetId);
		My402ListElem *elem;
		
		if(tokenbucket>=p->tokenReq){
			
			elem=My402ListFirst(&q1);
			p=(Packet*)elem->obj;
			
			My402ListUnlink(&q1,elem);
			p->q1Leave=gettime();
			
			tokenbucket=tokenbucket-p->tokenReq;
			
			
			fprintf(stdout,"%012.03Lfms: p%d leaves Q1, time in Q1 = %.03Lfms, token bucket now has %d token\n",(gettime()-emustart)/1000,p->packetId,(p->q1Leave-p->q1Enter)/1000,tokenbucket);
			totalPTimeAtQ1+=(((p->q1Leave-p->q1Enter))/1000);
			pthread_mutex_lock(&mutex);
			My402ListAppend(&q2,p);
			p->q2Enter=gettime();
			fprintf(stdout,"%012.03Lfms: p%d enters, Q2\n",(gettime()-emustart)/1000,p->packetId);
			pthread_mutex_unlock(&mutex);
			if(!My402ListEmpty(&q2))
					{	pthread_mutex_lock(&mutex);				
						pthread_cond_broadcast(&cv);
						pthread_mutex_unlock(&mutex);
					}
			
			//totalPTimeAtQ2+=(((pkt->packetId)*(pkt->q2Enter-emustart))/1000);
			
		//	usleep(1000000);
		}

				
		}

			//totalServiceTime+=p->serviceTime;
			if(p->packetId==number_packets)
			nopackets=1;
			
			/*pthread_mutex_lock(&mutex);
			My402ListAppend(&q1,p);
			pthread_mutex_unlock(&mutex);*/
	}
			
		}
	pthread_exit(NULL);
	
}

void *server1(){
	int i=0;
	
	for(i=0;i<number_packets;i++){
		
	
        while(My402ListLength(&q2)==0 && notokens==0){
		if(ctrl_c==1){
		//pthread_mutex_unlock(&mutex);
		break;	
		}
        if(serverPFlag==1)
         {
			pthread_exit(NULL); 
           
		}
        pthread_mutex_lock(&mutex);
         
        pthread_cond_wait(&cv,&mutex);
        pthread_mutex_unlock(&mutex);
	
    }
    serverp++; 
    
    if(My402ListLength(&q2)!=0){
		if(ctrl_c==1){
			break;
			}
     long double timeinq2;
    My402ListElem *elem;
    elem=My402ListFirst(&q2);
    Packet *pkt=(Packet*) elem->obj;
        
        My402ListUnlink(&q2,elem);
        //00000752.932ms: p1 leaves Q2, time in Q2 = 0.216ms
        pkt->q2Leave=gettime();
        timeinq2=pkt->q2Leave-pkt->q2Enter;
        totalPTimeAtQ2+=timeinq2/1000;
        fprintf(stdout,"%012.03Lfms: p%d leaves Q2, time in Q2 = %.03Lfms\n",(gettime()-emustart)/1000,pkt->packetId,timeinq2/1000);
        pkt->s1Enter = gettime();
        //serverSleep = serviceRate-(pkt->s1Leave-pkt->s1Enter);
        
        
        //pthread_mutex_lock(&mutex);
		
        fprintf(stdout,"%012.03Lfms: p%d begins service at S1, requesting %.03Lfms of service\n",(gettime()-emustart)/1000,pkt->packetId,pkt->serviceTime);
        
        usleep((pkt->serviceTime)*1000);
        pkt->s1Leave = gettime();
        pkt->sysExitTime = gettime();
        pTimeinS1+=(pkt->s1Leave-pkt->s1Enter)/1000;
        fprintf(stdout,"%012.03Lfms: p%d departs from S1, service time = %.03Lfms, time in system = %.03Lfms\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->s1Leave-pkt->s1Enter)/1000,(pkt->sysExitTime-pkt->sysArrivalTime)/1000);
        totalServiceTime+=(pkt->s1Leave-pkt->s1Enter)/1000;
        totalSysTime+=(pkt->sysExitTime-pkt->sysArrivalTime)/1000;
        
       // totalSysTimeSqr+=(((((pkt->sysExitTime-emustart)-(pkt->sysArrivalTime-emustart))/1000))*((((pkt->sysExitTime-emustart)-(pkt->sysArrivalTime-emustart))/1000)));
      totalSysTimeSqr1=totalSysTimeSqr1+pow((pkt->sysExitTime-emustart-(pkt->sysArrivalTime-emustart))/1000,2);
        
        //totalp=pkt->packetId;
        totalp++;
        totalEmulationTime=(pkt->sysExitTime-emustart)/1000;
        if(pkt->packetId==number_packets){
			notokens=1;
			}
        
	}
	
	 if(nopackets==1&&notokens==1&&My402ListLength(&q2)==0){
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    if(ctrl_c==1){
		break;	
		}
     
    pthread_mutex_unlock(&mutex);
        
	}
     
    pthread_exit(NULL); 
}
void *server2(){
	
	
	
	int j=0;

         for (j=0;j<number_packets;j++){
      
        while(My402ListLength(&q2)==0 && notokens==0){
           if(ctrl_c==1){
			    break;
			   }
            if(serverPFlag==1)
           {
			 pthread_exit(NULL);
		}
	
        pthread_mutex_lock(&mutex);
        
        pthread_cond_wait(&cv,&mutex);
        pthread_mutex_unlock(&mutex);
	
    }
    serverp++;
    
    if(My402ListLength(&q2)!=0){
     long double timeinq2;
    My402ListElem *elem;
    elem=My402ListFirst(&q2);
    Packet *pkt=(Packet*) elem->obj;
        if(ctrl_c==1){
			//pthread_mutex_unlock(&mutex);
			break;
			}
        My402ListUnlink(&q2,elem);
        //00000752.932ms: p1 leaves Q2, time in Q2 = 0.216ms
        pkt->q2Leave=gettime();
        timeinq2=pkt->q2Leave-pkt->q2Enter;
        fprintf(stdout,"%012.03Lfms: p%d leaves Q2, time in Q2 = %.03Lfms\n",(gettime()-emustart)/1000,pkt->packetId,timeinq2/1000);
        totalPTimeAtQ2+=(timeinq2)/1000;
        pkt->s2Enter = gettime();
        //serverSleep = serviceRate-(pkt->s1Leave-pkt->s1Enter);
        
		
        fprintf(stdout,"%012.03Lfms: p%d begins service at S2, requesting %.03Lfms of service\n",(gettime()-emustart)/1000,pkt->packetId,pkt->serviceTime);
        usleep(pkt->serviceTime*1000);
        pkt->s2Leave = gettime();
        pkt->sysExitTime = gettime();
        fprintf(stdout,"%012.03Lfms: p%d departs from S2, service time = %.03Lfms, time in system = %.03Lfms\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->s2Leave-pkt->s2Enter)/1000,(pkt->sysExitTime-pkt->sysArrivalTime)/1000);
        pTimeinS2+=(pkt->s2Leave-pkt->s2Enter)/1000;
        totalServiceTime+=(pkt->s2Leave-pkt->s2Enter)/1000;
        
		totalSysTime+=(pkt->sysExitTime-pkt->sysArrivalTime)/1000;
		
		 totalSysTimeSqr2=totalSysTimeSqr2+pow((pkt->sysExitTime-emustart-(pkt->sysArrivalTime-emustart))/1000,2);
       
        totalp++;
        totalEmulationTime=(pkt->sysExitTime-emustart)/1000;
       if(pkt->packetId==number_packets){
			notokens=1;
			}
        
	}
	
	    if(nopackets==1&&notokens==1&&My402ListLength(&q2)==0){
        pthread_mutex_unlock(&mutex);
        return 0;
    }
     if(ctrl_c==1){
		break;	
		}         
    pthread_mutex_unlock(&mutex);
     
        }
     
    pthread_exit(NULL); 
     
}


void Usage(){

fprintf(stderr,"Usage: warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");	
exit(0);
}

void generateStatistics() {

	long double variance=0.0;
		printf("\n\nStatistics: \n\n");
		//printf("inter==%lf\n",totalInterArrivalTime);
		//double num=number_packets;
		if(totalp==0){
			totalp=number_packets;
			}
			
		fprintf(stdout,"\taverage packet inter-arrival time = %.6gs\n",(double)(totalInterArrivalTime/(1000*(pctr)))); 
		
		fprintf(stdout,"\taverage packet service time = %.6gs\n\n",(double)(totalServiceTime/(1000*totalp)));
		fprintf(stdout,"\taverage number of packets in Q1 = %.6g\n",(double)((totalPTimeAtQ1)/totalEmulationTime));
		fprintf(stdout,"\taverage number of packets in Q2 = %.6g\n",(double)((totalPTimeAtQ2)/totalEmulationTime));
		fprintf(stdout,"\taverage number of packets at S1 = %.6g\n",(double)(pTimeinS1/totalEmulationTime));
		fprintf(stdout,"\taverage number of packets at S2 = %.6g\n\n",(double)(pTimeinS2/totalEmulationTime));
		
		fprintf(stdout,"\taverage time a packet spent in system = %.6gs\n",(double)(totalSysTime/(1000*totalp)));
		
		char A[150];
		char B[150];
		sprintf(A,"%LF",(((totalSysTimeSqr1+totalSysTimeSqr2))/(1000*1000*totalp)));
		sprintf(B,"%LF",((totalSysTime/(1000*totalp))*(totalSysTime/(1000*totalp))));
		variance=(((((totalSysTimeSqr1+totalSysTimeSqr2))/(1000*1000*totalp))))-((totalSysTime/(1000*totalp))*(totalSysTime/(1000*totalp)));
		
		if(variance<0)
		variance=variance*-1;
		fprintf(stdout,"\tstandard deviation for time spent in system = %.6gs\n\n",(double)(sqrt(variance)));
        fprintf(stdout,"\ttoken drop probability = %.6g\n",(double)((tokensdropped/totalTokens)));
		fprintf(stdout,"\tpacket drop probability = %.6g\n\n",(double)((drop/number_packets)));
     
}
void *handler(void *arg)
{    

sigset_t set;
	int sig;
        sigemptyset(&set);
        sigaddset(&set,SIGINT);
		
	   sigwait(&set,&sig);
		
		pthread_mutex_lock(&mutex);
		ctrl_c = 1;
		pthread_cancel(packet);
		pthread_cancel(token);
		pthread_cond_broadcast(&cv);
	   pthread_mutex_unlock(&mutex);
		printf("\n");
	   
	//	pthread_mutex_unlock(&mutex);
	
	pthread_exit(NULL);
		

}    
  
int isDirectory(char *filename){
	
	DIR* dir= opendir(filename);
	
	if(dir!=NULL){
	closedir(dir);
	return 1;
	}
	
	if(errno==ENOTDIR){
	return 0;
	}
	return -1;
}

int main(int argc, char* argv[]){
	

	
    sigemptyset(&set);
	sigaddset(&set,SIGINT);
	pthread_sigmask(SIG_BLOCK,&set,NULL);


	
	/*Intialise Queues*/
	(void)My402ListInit(&q1);
	(void)My402ListInit(&q2);
	
	/*Initialize mutex and CV*/
	pthread_mutex_init(&mutex,NULL);	
	pthread_cond_init(&cv,NULL);	
	
	if(argc>1&&argc<=13){
	int j;
	for(j=1;j<argc;j=j+2){
		
		if(!strcmp(argv[j],"-n")&&argv[j+1]!=0&&argv[j+1]!=NULL){
			if(atoi(argv[j+1])>=0&&atoi(argv[j+1])<=2147483647){
			number_packets=atoi(argv[j+1]);	
			if(number_packets==0){				
				fprintf(stdout,"Input is not in the right format\n");
				return 0;
				}
			}
		}
		 else if(!strcmp(argv[j],"-B")&&argv[j+1]!=0&&argv[j+1]!=NULL){
			 
			if(atoi(argv[j+1])>=0&&atoi(argv[j+1])<=2147483647){
			number_tokens=atoi(argv[j+1]);	
			if(number_tokens==0){				
				fprintf(stdout,"Input is not in the right format\n");
				return 0;
				}
			
			}
		}
		 else if(!strcmp(argv[j],"-r")&&argv[j+1]!=NULL){
			if(atof(argv[j+1])==0){				
				fprintf(stdout,"Input is not in the right format\n");
				return 0;
				}
			long double dval=0;
			sscanf(argv[j+1], "%Lf", &dval);
			
			
				if(dval>0){
				r=dval;
				if(1/r>0&&1/r<=10){
				tokenArrivalRate=(1/r)*1000000; //Check
				}
				if(1/r>10){
				r=0.1	;
				tokenArrivalRate=10*1000000; //Check
				}
				}
			
		}
		
		 else if(!strcmp(argv[j],"-P")&&argv[j+1]!=NULL){
			
			if(atoi(argv[j+1])>=0&&atoi(argv[j+1])<=2147483647){
			if(atoi(argv[j+1])==0){
				fprintf(stdout,"Input is not in the right format\n");
				return 0;
				}
			
			tokensRequired=atoi(argv[j+1]);	
			}
		}
		else if(!strcmp(argv[j],"-lambda")&&argv[j+1]!=NULL){
			
			if(atof(argv[j+1])==0){
				fprintf(stdout,"Input is not in the right format\n");
				return 0;
				
				}
			long double dval=0;
			
			sscanf(argv[j+1], "%Lf", &dval);
		
				if(dval>0){
				lambda=dval;
				if((1/lambda)>0&&(1/lambda)<=10){
				packetArrivalRate=(1/lambda)*1000000;
				
				//printf("ARR=%ld",packetArrivalRate);
				}
				if((1/lambda)>10){
				lambda=0.1	;
				packetArrivalRate=10*1000000; //Check*/
				}
			
			}
		}
		
		else if(!strcmp(argv[j],"-mu")&&argv[j+1]!=NULL){
				if(atof(argv[j+1])==0){
				fprintf(stdout,"Input is not in the right format\n");
				return 0;
				}
				
			
			long double dval=0;
			
			sscanf(argv[j+1], "%Lf", &dval);
			
			
			if(dval>0){
				mu=dval;
				if((1/mu)>0&&(1/mu)<=10){
				serviceRate=(1/mu)*1000000;
			}
			if((1/mu)>10){
			mu=0.1;
			serviceRate=10*1000000; //Check
			}
			
		}
	}
		else if(!strcmp(argv[j],"-t")&&argv[j+1]!=NULL){
			
		if(isDirectory(argv[j+1])==1){	
		
			fprintf(stderr, "Cannot open %s for reading. It is a directory.\n ", argv[2]);
                return 0;        
		
	}
		 if(isDirectory(argv[j+1])==-1){
		
		
		fprintf(stderr, "Cannot open %s for reading.\n ", argv[2]);
			printf("Error: %s\n",strerror(errno));
                return 0;        
		
	}
			
			fp=fopen(argv[j+1],"r");
			buf=argv[j+1];
			strcpy(tfile,argv[j+1]);
			if(fp!=NULL){
				
			if (fgets(buf, sizeof(buf), fp)!=NULL) {
					if(buf[strlen(buf) - 1] == '\n')
							buf[strlen(buf) - 1] = '\0';	
							number_packets = (int)strtod(buf,NULL);
							if(number_packets==0){
								fprintf(stdout,"Input file is not in the right format!\n");
								return 0;
								}
						}
			fclose(fp);
			mode=1;
			
			}
			
			else 
			{	
			fprintf(stderr,"File %s does not exist!\n", argv[j+1]);
			printf("%s",strerror(errno));
			exit(0);
			}
		
			
		}
		
		else {
			
			flag1++;
			Usage();
			
		}
		
		
	}
	
	
	
	
}
	
	else{
	Usage();	
	}
	
	
	if(flag1==0&&mode==0){
	printf("Emulation Parameters:\n");
	printf("number to arrive = %d\n",number_packets);
	printf("lambda= %.2Lf\n",lambda);
	printf("mu= %.2Lf\n",(mu));
	printf("r= %.2Lf\n",r);
	printf("B= %d\n",number_tokens);
	printf("P= %d\n",tokensRequired);
	printf("tsfile= FILENAME\n");
	
	printf("00000000.000ms: Emulation begins\n");
	}

	if(mode==1){
		
	/*Read from file and print*/	
	printf("Emulation Parameters:\n");
	printf("number to arrive = %d\n",number_packets);
	
	printf("r= %.2Lf\n",r);
	printf("B= %d\n",number_tokens);
	
	printf("tsfile= %s\n",tfile);
	
	
	printf("00000000.000ms: Emulation begins\n");
	
	
	}
	
	/*Create Threads*/
	emustart=gettime();
	
	
	pthread_create(&packet,0,parrival,NULL);
	pthread_create(&token,0,tokenArrival,NULL);
	pthread_create(&server1Thread,0,server1,NULL);
	pthread_create(&server2Thread,0,server2,NULL);
	pthread_create(&cntrlCHandler,0,handler,NULL);
	
	/*if(totalEmulationTime==0){
		totalEmulationTime=(gettime()-emustart)/1000;
		}*/
	/*Print Tracing*/
	pthread_join(token,NULL);
	pthread_join(packet,NULL); 
	pthread_join(server1Thread,NULL);
	pthread_join(server2Thread,NULL);
	totalEmulationTime=(gettime()-emustart)/1000;
	
	fprintf(stdout,"%012.03LFms: Emulation ends\n",totalEmulationTime);
	/*clear lists*/
	   if(!My402ListEmpty(&q1)){
		My402ListElem *elem= My402ListFirst(&q1);
		
		
		int i=0;
		len1=My402ListLength(&q1);
		
		for(i=0;i<len1;i++){
			
			Packet *pkt= (Packet*) elem->obj;
			fprintf(stdout,"\np%d removed from Q1\n",pkt->packetId);
			My402ListUnlink(&q1,elem);
			elem=My402ListFirst(&q1);
			
			
			}
		}
			if(!My402ListEmpty(&q2)){
				len2=My402ListLength(&q2);
			My402ListElem *elem1= My402ListFirst(&q2);
		int i=0;
		for(i=0;i<len2;i++){
			Packet *pkt1= (Packet*) elem1->obj;
			My402ListUnlink(&q1,elem1);
			fprintf(stdout,"\np%d removed from Q2\n",pkt1->packetId);
			elem1=My402ListFirst(&q2);
			}
		}
	
	
	/*Print Statistics*/
	generateStatistics();
	
	
	return 0;
	
}
