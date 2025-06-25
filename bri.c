/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void readbrightness(float *b, float *mb)
{
  FILE *f;
  char sb[4], smb[4];

  if (!(f = fopen(BRIGHTNESS, "r"))) {
    perror("can't read brightness");
    return;
  }
  fgets(sb, sizeof(sb), f);
  fclose(f);
  *b = strtof(sb, NULL);

  if (!(f = fopen(MAX_BRIGHTNESS, "r"))) {
    perror("can't read max_brightness");
    return;
  }
  fgets(smb, sizeof(smb), f);
  fclose(f);
  *mb = strtof(smb, NULL);
}

void writebrightness(float *b)
{
  FILE *fb;

  if (!(fb = fopen(BRIGHTNESS, "w"))) {
    perror("can't write brightness");
    return;
  }
  fprintf(fb, "%d", (int)roundf(*b));
  fclose(fb);
}

void changebrightness(int up, float step, float *b, float *mb)
{
  if (*mb == 0) {
    perror("max_brightness is 0, aborting");
    return;
  }
  *b = up ? *b + step * (*mb) / 100.0 : *b - step * (*mb) / 100.0;
  writebrightness(b);
}
