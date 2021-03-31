All of the code that I wrote for this project takes place in thermo_update.c
This project revolves around using various C bit operations to simulate turning on individual lights on 
a virtual thermometer. Each number 0-9 has a specific bit pattern that is used to turn on the lights that would
show the number if we were using a physical device.

To compile the code, a makefile is used and can be compiled by typing "make" into the terminal, assuming
you are in the directory of the code.

This project also uses automated testing that runs a series of tests to ensure full functionality.
These tests can be run by typing "make test" into the terminal while in the directory of the code.

The program can also be run with your own arguments by typing ./thermo_main arg1 arg2
with arg1 being the temprature and arg2 being either C or F depending on what unit you want to use.

The second half of this involved using C's GDB to solve a series of puzzles and gave me a good understanding
on the basics of running through code with a dedicated debugger.


