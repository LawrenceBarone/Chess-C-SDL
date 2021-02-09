#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>

STARTUPINFO sti = {0};
SECURITY_ATTRIBUTES sats = {0};
PROCESS_INFORMATION pi = {0};
HANDLE pipin_w, pipin_r, pipout_w, pipout_r;
BYTE buffer[2048];
DWORD writ, excode, read, available;
std::string nodes;

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

void ConnectToEngine(const char* path, std::string elo)
{
   pipin_w = pipin_r = pipout_w = pipout_r = NULL;
   sats.nLength = sizeof(sats);
   sats.bInheritHandle = TRUE;
   sats.lpSecurityDescriptor = NULL;

   CreatePipe(&pipout_r, &pipout_w, &sats, 0);
   CreatePipe(&pipin_r, &pipin_w, &sats, 0);
         
   sti.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
   sti.wShowWindow = SW_HIDE;
   sti.hStdInput = pipin_r;
   sti.hStdOutput = pipout_w;
   sti.hStdError = pipout_w;

   wchar_t* path_w = convertCharArrayToLPCWSTR(path);

   // foruma to convert elo into number of nodes for the engine
   nodes = std::to_string((int)exp((stoi(elo) + 838.8755655) / 242.5737137));

   CreateProcess(NULL, path_w, NULL, NULL, TRUE,0, NULL, NULL, &sti, &pi);
}


std::string getNextMove(std::string position)
{     
    std::string str;
    position = "position startpos moves "+position+"\ngo depth 10 nodes "+ nodes + "\n";

    std::cout << position << std::endl;

    WriteFile(pipin_w, position.c_str(), position.length(),&writ, NULL);
    Sleep(500);
        
    PeekNamedPipe(pipout_r, buffer,sizeof(buffer), &read, &available, NULL);   
    int n;
    do
    {   
        std::cout << buffer << std::endl;
        ZeroMemory(buffer, sizeof(buffer));
        if(!ReadFile(pipout_r, buffer, sizeof(buffer), &read, NULL) || !read) break; 
        if (read == 2048) break;
        buffer[read] = 0;    
        str+=(char*)buffer;
    }
    while(read >= sizeof(buffer));

    n = str.find("bestmove");
    if (n!=-1) return str.substr(n+9,4);
    
    if (str == "") {
        str += (char*)buffer;
    }
    n = str.find(" pv ");
    if(n != -1) return str.substr(n + 4, 5);

    //return "ER1 next move not found";
}


void CloseConnection()
{
    WriteFile(pipin_w, "quit\n", 5,&writ, NULL);
    if(pipin_w != NULL) CloseHandle(pipin_w);
    if(pipin_r != NULL) CloseHandle(pipin_r);
    if(pipout_w != NULL) CloseHandle(pipout_w);
    if(pipout_r != NULL) CloseHandle(pipout_r);
    if(pi.hProcess != NULL) CloseHandle(pi.hProcess);
    if(pi.hThread != NULL) CloseHandle(pi.hThread);
}


#endif CONNECTOR_H
