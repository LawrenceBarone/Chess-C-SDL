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
    defs::Choice wAIChoice = defs::Choice::LAWRENCE;
    defs::Choice bAIChoice = defs::Choice::LAWRENCE;
    string eloChoice = "0";
    string time = "15+5";
    string ModedGame = "NONE";
} mdata_struct;

extern mdata_struct GlobalStruct;

#endif