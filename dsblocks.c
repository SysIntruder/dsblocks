/* See LICENSE file for copyright and license details. */

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

enum { BtnLeft = 1,
       BtnMid,
       BtnRight,
       BtnScUp,
       BtnScDn };

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
static int counter, debug, update, error;

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
  char tmp[BLOCKLEN];

  readdate(tmp, BLOCKLEN);
  sprintf(sblocks[i], "%c%s%c", blocks[i].signal, tmp, blocks[i].signal);
}

void cmdbat(int i)
{
  readbattery(&batcap, &batadp);
  if (batadp)
    sprintf(sblocks[i], "%cP: %d%%+%c", blocks[i].signal, batcap, blocks[i].signal);
  else
    sprintf(sblocks[i], "%cP: %d%%%c", blocks[i].signal, batcap, blocks[i].signal);
}

void clkbri(int b)
{
  switch (b) {
  case BtnScUp:
    changebrightness(1, STEP, &rbri, &maxbri);
    break;
  case BtnScDn:
    changebrightness(0, STEP, &rbri, &maxbri);
    break;
  }
}

void cmdbri(int i, int b)
{
  if (b)
    clkbri(b);
  readbrightness(&rbri, &maxbri);
  updatebriperc();
  sprintf(sblocks[i], "%cB: %.f%%%c", blocks[i].signal, briperc, blocks[i].signal);
}

void clkmic(int b)
{
  switch (b) {
  case BtnLeft:
    togglemute(1, &micon);
    break;
  case BtnScUp:
    changevolume(1, 1, STEP, &minmic, &maxmic, &rmic);
    break;
  case BtnScDn:
    changevolume(0, 1, STEP, &minmic, &maxmic, &rmic);
    break;
  }
}

void cmdmic(int i, int b)
{
  if (b)
    clkmic(b);
  readvolume(1, &minmic, &maxmic, &rmic, &micon);
  updatemicperc();
  if (micon)
    sprintf(sblocks[i], "%cM: %.f%%%c", blocks[i].signal, micperc, blocks[i].signal);
  else
    sprintf(sblocks[i], "%cM: MUTE%c", blocks[i].signal, blocks[i].signal);
}

void clkspk(int b)
{
  switch (b) {
  case BtnLeft:
    togglemute(0, &spkon);
    break;
  case BtnScUp:
    changevolume(1, 0, STEP, &minspk, &maxspk, &rspk);
    break;
  case BtnScDn:
    changevolume(0, 0, STEP, &minspk, &maxspk, &rspk);
    break;
  }
}

void cmdspk(int i, int b)
{
  if (b)
    clkspk(b);
  readvolume(0, &minspk, &maxspk, &rspk, &spkon);
  updatespkperc();
  if (spkon)
    sprintf(sblocks[i], "%cV: %.f%%%c", blocks[i].signal, spkperc, blocks[i].signal);
  else
    sprintf(sblocks[i], "%cV: MUTE%c", blocks[i].signal, blocks[i].signal);
}

void runcmd(int i, int b)
{
  switch (blocks[i].cmd) {
  case CmdDate:
    cmddate(i);
    break;
  case CmdBat:
    cmdbat(i);
    break;
  case CmdBri:
    cmdbri(i, b);
    break;
  case CmdMic:
    cmdmic(i, b);
    break;
  case CmdSpk:
    cmdspk(i, b);
    break;
  }
}

void sighandler(int signum, siginfo_t *si, void *ucontext)
{
  int sig, btn;

  sig = signum - SIGRTMIN;
  btn = si->si_value.sival_int;
  for (int i = 0; i < LENGTH(blocks); i++) {
    if (blocks[i].signal == sig) {
      update = 1;
      runcmd(i, btn);
    }
  }
}

void setup()
{
  struct sigaction sa;

  sa.sa_sigaction = sighandler;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  update = 1;
  for (int i = 0; i < LENGTH(blocks); i++) {
    if (!blocks[i].signal) {
      printf("use 1 to ignore signal, aborting\n");
      error = 1;
      break;
    } else if (blocks[i].signal != 1) {
      sigaction(SIGRTMIN + blocks[i].signal, &sa, NULL);
    }
    runcmd(i, 0);
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

  while (!error) {
    if ((++counter) == 60)
      counter = 0;
    for (int i = 0; i < blen; i++) {
      if (blocks[i].interval == 0)
        continue;
      if ((counter % blocks[i].interval) || (blocks[i].interval == 60 && counter == 0)) {
        update = 1;
        runcmd(i, 0);
      }
    }
    if (update) {
      memset(status, 0, MAXLEN);
      for (int i = blen - 1; i >= 0; i--) {
        strcat(status, sblocks[i]);
        if (i > 0)
          strcat(status, DELIMITER);
      }
      update = 0;
    }
    if (debug) {
      printf("%d %s\n", getpid(), status);
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

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      printf("dsblocks-" VERSION "\n");
      return EXIT_SUCCESS;
    } else if (!strcmp(argv[i], "-d")) {
      debug = 1;
    } else {
      printf("usage: dsblocks [-v] [-d debug]\n");
      return EXIT_SUCCESS;
    }
  }

  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "can't open display");
    return EXIT_FAILURE;
  }

  setup();
  run(dpy);
  XCloseDisplay(dpy);
  if (error)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}
