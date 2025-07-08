/* See LICENSE file for copyright and license details. */

#define CAPACITY "/sys/class/power_supply/BAT0/capacity"
#define ADAPTER "/sys/class/power_supply/ADP0/online"
#define BRIGHTNESS "/sys/class/backlight/amdgpu_bl1/brightness"
#define MAX_BRIGHTNESS "/sys/class/backlight/amdgpu_bl1/max_brightness"
#define CAPTURE "Capture"
#define PLAYBACK "Master"
#define RECORDPID "/tmp/record"
#define STEP 5
#define BLOCKLEN 25
#define MAXLEN 2048
#define DELIMITER " | "

/* signal 1 will be ignored */
static const Block blocks[] = {
    {CmdDate, 60, 1},
    {CmdBat, 20, 2},
    {CmdBri, 0, 3},
    {CmdMic, 0, 4},
    {CmdSpk, 0, 5},
    {CmdRecord, 0, 6},
};
