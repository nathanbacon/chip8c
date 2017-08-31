# chip8c

This project is an interpreter and virtual machine for chip-8. Chip-8 is an interpreted programming language that allows for simple games to be ported to a range of systems from computers to phones to graphing calculators. While commonly referred to as an emulator, in beginning projects such as this one, the chip8 system has always been a virtual machine and was not originally implemented on hardware. If one wanted to run chip-8 games on a given platform, one would implement an interpreter/virtual machine such as this one. 

Key mappings

|_ 1_ 2_ 3_ 4|          |_ 1_ 2_ 3_ 4|
|_ 5_ 6_ 7_ 8|     =>   |_ q_ w_ e_ r|
|_ 9_10_11_12|     =>   |_ a_ s_ d_ f|
|_13_14_15_16|          |_ z_ x_ c_ v|

Note: Key mappings vary by game so you have to get a feel for each game.

Mac Installation

To install, simply copy and and paste the following command to the terminal in the project directory:

gcc -o chip8c -Wall main.c sdl_helpers.c chip8.c -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2

Then to run enter the command:

./chip8c *"game name"*

A pack of public domain games is available [here](https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html).

Demo

[![CHIP8C demo](https://img.youtube.com/vi/av1oQvbHOi4/0.jpg)](https://youtu.be/av1oQvbHOi4)
