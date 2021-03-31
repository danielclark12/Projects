
•The purpose of your program
The purpose of our program is to use the map and reduce functions to count the amount of words of different lengths
across large sets of files. The final result of the calculations made will be stored in a finalData output folder.


•How to compile the program
We used a makefile to compile our program. The makefile contains all of the commands that are needed to compile each C file that is used.
Once in the code directory, the command "make" can be typed into the terminal for compillation of the program.


•What exactly your program does
Our mapreduce function is the main driver of the program and it calls multiple mappers and reducers based off of the input args
and uses system calls such as fork() and execl() to create child processes that call mapper.c or reducer.c. Our intermediate
data structure is an array with each index being the length of the word and the value being the frequency of that length.
Our program then writes the data to the intermediate data folder with each folder representing the length of words. Depending
on what mapperID is being used, the file name is changed to be the mapperID.txt where the key,value pair is stored.

The generated m_mapperID.txt files for each length in the intermediate data folder are then accessed by our reducers which keeps track
of the total number of words of a given length across all of the mappers.

•Any assumptions outside this document
To run the program I assume that ./mapreduce intArg intArg CharFileDirArg will be used with intArg being the number
of mappers and reducers and fileDir to be the path to the file that will be read.


