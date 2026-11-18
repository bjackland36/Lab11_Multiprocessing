# Lab11_Multiprocessing
Last Update to README:

11/18/2026 by Blake Ackland

## Description of Program:


## Build Instructions:
This program provides a Makefile to make building this program easier. To build in Ubuntu, simply type 'make'. To build without the Makefile, run the following command: 'gcc -Wall mandel.c jpegrw.c -o mandel'
To remove '.d' and '.o' files, type 'make clean' in Ubuntu.

## Run Instructions:
To run this program, there is an option of './mandel' which will run the standard program, or './mandel -h' which will display a summary of program usage before running the standard program.

## Program Commands:
This program supports the following commands:



## Program Limitations:
This program currently only supports 1 character names for vectors in memory.
This program clears memory when 'load' command successfully finds file, meaning an invalid read still clears vector memory.