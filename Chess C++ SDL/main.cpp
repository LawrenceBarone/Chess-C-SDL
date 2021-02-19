#define GUI
#include "global.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include "multiple_windows.hpp"

#ifdef GUI
#include "gui.hpp"
#else
#include "utils.hpp"
#endif
#include <SDL_image.h>
#include <SDL_ttf.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

Mix_Music* music;

bool soundOn = false;


const int SCREEN_HEIGHT = 800;
const int SCREEN_WIDTH = 600;



mdata_struct GlobalStruct;


#ifndef GUI
extern void search(board::Game& game);

void consoleInterface()
{
	print("Starting Yel 1.0 Chess Engine!");
	print("");

	// string d'initialisation
	/*
	* minuscule = black
	* Majuscule = while
	* 
	*	r = rook
	*	n = knight
	*	b = bishop
	*	q = queen
	*	k = king
	*	p = pawn
	* 
	*	/ = next line
	* 
	*	8 = empty
	* 
	*	w = white begins
	* 
	* castles
	*	K = white king side
	*	Q = while queen side
	*	k = black king side
	*	q = black queen side
	* 
	*/
	const std::string Startfen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	board::Game game;
	game.init();
	utils::loadFen(Startfen, game);
	game.setPositionKey();
	game.generateMove(false);

	while (true)
	{
		print("");
		game.printBoard();
		std::cout << "Position Key: " << std::hex << game.getBoard().hash << std::endl;

		std::string turn = game.getBoard().side == defs::WHITE ? "White" : "Black";
		std::cout << turn << " Enter Move (Ex. e2e4): " << std::endl;

		std::string m;
		std::cin >> m;

		if (m == "quit") break;
		if (m == "fen")
		{
			print("Please Enter valid fen");
			std::string fen;
			std::string pos = "";
			std::string side = "";
			std::string castPerm = "";
			std::string enPass = "";
			std::string fifty = "";
			std::string fullmove = "";

			std::cin >> pos >> side >> castPerm >> enPass >> fifty >> fullmove;

			fen = pos + " " + side + " " + castPerm + " " + enPass + " " + fifty + " " + fullmove;

			game.getBoard().moves.clear();
			game.getBoard().histHash.clear();
			game.init();
			for (int i = wP; i <= bK; i++)
			{
				game.getBoard().pieces[i].clear();
			}
			utils::loadFen(fen, game);
			game.setPositionKey();
			game.generateMove(false);

			continue;
		}
		if (m == "moves")
		{
			printMoves(game.getBoard().moves);
			continue;
		}
		if (m == "takeback")
		{
			game.takeback();
			game.getBoard().moves.clear();
			game.generateMove(false);
			continue;
		}
		if (m == "go")
		{
			search(game);
			printPv(game.getBoard().pv);
			game.makeMove(game.getBoard().pv[0].m);
			game.getBoard().moves.clear();
			game.generateMove(false);
		}

		const auto move = utils::parseMove(m, game);
		if (move)
		{
			if (game.makeMove(move))
			{
				game.getBoard().moves.clear();
				game.generateMove(false);
			}
		}
	}
}
#endif

void SetGame() {

}

void runChess() {
#ifdef GUI
	gui::Gui gui;
	gui.run();
#else
	consoleInterface();
#endif
}

