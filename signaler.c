#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc != 3) {
    return 1;
  }

  pid_t pid = atoi(argv[1]);
  int val = atoi(argv[2]);

  union sigval value;
  value.sival_int = val;

  sigqueue(pid, SIGRTMIN, value);
  return 0;
}
