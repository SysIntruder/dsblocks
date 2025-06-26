/* See LICENSE file for copyright and license details. */

#include <stdio.h>

void readbattery(int *c, int *a)
{
  FILE *f;

  if (!(f = fopen(CAPACITY, "r"))) {
    perror("can't read capacity");
    return;
  }
  fscanf(f, "%d", c);
  fclose(f);

  if (!(f = fopen(ADAPTER, "r"))) {
    perror("can't read adapter");
    return;
  }
  fscanf(f, "%d", a);
  fclose(f);
}
