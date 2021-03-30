#ifndef MAPPER_H
#define MAPPER_H

#include "utils.h"

int mapperID;
char *inputFileDir;
char *intermediateDir = "output/IntermediateData"; //SIZE: 23
int interDS[MaxWordLength];

int lenArr[21];

void map(char * inputFileName);

#endif