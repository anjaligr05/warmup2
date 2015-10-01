#include "defs.h"
#define NUM_THRS 100
int done[NUM_THRS];
pthread_t tid[NUM_THRS];
void *child(void *arg)
{
  int index=(int)(long)(arg);
  usleep(lrand48()%10000000);
  done[index] = 1;
  return 0;
}
void waitall()
{
  for (;;) {
    int i=0, num_done=0;
    for (i=0; i < NUM_THRS; i++) {
      if (!done[i]) break;
      num_done++;
    }
    if (num_done == NUM_THRS) break;
  }
}
int main(int argc, char *argv[])
{
  int i=0;
  memset(done, 0, sizeof(done));
  srand48(0);
  for (i=0; i < NUM_THRS; i++) {
    pthread_create(&tid[i], 0, child, (void*)(long)i);
  }
  waitall();
  return 0;
}
