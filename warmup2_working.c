#include "tokenbucket.h"
#include "my402list.h"
#include "cs402.h"



int flag1=0;

double gettime(){

struct timeval tv;
	
	gettimeofday(&tv,NULL);
	return ((tv.tv_sec*1000000) + (tv.tv_usec));
	
}

double prevPtime;
int flg=0;

void computeTime(Packet *pkt){
	
			
		
		pkt->sysArrivalTime=pstart;
		
		
		//printf("Entered!");
		//printf("arr=%lf\n",pkt->pInterArrivalTime);
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
		
		fprintf(stdout,"%012.03lfms: p%d arrives, needs %d tokens, inter-arrival time= %.03lfms\n",(ptime-emustart)/1000,pkt->packetId,pkt->tokenReq,pkt->pInterArrivalTime);
		//printf("00000503.112ms: p1 arrives, needs 3 tokens, inter-arrival time = 503.112ms");
		
		if(My402ListLength(&q1)!=0){
			My402ListAppend(&q1,pkt);
			pkt->q1Enter=gettime()-emustart;
			fprintf(stdout,"%012.03lfms: p%d enters, Q1\n",(pkt->q1Enter)/1000,pkt->packetId);
			
		}
		if(My402ListLength(&q1)==0){
		My402ListAppend(&q1,pkt);
		
		pkt->q1Enter=gettime()-emustart;
		//fprintf(stdout,"%012.03lfms: p%d enters, Q1\n",(pkt->q1Enter)/1000,pkt->packetId);
		My402ListElem *elem;
		
		if(tokenbucket>=pkt->tokenReq){
			
			elem=My402ListFirst(&q1);
			pkt=(Packet*)elem->obj;
			
			My402ListUnlink(&q1,elem);
			pkt->q1Leave=gettime()-emustart;
			
			tokenbucket=tokenbucket-pkt->tokenReq;
			fprintf(stdout,"%012.03fms: p%d leaves Q1, time in Q1 = %.03lfms, token bucket now has %d token\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->q1Leave-pkt->q1Enter)/1000,tokenbucket);
			
			My402ListAppend(&q2,pkt);
			
			if(!My402ListEmpty(&q2))
					{	pthread_mutex_lock(&mutex);				
						pthread_cond_broadcast(&cv);
						pthread_mutex_unlock(&mutex);
					}
			pkt->q2Enter=gettime();
			
			fprintf(stdout,"%012.03fms: p%d enters Q2\n",(gettime()-emustart)/1000,pkt->packetId);
		}
		/*else{
			
			My402ListAppend(&q1,pkt);
			pkt->q1Enter=gettime()-emustart;
			fprintf(stdout,"%012.03lfms: p%d enters, Q1\n",(pkt->q1Enter)/1000,pkt->packetId);
			
			}*/
				
		}
		
		
		//00000751.186ms: p1 leaves Q1, time in Q1 = 247.810ms, token bucket now has 0 token
		
		
		//pthread_mutex_unlock(&mutex);
	
}
double prevTokenTime;
int pflag=1;int k=1;
void *tokenArrival (){
	
	int i=1;
	int tokensdropped=0;
	//00000251.726ms: token t1 arrives, token bucket now has 1 token
    //00000502.031ms: token t2 arrives, token bucket now has 2 tokens
   // pthread_mutex_lock(&mutex);
	
	//for(i=1;i<=number_tokens;i=i+1){
		
	//int k=0;
	while(1){
	
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
	if(notokens==1){
		
		pthread_mutex_lock(&mutex);
		pthread_cond_broadcast(&cv); 
		pthread_mutex_unlock(&mutex);   
		pthread_exit(NULL);
		}
	
	
	if(noserver1==1&&noserver2==1){
	break;	
	}
	//tokenbucket=i;
	//pthread_mutex_lock(&mutex);
	if(tokenbucket<number_tokens){
	//j++;
	tokenbucket++;
	fprintf(stdout,"%012.03lfms: t%d arrives, token bucket now has %d token\n",(ptime-emustart)/1000,i,tokenbucket);
//	pthread_mutex_unlock(&mutex);
	}
	else{
	
	//i=1;
	tokensdropped++;
	
	fprintf(stdout,"%012.03lfms: t%d arrives, dropped\n",(ptime-emustart)/1000,i);
	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&mutex);
	
	}
	
	
	My402ListElem *elem;
	Packet *pkt;
	//if(My402ListLength(&q1)==0)
	//break;
	
	if(My402ListLength(&q1)!=0){
		
	elem=My402ListFirst(&q1);
			pkt=(Packet*)elem->obj;
		if(tokenbucket>=pkt->tokenReq){
			
			//printf("Number of Packets:%d",My402ListLength(&q1));
			
			My402ListUnlink(&q1,elem);
			pkt->q1Leave=gettime()-emustart;
			
			tokenbucket=tokenbucket-pkt->tokenReq;
			fprintf(stdout,"%012.03fms: p%d leaves Q1, time in Q1 = %.03lfms, token bucket now has %d token\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->q1Leave-pkt->q1Enter)/1000,tokenbucket);
			
			
			My402ListAppend(&q2,pkt);
			
			if(!My402ListEmpty(&q2))
					{	
						pthread_mutex_lock(&mutex);				
						pthread_cond_broadcast(&cv);
						pthread_mutex_unlock(&mutex);
					}
			
			pkt->q2Enter=gettime();
			
			fprintf(stdout,"%012.03fms: p%d enters Q2\n",(gettime()-emustart)/1000,pkt->packetId);
			
			
		}
	/*if(tokenbucket==0){	
		i=1;
		continue;
		}*/
	}
	else if(nopackets==1&&My402ListEmpty(&q1)){
		notokens = 1;
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&mutex);
		pthread_exit(NULL);
		}
/*	if(i>number_tokens){
		pthread_cond_signal(&cv);
		pthread_mutex_unlock(&mutex);
		return 0;
		}*/
	i++;
	if(My402ListLength(&q1)==0 && nopackets==1) {
		pthread_exit(NULL);
	}
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *parrival(){
	
	int i,drop=0;
	double prev=0;
	
	
	//double startTime,emuStartTime,ptime,endTime;
	
	
	/*packet arrival thread*/	
	
	/*Deterministic*/
	
	if(mode==0){
	
	//printf("Num=%d\n",number_packets);
	
	for (i=0;i<number_packets;i++) {	
		double ptime1;
		
		Packet *pkt;
		pkt= (Packet*) malloc(sizeof(Packet));
		
		pkt->packetId=i+1;
		
			
		pkt->pInterArrivalTime= packetArrivalRate/1000;
		
		pkt->tokInterArrivalTime=tokenArrivalRate/1000;
		pkt->B=number_tokens;
		pkt->tokenReq=tokensRequired;
		
		pkt->serviceTime=serviceRate/1000;
		
		if(pkt->tokenReq>number_tokens){
		usleep(pkt->pInterArrivalTime*1000);
		
		ptime1=gettime();
		if(prev==0) {
			
			pkt->pInterArrivalTime = (ptime1 - emustart)/1000;
			prev = ptime1;
			//flg++;
		}
		else {
			pkt->pInterArrivalTime = (ptime1 - prev)/1000;
			prev = ptime1;
		}
	
		
		
			
			drop++;
			if(pkt->packetId==number_packets)
				nopackets=1;
			//p1 arrives, needs 3 tokens, inter-arrival time = 503.112ms, dropped
			fprintf(stdout,"%012.03lfms: p%d arrives, needs %d tokens, inter-arrival time= %lfms, dropped\n",(gettime()-emustart)/1000,pkt->packetId,pkt->tokenReq,pkt->pInterArrivalTime);
			continue;
		}
		
		
		computeTime(pkt);
		if(pkt->packetId==number_packets)
		nopackets=1;
		
		/*pthread_mutex_lock(&mutex);
		
		
		pthread_mutex_unlock(&mutex);*/
			
		}
		
	}
	/*Tsfile reading*/
	if(mode==1){
		double ptime1;
		double prev=0;
			char newbuf[300];
			FILE *fptr;
			fptr=fopen(tfile,"r");
			if (fgets(newbuf, sizeof(newbuf), fptr)!=NULL) {
							newbuf[strlen(newbuf) - 1] = '\0';
						}
		
			int k=1;
			while (fgets(newbuf, sizeof(newbuf), fptr)!=NULL) {
			
			Packet *p=(Packet *)malloc(sizeof(Packet));
			
			sscanf(newbuf,"%lf\t%d\t%lf",&packetArrivalRate,&tokensRequired,&serviceRate);
			p->pInterArrivalTime=packetArrivalRate;
			
			p->tokenReq=tokensRequired;
			p->serviceTime=serviceRate;	
			p->packetId=k;
			k++;
		
		if(p->tokenReq>number_tokens){
			usleep(p->pInterArrivalTime*1000);
		
		
		ptime1=gettime();
		if(prev==0) {
			
			p->pInterArrivalTime = (ptime1 - emustart)/1000;
			prev = ptime1;
			//flg++;
		}
		else {
			p->pInterArrivalTime = (ptime1 - prev)/1000;
			prev = ptime1;
		}
		
		
			
			drop++;
			if(p->packetId==number_packets)
				nopackets=1;
			//p1 arrives, needs 3 tokens, inter-arrival time = 503.112ms, dropped
			fprintf(stdout,"%012.03lfms: p%d arrives, needs %d tokens, inter-arrival time= %.03lfms, dropped\n",(gettime()-emustart)/1000,p->packetId,p->tokenReq,p->pInterArrivalTime);
			continue;
		}
		
			
			computeTime(p);
			
			if(p->packetId==number_packets)
			nopackets=1;
			
			/*pthread_mutex_lock(&mutex);
			My402ListAppend(&q1,p);
			pthread_mutex_unlock(&mutex);*/
	}
			
		}
		
     /* get inter_arrival_time;
        get service_time;
        usleep(...); /* use inter_arrival_time somehow 
        customer = NewCustomer(service_time, ...);
        pthread_mutex_lock(&mutex);
        Q1->enqueue(customer);
        ... /* other stuff 
        pthread_cond_signal/broadcast(&cv);
        pthread_mutex_unlock(&mutex);*/
    
	pthread_exit(NULL);
	
}

void *server1(){
	
	/*for (;;) {
        pthread_mutex_lock(&mutex);
        while (cur_queue_size == 0 && !shutdown) {
            pthread_cond_wait(&cv, &mutex);
        }
        /* dequeue a job 
        pthread_mutex_unlock(&mutex);
        /* work on the job based on its service_time 
    }*/
    int i=0;
   while(1){
		
		while(My402ListLength(&q2)==0 && notokens==0){
			//if(nopackets==1)
			//break;
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cv,&mutex);
		pthread_mutex_unlock(&mutex);
	}
	if(My402ListEmpty(&q2)){
	noserver1=1;	
	break;
	}
/*	if(noserver2==1){
	My402ListUnlinkAll(&q1);
	My402ListUnlinkAll(&q2);
	break;	
		
	}*/
/*	if(My402ListLength(&q2)==0&&nopackets==1){
		pthread_mutex_unlock(&mutex);
		return 0;
	}*/
	
	double timeinq2;
	My402ListElem *elem;
	elem=My402ListFirst(&q2);
	Packet *pkt=(Packet*) elem->obj;
	
	if(My402ListLength(&q2)!=0){
		
		My402ListUnlink(&q2,elem);
		//00000752.932ms: p1 leaves Q2, time in Q2 = 0.216ms
		pkt->q2Leave=gettime();
		timeinq2=pkt->q2Leave-pkt->q2Enter;
		fprintf(stdout,"%012.03fms: p%d leave Q2, time in Q2 = %.03lfms\n",(gettime()-emustart)/1000,pkt->packetId,timeinq2/1000);
		pkt->s1Enter = gettime();
		//serverSleep = serviceRate-(pkt->s1Leave-pkt->s1Enter);
		usleep(serviceRate);
		pkt->s1Leave = gettime();
		pkt->sysExitTime = gettime();
		//pthread_mutex_lock(&mutex);
		fprintf(stdout,"%012.03fms: p%d begins service at S1, requesting %.03fms of service\n",(gettime()-emustart)/1000,pkt->packetId,serviceRate/1000);
		
		fprintf(stdout,"%012.03fms: p%d departs from S1, service time = %.03fms, time in system = %.03fms\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->q1Leave-pkt->q1Enter)/1000,(pkt->sysExitTime-emustart)/1000);
		//pthread_mutex_unlock(&mutex);
		}
	
	if(nopackets==1&&notokens==1&&My402ListLength(&q2)==0){
		pthread_mutex_unlock(&mutex);
		return 0;
	}
	
	//pthread_mutex_unlock(&mutex);
		}
	
	pthread_exit(NULL);	
	
	
}
void *server2(){
	
	/*for (;;) {
        pthread_mutex_lock(&mutex);
        while (cur_queue_size == 0 && !shutdown) {
            pthread_cond_wait(&cv, &mutex);
        }
        /* dequeue a job 
        pthread_mutex_unlock(&mutex);
        /* work on the job based on its service_time 
    }*/
    int i=0;
    while(1){
		
		while(My402ListLength(&q2)==0 && notokens==0){
			//if(nopackets==1)
			//break;
			pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cv,&mutex);
		pthread_mutex_unlock(&mutex);
	}
	if(My402ListEmpty(&q2)){
	noserver2=1;	
	break;
	}
	/*if(noserver2==1){
	My402ListUnlinkAll(&q1);
	My402ListUnlinkAll(&q2);
	break;	
		
	}*/
	/*if(My402ListLength(&q2)==0&&nopackets==1){
		pthread_mutex_unlock(&mutex);
		return 0;
	}*/
	double timeinq2;
	My402ListElem *elem;
	elem=My402ListFirst(&q2);
	Packet *pkt=(Packet*) elem->obj;
	
	if(My402ListLength(&q2)!=0){
		
		My402ListUnlink(&q2,elem);
		//00000752.932ms: p1 leaves Q2, time in Q2 = 0.216ms
		pkt->q2Leave=gettime();
		timeinq2=pkt->q2Leave-pkt->q2Enter;
		fprintf(stdout,"%012.03fms: p%d leave Q2, time in Q2 = %.03lfms\n",(gettime()-emustart)/1000,pkt->packetId,timeinq2/1000);
		pkt->s2Enter = gettime();
		//serverSleep = serviceRate-(pkt->s1Leave-pkt->s1Enter);
		usleep(serviceRate);
		pkt->s2Leave = gettime();
		pkt->sysExitTime = gettime();
		//p1 begins service at S1, requesting 2850ms of service
		//p1 departs from S1, service time = 2859.911ms, time in system = 3109.731ms
	//	pthread_mutex_lock(&mutex);
		fprintf(stdout,"%012.03fms: p%d begins service at S2, requesting %.03fms of service\n",(gettime()-emustart)/1000,pkt->packetId,serviceRate/1000);
		fprintf(stdout,"%012.03fms: p%d departs from S2, service time = %.03fms, time in system = %.03fms\n",(gettime()-emustart)/1000,pkt->packetId,(pkt->q2Leave-pkt->q2Enter)/1000,(pkt->sysExitTime-emustart)/1000);
		//pthread_mutex_unlock(&mutex);
		}
		

	if(nopackets==1&&notokens==1&&My402ListLength(&q2)==0){
		pthread_mutex_unlock(&mutex);
		return 0;
	}
			
	pthread_mutex_unlock(&mutex);
	
		}
	
	pthread_exit(NULL);	
	
	
}


void Usage(){

fprintf(stderr,"Usage: warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");	
exit(0);
}


int main(int argc, char* argv[]){
	
	pthread_t packet,token,server1Thread,server2Thread;
	
	
	/*Intialise Queues*/
	(void)My402ListInit(&q1);
	(void)My402ListInit(&q2);
	
	/*Initialize mutex and CV*/
	pthread_mutex_init(&mutex,NULL);	
	pthread_cond_init(&cv,NULL);	
	
	if(argc>1&&argc<13){
	int j;
	for(j=1;j<argc;j=j+2){
		
		if(!strcmp(argv[j],"-n")&&argv[j+1]!=0){
			
			if(atoi(argv[j+1])>0&&atoi(argv[j+1])<2147483647){
			number_packets=atoi(argv[j+1]);	
			
			}
		}
		 else if(!strcmp(argv[j],"-B")&&argv[j+1]!=0){
			 
			if(atoi(argv[j+1])>0&&atoi(argv[j+1])<2147483647){
			number_tokens=atoi(argv[j+1]);	
			
			
			}
		}
		 else if(!strcmp(argv[j],"-r")&&argv[j+1]!=0){
			
			double dval=(double)0;
			sscanf(argv[j+1], "%lf", &dval);
			
			//dval=atof(argv[j+1]);
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
		
		 else if(!strcmp(argv[j],"-P")&&argv[j+1]!=0){
			
			if(atoi(argv[j+1])>0&&atoi(argv[j+1])<2147483647){
			tokensRequired=atoi(argv[j+1]);	
			}
		}
		else if(!strcmp(argv[j],"-lambda")){
			
			double dval=(double)0;
			
			sscanf(argv[j+1], "%lf", &dval);
		
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
		
		else if(!strcmp(argv[j],"-mu")&&argv[j+1]!=0){
			
			double dval=(double)0;
			
			sscanf(argv[j+1], "%lf", &dval);
			
			
			if(dval>0){
				mu=dval;
				if((1/mu)>0&&(1/mu)<=10){
				serviceRate=(1/mu)*1000000;
			}
			if((1/mu)>10){
			mu=0.1	;
			serviceRate=10*1000000; //Check
			}
			
		}
	}
		else if(!strcmp(argv[j],"-t")&&argv[j+1]!=0){
		
			
			fp=fopen(argv[j+1],"r");
			buf=argv[j+1];
			strcpy(tfile,argv[j+1]);
			if(fp!=NULL){
				
			if (fgets(buf, sizeof(buf), fp)!=NULL) {
					if(buf[strlen(buf) - 1] == '\n')
							buf[strlen(buf) - 1] = '\0';	
							number_packets = (int)strtod(buf,NULL);
							
						}
		//	fclose(fp);
			mode=1;
			
			}
			else 
			{	
			fprintf(stderr,"File %s does not exist!\n", argv[j+1]);
			//printf(strerror(errno));
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
	printf("lambda= %lf\n",lambda);
	printf("mu= %lf\n",serviceRate/1000000);
	printf("r= %lf\n",r);
	printf("B= %d\n",number_tokens);
	printf("P= %d\n",tokensRequired);
	printf("tsfile= FILENAME\n");
	
	printf("00000000.000ms: emulation begins\n");
	}

	if(mode==1){
		
	/*Read from file and print*/	
	printf("Emulation Parameters:\n");
	printf("number to arrive = %d\n",number_packets);
	
	printf("r= %lf\n",r);
	printf("B= %d\n",number_tokens);
	
	printf("tsfile= %s\n",tfile);
	
	
	printf("00000000.000ms: emulation begins\n");
	
	
	}
	
	/*Create Threads*/
	emustart=gettime();
	
	
	pthread_create(&packet,0,parrival,NULL);
	pthread_create(&token,0,tokenArrival,NULL);
	pthread_create(&server1Thread,0,server1,NULL);
	pthread_create(&server2Thread,0,server2,NULL);
	
	
	/*Print Tracing*/
	/*Print Statistics*/
	pthread_join(token,NULL);
	pthread_join(packet,NULL); 
	pthread_join(server1Thread,NULL);
	pthread_join(server2Thread,NULL);
	
	
	
	return 0;
	
}
