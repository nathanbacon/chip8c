# chip8c

[![CHIP8C demo](https://img.youtube.com/vi/av1oQvbHOi4/0.jpg)](https://youtu.be/av1oQvbHOi4)

Controls

--------------------------------------------


Mac Installation

To install, simply copy and and paste the following command to the terminal in the project directory:

gcc -o chip8c -Wall main.c sdl_helpers.c chip8.c -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2

Then to run enter the command:

./chip8c *"game name"*

A pack of public domain games is available [here](https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html).
