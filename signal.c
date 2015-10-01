#include "defs.h"
#include <pthread.h>
#include <sys/time.h>
void handler(int signo){
	printf("Caught a signal!!%d\n",signo);
	
}

int main(int argc, char* argv[]){

sigset_t set;
sigemptyset(&set);
sigaddset(&set,SIGINT);
sigprocmask(SIG_BLOCK,&set,0);
sigset(SIGINT,handler);

for(;;){
	
	
	}

return 0;
}
