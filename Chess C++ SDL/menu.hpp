#pragma once
#include <stdio.h>
#include <SDL.h>
#include "global.hpp"
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace menu {
	class menu
	{
	public:
		menu();
		~menu();
		void init();
		void run();
	private:
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;
		bool btnG;
		bool quit;
		SDL_Surface* background_surface = NULL;
		SDL_Texture* background_texture = NULL;
		//Images
		SDL_Surface* bStart_surface = NULL, * sExit = NULL, * sTimeF = NULL, * sH = NULL, * sHn = NULL, * sSupSign = NULL, * sInfSign = NULL, * sStartGame = NULL, * sMenu = NULL, * sStock = NULL, * sLaw = NULL, * sNoT = NULL, * sSici = NULL, * sIta = NULL, * sFlecheGB = NULL, * sFlecheDW = NULL;
		SDL_Texture* bSTart_texture = NULL, * tExit = NULL, * tTimeF = NULL, * tH = NULL, * tHn = NULL, * tSupSign = NULL, * tInfSign = NULL, * tStartGame = NULL, * tMenu = NULL, * tStock = NULL, * tLaw = NULL, * tNoT = NULL, * tSici = NULL, * tIta = NULL, * tFlecheGB = NULL, * tFlecheDW = NULL;
		//Text 
		SDL_Texture* tGameMode = NULL, * tPlayers = NULL, * txtIA = NULL, * txtLaw = NULL, * tTimeForm = NULL, * tPreformatedGames = NULL, * tEloStockfish = NULL;
		SDL_Surface* sGameMode = NULL, * sPlayers = NULL, * sIA = NULL, * sxtLaw = NULL, * sTimeForm = NULL, * sPreformatedGames = NULL, * sEloStockfish = NULL;
		
		TTF_Font* sans;
		SDL_Color black;
		SDL_Color white;
		SDL_Color grey;

		int Mx = 0;
		int My = 0;

		//change elo
		char eloS[20] = "";
		char eloL[20] = "";
		int countS = 1200, countL = 1700;

		SDL_Rect menuPos;
		SDL_Rect exitPos;
		SDL_Rect timePos;
		SDL_Rect WPos;
		SDL_Rect BPos;
		SDL_Rect StockPos;
		SDL_Rect LawPos;
		SDL_Rect StockSignPos;
		SDL_Rect LawSignPos;
		SDL_Rect NoTPos;
		SDL_Rect SiciPos;
		SDL_Rect ItaPos;
		SDL_Rect StockTextPos;
		SDL_Rect LawTextPos;
		SDL_Rect FlechePosGB; //Stock
		SDL_Rect FlechePosDW; //Stock

		//human or IA
		bool hw = true, hn = false;
		//Already IA W or B
		bool alreadyW = false, alreadyB = true;
		//Global Struct vars
		int eloStock = 0, eloLaw = 0;
	};
}

