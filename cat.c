#include "defs.h"
#define BUFSIZE 1024
int main(int argc, char *argv[])
{
  char buf[BUFSIZE];
  int n=0;
  const char *note="Write failed\n";

  while ((n = read(0, buf, sizeof(buf))) > 0)
    if (write(1, buf, n) != n) {
      (void)write(2, note, strlen(note));
      exit(1);
    }
  return(0);
}
