#pragma once
#include <string>
using namespace std;


#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct mdata_struct
{
    string wChoice;
    string bChoice;
    string wAIChoice;
    string bAIChoice;
    string eloChoice;
    string time;
    string ModedGame;

} mdata_struct;

extern mdata_struct GlobalStruct;

#endif