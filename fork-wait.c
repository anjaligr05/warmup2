#include "defs.h"
#include <sys/time.h>

#define NUM_CHILD 5
int sleep_time[NUM_CHILD], chd_num=0;
struct timeval tv;
time_t curtime;
int main(int argc, char *argv[])
{
  srand48(time(0));
  char buffer[30];
  for (chd_num=0; chd_num < NUM_CHILD; chd_num++) {
    sleep_time[chd_num] = lrand48() % 5000000;
    if (fork() == 0) {
      int pid=((int)getpid());
      printf("(Child) pid = %1d (0x%08x)\n", pid, pid);
      
      gettimeofday(&tv,NULL);
      curtime=tv.tv_sec;
      strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
	printf("%s%ld\n",buffer,tv.tv_usec);
     
      usleep(sleep_time[chd_num]);
      exit(pid+1);
    }
  }
  for (;;) {
    int pid=0, rc=0;
    if ((pid=wait(&rc)) == (-1)) break;
    printf("child with pid %1d exited: 0x%08x.\n", pid, rc);
  }
  return 0;
}
