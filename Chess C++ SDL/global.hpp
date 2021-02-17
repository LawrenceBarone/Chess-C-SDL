#pragma once
#include <string>
#include "defs.hpp"
using namespace std;


#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct mdata_struct
{
    string wChoice;
    string bChoice;
    defs::AIChoice wAIChoice;
    defs::AIChoice bAIChoice;
    string eloChoice;
    string time;
    string ModedGame;
	int gameBaseTime = 0;
	int secondForEachMovement = 0;
} mdata_struct;

extern mdata_struct GlobalStruct;

#endif