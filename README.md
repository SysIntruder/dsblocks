# DSBlocks
_simple status blocks for dwm_

DSBlocks is simple status blocks for dwm with signaling support.
## Modules
- date
- battery percentage
- laptop brightness percentage
- alsa microphone
- alsa speaker
## C Libraries Used
- lX11 (X11, for updating root window name)
- lasound (alsa, for sound control)
- lm (math)
## Setup
```
sudo make clean install
```
## Signaling Change
`signaler.c` can be used to test sending signal like this
```
./signaler <block_signal> <block_button> <pid>
```
