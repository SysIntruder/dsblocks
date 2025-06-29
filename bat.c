/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>

void readbattery(int *c, int *a)
{
  FILE *f;
  char notifcmd[MAXLEN];

  if (!(f = fopen(CAPACITY, "r"))) {
    perror("can't read capacity");
    return;
  }
  fscanf(f, "%d", c);
  fclose(f);
  if (*c <= 20) {
    snprintf(notifcmd, MAXLEN, "notify-send -u critical 'Battery Low: %d%%'", *c);
    system(notifcmd);
  }

  if (!(f = fopen(ADAPTER, "r"))) {
    perror("can't read adapter");
    return;
  }
  fscanf(f, "%d", a);
  fclose(f);
}
