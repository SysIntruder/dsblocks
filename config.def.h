/* See LICENSE file for copyright and license details. */

#define CAPACITY "/sys/class/power_supply/BAT0/capacity"
#define ADAPTER "/sys/class/power_supply/ADP0/online"
#define BRIGHTNESS "/sys/class/backlight/amdgpu_bl1/brightness"
#define MAX_BRIGHTNESS "/sys/class/backlight/amdgpu_bl1/max_brightness"
#define CAPTURE "Capture"
#define PLAYBACK "Master"
#define STEP 5
#define PFXLEN 4    /* longest pfx + 1 */
#define MAXLEN 2048 /* more than BLOCKLEN * number of blocks */
#define DELIMITER " "

/* signal 1 will be ignored */
static const Block blocks[] = {
    { CmdDate, 60, 1, ""    },
    { CmdBat,  60, 2, "P: " },
    { CmdBri,  0,  3, "B: " },
    { CmdMic,  0,  4, "M: " },
    { CmdSpk,  0,  5, "S: " },
};
