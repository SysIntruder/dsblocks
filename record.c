#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

void readrecord(pid_t *pid)
{
  FILE *f;
  char buff[64], *endptr;
  long rpid;

  if (!(f = fopen(RECORDPID, "r"))) {
    perror("no record pid found");
    return;
  }
  fgets(buff, 64, f);
  fclose(f);

  errno = 0;
  rpid = strtol(buff, &endptr, 10);
  if (errno || (*endptr != '\0' && *endptr != '\n') || rpid <= 0) {
    perror("invalid pid");
    return;
  }
  *pid = rpid;
}

void killrecord(pid_t pid)
{
  if (kill(pid, SIGTERM)) {
    perror("failed to kill record.sh");
  }
  if (remove(RECORDPID)) {
    perror("failed removing "RECORDPID);
  }
}
