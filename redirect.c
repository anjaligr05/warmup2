#include "defs.h"
int main(int argc, char *argv[])
{
  pid_t pid=(pid_t)0;
  if ((pid=fork()) == 0) {
    /* set up file descriptor 1 in the child process */
    close(1);
    if (open("/tmp/Output", O_CREAT|O_WRONLY, 0666) == -1) {
      perror("/tmp/Output");
      exit(1);
    }
    /* stdout of /bin/date has been redirected to /tmp/Output  */
    execl("/bin/date", "date", (char*)0);
    exit(1);
  }
  while(pid != wait(0)) ;
  return 0;
}
