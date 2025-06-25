/* See LICENSE file for copyright and license details. */

#include <time.h>

void readdate(char *d, size_t ds)
{
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(d, ds, "%a %d %b %H:%M", timeinfo);
}
