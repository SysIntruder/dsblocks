#include <stdio.h>
#include <stdlib.h>

void readbattery(int *c, int *a)
{
  FILE *f;
  char sc[4], sa[2];

  if (!(f = fopen(CAPACITY, "r"))) {
    perror("can't read capacity");
    return;
  }
  fgets(sc, sizeof(sc), f);
  fclose(f);
  *c = strtof(sc, NULL);

  if (!(f = fopen(ADAPTER, "r"))) {
    perror("can't read adapter");
    return;
  }
  fgets(sa, sizeof(sa), f);
  fclose(f);
  *a = strtof(sa, NULL);
}
