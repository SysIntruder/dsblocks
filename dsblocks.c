#include <X11/Xlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LENGTH(X) (sizeof(X) / sizeof(X)[0])

enum { CmdDate,
       CmdBat,
       CmdBri,
       CmdMic,
       CmdSpk };

typedef struct {
  int cmd;
  int interval;
  int signal;
} Block;

#include "config.h"

#include "alsa.c"
#include "bat.c"
#include "bri.c"
#include "date.c"

static int batcap, batadp;
static float rbri, maxbri;
static long rspk, minspk, maxspk;
static long rmic, minmic, maxmic;
static int spkon, micon;
static float briperc, spkperc, micperc;
static char sblocks[LENGTH(blocks)][BLOCKLEN];
static int counter, debug;

void updatebriperc()
{
  if (maxbri > rbri)
    briperc = rbri / maxbri * 100.0;
}

void updatespkperc()
{
  spkperc = 100.0 * (rspk - minspk) / (maxspk - minspk);
}

void updatemicperc()
{
  micperc = 100.0 * (rmic - minmic) / (maxmic - minmic);
}

void cmddate(int i)
{
  readdate(sblocks[i], BLOCKLEN);
}

void cmdbat(int i)
{
  readbattery(&batcap, &batadp);
  if (batadp)
    sprintf(sblocks[i], "%cP: %d%%+%c", blocks[i].signal, batcap, blocks[i].signal);
  else
    sprintf(sblocks[i], "%cP: %d%%%c", blocks[i].signal, batcap, blocks[i].signal);
}

void cmdbri(int i)
{
  readbrightness(&rbri, &maxbri);
  updatebriperc();
  sprintf(sblocks[i], "%cB: %.f%%%c", blocks[i].signal, briperc, blocks[i].signal);
}

void cmdmic(int i)
{
  readvolume(1, &minmic, &maxmic, &rmic, &micon);
  updatemicperc();
  if (micon)
    sprintf(sblocks[i], "%cM: %.f%%%c", blocks[i].signal, micperc, blocks[i].signal);
  else
    sprintf(sblocks[i], "%cM: MUTE%c", blocks[i].signal, blocks[i].signal);
}

void cmdspk(int i)
{
  readvolume(0, &minspk, &maxspk, &rspk, &spkon);
  updatespkperc();
  if (spkon)
    sprintf(sblocks[i], "%cV: %.f%%%c", blocks[i].signal, spkperc, blocks[i].signal);
  else
    sprintf(sblocks[i], "%cV: MUTE%c", blocks[i].signal, blocks[i].signal);
}

void readstatus()
{
  for (int i = 0; i < LENGTH(blocks); i++) {
    switch (blocks[i].cmd) {
    case CmdDate:
      cmddate(i);
      break;
    case CmdBat:
      cmdbat(i);
      break;
    case CmdBri:
      cmdbri(i);
      break;
    case CmdMic:
      cmdmic(i);
      break;
    case CmdSpk:
      cmdspk(i);
      break;
    }
  }
}

void handlesignal(int sig, siginfo_t *info, void *ctx)
{
  if (info->si_value.sival_int) {
    for (int i = 0; i < LENGTH(blocks); i++) {
      if (blocks[i].signal != 0 && blocks[i].signal == info->si_value.sival_int) {
        switch (blocks[i].cmd) {
        case CmdDate:
          cmddate(i);
          break;
        case CmdBat:
          cmdbat(i);
          break;
        case CmdBri:
          cmdbri(i);
          break;
        case CmdMic:
          cmdmic(i);
          break;
        case CmdSpk:
          cmdspk(i);
          break;
        }
      }
    }
  }
}

void run(Display *dpy)
{
  time_t now;
  struct tm *tm_info;
  Window root;
  char status[MAXLEN];
  int blen;

  now = time(NULL);
  tm_info = localtime(&now);
  counter = tm_info->tm_sec;
  counter = counter > 0 ? counter - 1 : counter;

  root = DefaultRootWindow(dpy);
  blen = LENGTH(blocks);

  while (1) {
    memset(status, 0, MAXLEN);
    if ((++counter) == 60)
      counter = 0;
    for (int i = 0; i < blen; i++) {
      if (blocks[i].interval == 0)
        continue;
      if (blocks[i].interval == counter || (blocks[i].interval == 60 && counter == 0)) {
        switch (blocks[i].cmd) {
        case CmdDate:
          cmddate(i);
          break;
        case CmdBat:
          cmdbat(i);
          break;
        case CmdBri:
          cmdbri(i);
          break;
        case CmdMic:
          cmdmic(i);
          break;
        case CmdSpk:
          cmdspk(i);
          break;
        }
      }
    }
    for (int i = blen - 1; i >= 0; i--) {
      strcat(status, sblocks[i]);
      if (i > 0)
        strcat(status, DELIMITER);
    }
    if (debug) {
      printf("%s\n", status);
    } else {
      XStoreName(dpy, root, status);
      XFlush(dpy);
    }
    sleep(1);
  }
}

int main(int argc, char *argv[])
{
  Display *dpy;
  struct sigaction sa;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-d")) {
      debug = 1;
    }
  }

  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "can't open display");
    return EXIT_FAILURE;
  }

  sa.sa_sigaction = handlesignal;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGRTMIN, &sa, NULL);

  readstatus();
  run(dpy);
  XCloseDisplay(dpy);
  return EXIT_SUCCESS;
}
