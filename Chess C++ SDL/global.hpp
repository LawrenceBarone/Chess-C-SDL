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
	int gameBaseTime = 10; //temps de base pour les deux joueurs en minute
	int secondForEachMovement = 0; //temps gagner à chaque coup en seconde
} mdata_struct;

extern mdata_struct GlobalStruct;

#endif