void close() {
	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
int main(int argc, char* argv[])
{
	//init music
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		return false;
	}

	// Load music
	music = Mix_LoadMUS("music/chopin.wav");
	if (!music) {
		return false;
	}

	// Play music forever
	if(soundOn)
		Mix_PlayMusic(music, -1);

	bool btnG = false;
	bool quit = false;
	SDL_Event e;

	GlobalStruct.wAIChoice = defs::PLAYER;
	GlobalStruct.bAIChoice = defs::STOCKFISH;
	GlobalStruct.eloStockfish = "0";
	GlobalStruct.depthLawrence = "3";
	GlobalStruct.time = "0+0";
	GlobalStruct.ModedGame = "NONE";

	int a = 1;
	SDL_Event event;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_Surface* background_surface = NULL;
	SDL_Texture* background_texture = NULL;
	
	//Images
	SDL_Surface* bStart_surface = NULL, * sExit = NULL, * sTimeF = NULL, * sH = NULL, * sHn = NULL, * sSupSign = NULL, * sInfSign = NULL, * sStartGame = NULL, * sMenu = NULL, * sStock = NULL, * sLaw = NULL,*sNoT = NULL,* sSici = NULL,*sIta = NULL, *sFlecheGB = NULL,*sFlecheDW = NULL;
	SDL_Texture* bSTart_texture = NULL, * tExit = NULL, * tTimeF = NULL, * tH = NULL, * tHn = NULL, * tSupSign = NULL, * tInfSign = NULL, * tStartGame = NULL, *tMenu = NULL, *tStock = NULL, *tLaw = NULL,*tNoT = NULL,*tSici = NULL, *tIta = NULL, * tFlecheGB = NULL, * tFlecheDW = NULL;

	//Text 
	SDL_Texture *tGameMode = NULL, *tPlayers = NULL, *txtIA = NULL, * txtLaw = NULL, *tTimeForm = NULL, *tPreformatedGames = NULL , *tEloStockfish = NULL;
	SDL_Surface* sGameMode = NULL, * sPlayers = NULL, * sIA = NULL, * sxtLaw = NULL, * sTimeForm = NULL, * sPreformatedGames = NULL, * sEloStockfish = NULL;

	if (TTF_Init() < 0) {
		std::cout << "TTF not init"  << std::endl;
	}

	TTF_Font* sans = TTF_OpenFont("fonts/Sans.ttf",30);
	SDL_Color black = { 0, 0, 0 };
	SDL_Color white = { 255, 255, 255 };
	SDL_Color grey = { 128, 128, 128 };
		
	int Mx = 0;
	int My = 0;

	

	//change elo
	char eloS[20] = "";
	char eloL[20] = "";
	int countS = 1200, countL = 1700;

	SDL_Rect menuPos;   
	menuPos.x = 0;
	menuPos.y = 0;
	menuPos.w = 800;
	menuPos.h = 600;
	
	SDL_Rect exitPos;
	exitPos.x = 616;
	exitPos.y = 512;
	exitPos.w = 114;
	exitPos.h = 43;
	
	SDL_Rect timePos;
	timePos.x = 365;
	timePos.y = 70;
	timePos.w = 365;
	timePos.h = 367;

	SDL_Rect WPos;
	WPos.x = 113;
	WPos.y = 98;
	WPos.w = 126;
	WPos.h = 39;

	SDL_Rect BPos;
	BPos.x = 113;
	BPos.y = 174;
	BPos.w = 126;
	BPos.h = 34;

	SDL_Rect StockPos;
	StockPos.x = 88;
	StockPos.y = 301;
	StockPos.w = 177;
	StockPos.h = 46;

	SDL_Rect LawPos;
	LawPos.x = 88;
	LawPos.y = 405;
	LawPos.w = 177;
	LawPos.h = 46;

	SDL_Rect StockSignPos;
	StockSignPos.x = 108;
	StockSignPos.y = 355;
	StockSignPos.w = 16;
	StockSignPos.h = 31;

	SDL_Rect LawSignPos;
	LawSignPos.x = 108;
	LawSignPos.y = 460;
	LawSignPos.w = 16;
	LawSignPos.h = 31;

	SDL_Rect NoTPos;
	NoTPos.x = 612;
	NoTPos.y = 13;
	NoTPos.w = 112;
	NoTPos.h = 50;

	SDL_Rect SiciPos;
	SiciPos.x = 480;
	SiciPos.y = 450;
	SiciPos.w = 112;
	SiciPos.h = 50;

	SDL_Rect ItaPos;
	ItaPos.x = 614;
	ItaPos.y = 450;
	ItaPos.w = 112;
	ItaPos.h = 50;

	SDL_Rect StockTextPos;
	StockTextPos.x = 140;
	StockTextPos.y = 355;
	StockTextPos.w = 100;
	StockTextPos.h = 30;

	SDL_Rect LawTextPos;
	LawTextPos.x = 140;
	LawTextPos.y = 460;
	LawTextPos.w = 100;
	LawTextPos.h = 30;

	SDL_Rect FlechePosGB; //Stock
	FlechePosGB.x = 285; //285
	FlechePosGB.y = 315; //315
	FlechePosGB.w = 30; //30
	FlechePosGB.h = 20; //20

	SDL_Rect FlechePosDW; //Stock
	FlechePosDW.x = 0; //45
	FlechePosDW.y = 0; //315
	FlechePosDW.w = 0; //30
	FlechePosDW.h = 0; //20

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"Menu Chess",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_HEIGHT,
		SCREEN_WIDTH,
		SDL_WINDOW_SHOWN
	);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	background_surface = IMG_Load("imgs/menuHor2.png");
	background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);



	sH = IMG_Load("imgs/humanblanc.png");
	tH = SDL_CreateTextureFromSurface(renderer, sH);

	

	sFlecheDW = IMG_Load("imgs/flecheNoir.png");
	tFlecheDW = SDL_CreateTextureFromSurface(renderer, sFlecheDW);

	sFlecheGB = IMG_Load("imgs/flecheNoir.png");
	tFlecheGB = SDL_CreateTextureFromSurface(renderer, sFlecheGB);

	sprintf_s(eloS, "%d", countS);
	sprintf_s(eloL, "%d", countL);

	sEloStockfish = TTF_RenderText_Solid(sans, eloS, black);
	tEloStockfish = SDL_CreateTextureFromSurface(renderer, sEloStockfish);

	sxtLaw = TTF_RenderText_Solid(sans, eloL, black);
	txtLaw = SDL_CreateTextureFromSurface(renderer, sxtLaw);

	//human or IA
	bool hw = true, hn = false;
	//Already IA W or B
	bool alreadyW = false, alreadyB = true;

	while (a) {
		SDL_GetMouseState(&Mx, &My);
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				a = 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
				
				//White Human or computer
				if (Mx >= WPos.x && Mx <= WPos.x + WPos.w && My >= WPos.y && My <= WPos.y + WPos.h) {
					if (hw == true) {
						
						sH = IMG_Load("imgs/computerblanc.png");
						tH = SDL_CreateTextureFromSurface(renderer, sH);
						//Upload IA White
						hw = false;
						
					}
					else {
						//Upload Humain White
						sH = IMG_Load("imgs/humanblanc.png");
						tH = SDL_CreateTextureFromSurface(renderer, sH);
						FlechePosDW.w = 0;
						FlechePosDW.h = 0;
						hw = true;
						GlobalStruct.wAIChoice = defs::PLAYER;
						
					}
				}
				//Black Human or computer
				if (Mx >= BPos.x && Mx <= BPos.x + BPos.w && My >= BPos.y && My <= BPos.y + BPos.h) {
					if (hn == false) {
						//Upload Humain Black
						sHn = IMG_Load("imgs/humannoir.png");
						tHn = SDL_CreateTextureFromSurface(renderer, sHn);
						FlechePosGB.w = 0;
						FlechePosGB.h = 0;
						GlobalStruct.bAIChoice = defs::PLAYER;
						
						hn = true;
					}
					else {
						//Upload IA Black
						sHn = IMG_Load("imgs/computerblack.png");
						tHn = SDL_CreateTextureFromSurface(renderer, sHn);
						hn = false;
						
						
					}
				}
				//Stockfish white or black 
				if (hw == false) {
					
					if (Mx >= StockPos.x && Mx <= StockPos.x + (StockPos.w) / 2 && My >= StockPos.y && My <= StockPos.y + StockPos.h) {
						
						FlechePosDW.x = 45;
						FlechePosDW.y = 315;
						FlechePosDW.w = 30;
						FlechePosDW.h = 25;
							GlobalStruct.wAIChoice = defs::STOCKFISH;
							
							sFlecheDW = IMG_Load("imgs/flecheBlanche.png");
							tFlecheDW = SDL_CreateTextureFromSurface(renderer, sFlecheDW);
						
					}
				}
				if (hn == false) {
					
					if (Mx >= (StockPos.x) * 2 && Mx <= StockPos.x + StockPos.w && My >= StockPos.y && My <= StockPos.y + StockPos.h) {
						
						FlechePosGB.x = 285;
						FlechePosGB.y = 315;
						FlechePosGB.w = 30;
						FlechePosGB.h = 25;

							GlobalStruct.bAIChoice = defs::STOCKFISH;
						
							sFlecheGB = IMG_Load("imgs/flecheNoir.png");
							tFlecheGB = SDL_CreateTextureFromSurface(renderer, sFlecheGB);
						
					}
				}
				
				//Lawrence white or black 
				if (hw == false) {
					
					if (Mx >= LawPos.x && Mx <= LawPos.x + LawPos.w/2 && My >= LawPos.y && My <= LawPos.y + LawPos.h) {
					
						FlechePosDW.x = 45;
						FlechePosDW.y = 415;
						FlechePosDW.w = 30;
						FlechePosDW.h = 25;
						sFlecheDW = IMG_Load("imgs/flecheBlanche.png");
						tFlecheDW = SDL_CreateTextureFromSurface(renderer, sFlecheDW);
							GlobalStruct.wAIChoice = defs::LAWRENCE;
						
						
					}
				}
				if (hn == false) {
					
					if (Mx >= (LawPos.x)*2 && Mx <= LawPos.x + LawPos.w && My >= LawPos.y && My <= LawPos.y + LawPos.h) {
						
						FlechePosGB.x = 285;
						FlechePosGB.y = 415;
						FlechePosGB.w = 30;
						FlechePosGB.h = 25;
							GlobalStruct.bAIChoice = defs::LAWRENCE;
						
							sFlecheGB = IMG_Load("imgs/flecheNoir.png");
							tFlecheGB = SDL_CreateTextureFromSurface(renderer, sFlecheGB);
						
					}
				}
				

				//Stockfish Signs elo
				if (Mx >= StockSignPos.x && Mx <= StockSignPos.x + StockSignPos.w && My >= StockSignPos.y && My <= StockSignPos.y + (StockSignPos.h)/2) {
					countS < 3200 ? countS += 100 : 3200;
					sprintf_s(eloS, "%d", countS);
					sEloStockfish = TTF_RenderText_Solid(sans, eloS, black);
					tEloStockfish = SDL_CreateTextureFromSurface(renderer, sEloStockfish);
					
				}
				else if (Mx >= StockSignPos.x && Mx <= StockSignPos.x + StockSignPos.w && My >= StockSignPos.y && My <= StockSignPos.y + StockSignPos.h) {

					countS > 1200 ? countS -= 100 : 1200; 
					sprintf_s(eloS, "%d", countS);
					sEloStockfish = TTF_RenderText_Solid(sans, eloS, black);
					tEloStockfish = SDL_CreateTextureFromSurface(renderer, sEloStockfish);
					
				}
				//Lawrence Signs elo
				if (Mx >= LawSignPos.x && Mx <= LawSignPos.x + LawSignPos.w && My >= LawSignPos.y && My <= LawSignPos.y + (LawSignPos.h) / 2) {
					countL < 1900 ? countL += 50 : 1900;
					sprintf_s(eloL, "%d", countL);
					sxtLaw = TTF_RenderText_Solid(sans, eloL, black);
					txtLaw = SDL_CreateTextureFromSurface(renderer, sxtLaw);

				}
				else if (Mx >= LawSignPos.x && Mx <= LawSignPos.x + LawSignPos.w && My >= LawSignPos.y && My <= LawSignPos.y + LawSignPos.h) {

					countL > 1600 ? countL -= 50 : 1600;
					sprintf_s(eloL, "%d", countL);
					sxtLaw = TTF_RenderText_Solid(sans, eloL, black);
					txtLaw = SDL_CreateTextureFromSurface(renderer, sxtLaw);

				}
				//Time Choice
				//Do 9 struct of the pos , or do the calcul splits of 1 square into 9 squares .
				if (Mx >= NoTPos.x && Mx <= NoTPos.x + NoTPos.w && My >= NoTPos.y && My <= NoTPos.y + NoTPos.h) {
					GlobalStruct.time = "0+0";
					a = 0;
				}
				if (Mx >= timePos.x && Mx <= timePos.x + timePos.w / 3) {
					if (My >= timePos.y && My <= timePos.y + timePos.h / 3) {
						GlobalStruct.time = "1+0";
						a = 0;
						
					}
					else if (My >= timePos.y && My <= timePos.y + (timePos.h / 3)*2) {
						GlobalStruct.time = "3+2";
						a = 0;
						
					}
					else if (My >= timePos.y && My <= timePos.y + timePos.h) {
						GlobalStruct.time = "10+0";
						a = 0;
						
					}
				}
				else if (Mx >= timePos.x && Mx <= timePos.x + (timePos.w /3) *2) {
					if (My >= timePos.y && My <= timePos.y + timePos.h / 3) {
						GlobalStruct.time = "2+1";
						a = 0;
						
					}
					else if (My >= timePos.y && My <= timePos.y + (timePos.h / 3) * 2) {
						GlobalStruct.time = "5+0";
						a = 0;
						
					}
					else if (My >= timePos.y && My <= timePos.y + timePos.h) {
						GlobalStruct.time = "10+5";
						a = 0;
						
					}
				}
				else if(Mx >= timePos.x && Mx <= timePos.x + timePos.w){
					if (My >= timePos.y && My <= timePos.y + timePos.h / 3) {
						GlobalStruct.time = "3+0";
						a = 0;
						
					}
					else if (My >= timePos.y && My <= timePos.y + (timePos.h / 3) * 2) {
						GlobalStruct.time = "5+3";
						a = 0;
						
					}
					else if (My >= timePos.y && My <= timePos.y + timePos.h) {
						GlobalStruct.time = "15+10";
						a = 0;
						
					}
				}
				//Mode Game 
				if (Mx >= SiciPos.x && Mx <= SiciPos.x + SiciPos.w && My >= SiciPos.y && My <= SiciPos.y + SiciPos.h) {
					GlobalStruct.ModedGame = "SICILIAN";
					a = 0;
				}
				if (Mx >= ItaPos.x && Mx <= ItaPos.x + ItaPos.w && My >= ItaPos.y && My <= ItaPos.y + ItaPos.h) {
					GlobalStruct.ModedGame = "ITALIAN";
					a = 0;
				}
				//Exit
				if (Mx >= exitPos.x && Mx <= exitPos.x + exitPos.w && My >= exitPos.y && My <= exitPos.y + exitPos.h) {
					return EXIT_FAILURE;
				}
				break;

			}
		}
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, background_texture, NULL, NULL);
		SDL_RenderCopy(renderer, tTimeF, NULL, &timePos);
		SDL_RenderCopy(renderer, tStartGame, NULL, &NoTPos);
		SDL_RenderCopy(renderer, tH, NULL, &WPos);
		SDL_RenderCopy(renderer, tHn, NULL, &BPos);
		SDL_RenderCopy(renderer, tStock, NULL, &StockPos);
		SDL_RenderCopy(renderer, tLaw, NULL, &LawPos);
		SDL_RenderCopy(renderer, tSupSign, NULL, &StockSignPos);
		SDL_RenderCopy(renderer, tInfSign, NULL, &LawSignPos);
		SDL_RenderCopy(renderer, tNoT, NULL, &NoTPos);
		SDL_RenderCopy(renderer, tSici, NULL, &SiciPos);
		SDL_RenderCopy(renderer, tIta, NULL, &ItaPos);
		SDL_RenderCopy(renderer, tEloStockfish, NULL, &StockTextPos);
		SDL_RenderCopy(renderer, txtLaw, NULL, &LawTextPos);
		SDL_RenderCopy(renderer, tFlecheDW, NULL, &FlechePosDW);
		SDL_RenderCopy(renderer, tFlecheGB, NULL, &FlechePosGB);
		SDL_RenderPresent(renderer);
	}
	
	SDL_DestroyTexture(background_texture);
	SDL_FreeSurface(background_surface);

	SDL_DestroyTexture(tTimeF);
	SDL_FreeSurface(sTimeF);

	SDL_DestroyTexture(tFlecheDW);
	SDL_FreeSurface(sFlecheDW);

	SDL_DestroyTexture(tFlecheGB);
	SDL_FreeSurface(sFlecheGB);

	SDL_DestroyTexture(tStartGame);
	SDL_FreeSurface(sStartGame);

	SDL_DestroyTexture(txtLaw);
	SDL_FreeSurface(sxtLaw);

	SDL_DestroyTexture(tH);
	SDL_FreeSurface(sH);

	SDL_DestroyTexture(tHn);
	SDL_FreeSurface(sHn);

	SDL_DestroyTexture(tStock);
	SDL_FreeSurface(sStock);

	SDL_DestroyTexture(tLaw);
	SDL_FreeSurface(sLaw);

	SDL_DestroyTexture(tSupSign);
	SDL_FreeSurface(sSupSign);

	SDL_DestroyTexture(tInfSign);
	SDL_FreeSurface(sInfSign);

	SDL_DestroyTexture(tNoT);
	SDL_FreeSurface(sNoT);

	SDL_DestroyTexture(tSici);
	SDL_FreeSurface(sSici);

	SDL_DestroyTexture(tIta);
	SDL_FreeSurface(sIta);

	SDL_DestroyWindow(window);
	close();

	SetGame();
	runChess();
	return 0;
}






