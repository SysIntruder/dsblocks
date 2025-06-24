#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc != 4) {
    return 1;
  }

  int signum = atoi(argv[1]);
  int val = atoi(argv[2]);
  pid_t pid = atoi(argv[3]);

  union sigval value;
  value.sival_int = val;

  sigqueue(pid, SIGRTMIN + signum, value);
  return 0;
}
