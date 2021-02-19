#pragma once
#include <string>
#include "defs.hpp"
using namespace std;


#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct mdata_struct
{
    defs::Choice wAIChoice;
    defs::Choice bAIChoice;
    string eloStockfish;
    string depthLawrence;
    string time;
    string ModedGame;
} mdata_struct;

extern mdata_struct GlobalStruct;

#endif