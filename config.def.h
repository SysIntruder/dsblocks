#define CAPACITY "/sys/class/power_supply/BAT0/capacity"
#define ADAPTER "/sys/class/power_supply/ADP0/online"
#define BRIGHTNESS "/sys/class/backlight/amdgpu_bl1/brightness"
#define MAX_BRIGHTNESS "/sys/class/backlight/amdgpu_bl1/max_brightness"
#define CAPTURE "Capture"
#define PLAYBACK "Master"
#define STEP 5
#define BLOCKLEN 25
#define MAXLEN 2048
#define DELIMITER " | "

static const Block blocks[] = {
    {CmdDate, 60, 0},
    {CmdBat, 0, 1},
    {CmdBri, 0, 2},
    {CmdMic, 0, 3},
    {CmdSpk, 0, 4},
};
