#define GUI
#define DEFS
#include "global.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include "multiple_windows.hpp"
#ifdef DEFS
	#include "defs.hpp"
#endif // DEFS



#ifdef GUI
#include "gui.hpp"
#else
#include "utils.hpp"
#endif
#include <SDL_image.h>
#include <SDL_ttf.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include <SDL_ttf.h>

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
	gui.run(GlobalStruct);
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

	GlobalStruct.wChoice = "h";
	GlobalStruct.bChoice = "ia";
	GlobalStruct.wAIChoice = defs::Choice::STOCKFISH;
	GlobalStruct.bAIChoice = defs::Choice::STOCKFISH;
	GlobalStruct.eloChoice = "20";
	GlobalStruct.time = "";
	GlobalStruct.ModedGame = "";

	int a = 1;
	SDL_Event event;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_Surface* background_surface = NULL;
	SDL_Texture* background_texture = NULL;
	
	//Images
	SDL_Surface* bStart_surface = NULL, * bQuit_surface = NULL, * sTimeF = NULL, * sH = NULL, * sHn = NULL , * sSupSign = NULL , * sInfSign = NULL, *sStartGame = NULL;
	SDL_Texture* bSTart_texture = NULL, * bQuit_texture = NULL, * tTimeF = NULL, * tH = NULL, * tHn = NULL, * tSupSign = NULL, * tInfSign = NULL, * tStartGame = NULL;

	//Text 
	SDL_Texture *tGameMode = NULL, *tPlayers = NULL, *tIA = NULL, * tLaw = NULL, *tTimeForm = NULL, *tPreformatedGames = NULL , *tEloStockfish = NULL;
	SDL_Surface* sGameMode = NULL, * sPlayers = NULL, * sIA = NULL, * sLaw = NULL, * sTimeForm = NULL, * sPreformatedGames = NULL, * sEloStockfish = NULL;

	if (TTF_Init() < 0) {
		std::cout << "TTF not init"  << std::endl;
	}

	TTF_Font* sans = TTF_OpenFont("fonts/Sans.ttf",30);
	SDL_Color black = { 0, 0, 0 };
	SDL_Color white = { 255, 255, 255 };
	SDL_Color grey = { 128, 128, 128 };
		
	int Mx = 0;
	int My = 0;

	//human or IA
	bool hw = false, hn = false;

	//change elo
	char elo[20] = "";
	int count = 1200;

	SDL_Rect bStart_pos;
	bStart_pos.w = 200;
	bStart_pos.h = 100;
	bStart_pos.x = 200;
	bStart_pos.y = 100;

	SDL_Rect TimeFpos;
	TimeFpos.x = 500;
	TimeFpos.y = 250;
	TimeFpos.w = 100;
	TimeFpos.h = 100;
	

	SDL_Rect bQuit_pos;
	bQuit_pos.x = 350;
	bQuit_pos.y = 250;
	bQuit_pos.w = 100;
	bQuit_pos.h = 100;
	

	SDL_Rect Message_rect;
	Message_rect.x = 10;
	Message_rect.y = 10;
	Message_rect.w = 50;
	Message_rect.h = 30;

	SDL_Rect Players_rect;
	Players_rect.x = 40; 
	Players_rect.y = 50;
	Players_rect.w = 50;
	Players_rect.h = 30;

	SDL_Rect HorIAw;
	HorIAw.x = 170; 
	HorIAw.y = 50;
	HorIAw.w = 60;
	HorIAw.h = 40;

	SDL_Rect HorIAn;
	HorIAn.x = 420;
	HorIAn.y = 50;
	HorIAn.w = 60;
	HorIAn.h = 40;

	SDL_Rect IAtextpos;
	IAtextpos.x = 10;
	IAtextpos.y = 100;
	IAtextpos.w = 60;
	IAtextpos.h = 40;

	SDL_Rect infPos;
	infPos.x = 250;
	infPos.y = 100;
	infPos.w = 40;
	infPos.h = 40;

	SDL_Rect eloPos;
	eloPos.x = 350;
	eloPos.y = 100;
	eloPos.w = 60;
	eloPos.h = 50;

	SDL_Rect supPos;
	supPos.x = 500;
	supPos.y = 100;
	supPos.w = 40;
	supPos.h = 40;

	SDL_Rect startGPos;
	startGPos.x = 250;
	startGPos.y = 200;
	startGPos.w = 40;
	startGPos.h = 40;

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"Setting",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_HEIGHT,
		SCREEN_WIDTH,
		SDL_WINDOW_SHOWN
	);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	background_surface = IMG_Load("imgs/bgimg.jpg");
	background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);

	sH = IMG_Load("imgs/H.png");
	tH = SDL_CreateTextureFromSurface(renderer, sH);
	sHn = IMG_Load("imgs/Hn.png");
	tHn = SDL_CreateTextureFromSurface(renderer, sHn);

	sSupSign = IMG_Load("imgs/infsign.png");
	tSupSign = SDL_CreateTextureFromSurface(renderer, sSupSign);
	sInfSign = IMG_Load("imgs/supsign.png");
	tInfSign = SDL_CreateTextureFromSurface(renderer, sInfSign);

	sStartGame = IMG_Load("imgs/startButton.png");
	tStartGame = SDL_CreateTextureFromSurface(renderer, sStartGame);

	bQuit_surface = IMG_Load("imgs/wP.png");
	bQuit_texture = SDL_CreateTextureFromSurface(renderer, bQuit_surface);

	sTimeF = IMG_Load("imgs/TimeFormat.png");
	tTimeF = SDL_CreateTextureFromSurface(renderer, sTimeF);

	sGameMode = TTF_RenderText_Solid(sans, "Game Mode:", white);
	tGameMode = SDL_CreateTextureFromSurface(renderer, sGameMode);

	sPlayers = TTF_RenderText_Solid(sans, "Player 1:           vs Player 2:", white);
	tPlayers = SDL_CreateTextureFromSurface(renderer, sPlayers);

	sIA = TTF_RenderText_Solid(sans, "IA-  Stockfish:   ", white);
	tIA = SDL_CreateTextureFromSurface(renderer, sIA);

	sprintf_s(elo, "%d", count);

	sEloStockfish = TTF_RenderText_Solid(sans, elo, white);
	tEloStockfish = SDL_CreateTextureFromSurface(renderer, sEloStockfish);

	sLaw = TTF_RenderText_Solid(sans, "Lawrence: +- 1800 ", white);
	tLaw = SDL_CreateTextureFromSurface(renderer, sLaw);


	SDL_QueryTexture(tGameMode, NULL, NULL, &Message_rect.w, &Message_rect.h);
	SDL_Rect dstrect = { 0, 0, Message_rect.w, Message_rect.h };

	SDL_QueryTexture(tPlayers, NULL, NULL, &Players_rect.w, &Players_rect.h);
	SDL_Rect tdstrect = { Players_rect.x, Players_rect.y, Players_rect.w, Players_rect.h };

	SDL_QueryTexture(tIA, NULL, NULL, &IAtextpos.w, &IAtextpos.h);
	SDL_Rect tIArect = { IAtextpos.x, IAtextpos.y, IAtextpos.w, IAtextpos.h };

	SDL_QueryTexture(tIA, NULL, NULL, &IAtextpos.w, &IAtextpos.h);
	SDL_Rect tLawRect = { IAtextpos.x, IAtextpos.y+30, IAtextpos.w-10, IAtextpos.h };

	SDL_QueryTexture(tEloStockfish, NULL, NULL, &eloPos.w, &eloPos.h);
	SDL_Rect tEloRect = { eloPos.x, eloPos.y, eloPos.w, eloPos.h };

	

	while (a) {
		SDL_GetMouseState(&Mx, &My);
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				a = 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
				std::cout << "left button pressed" << std::endl;
				if (Mx >= HorIAw.x && Mx <= HorIAw.x + HorIAw.w && My >= HorIAw.y && My <= HorIAw.y + HorIAw.h) {
					if (hw == false) {
						hw = true;
						GlobalStruct.wChoice = "ia";
						sH = IMG_Load("imgs/IA.png");
						tH = SDL_CreateTextureFromSurface(renderer, sH);
						
					}
					else if (hw == true) {
						hw = false;
						GlobalStruct.wChoice = "h";
						sH = IMG_Load("imgs/H.png");
						tH = SDL_CreateTextureFromSurface(renderer, sH);
						
					}
				}
				if (Mx >= HorIAn.x && Mx <= HorIAn.x + HorIAn.w && My >= HorIAn.y && My <= HorIAn.y + HorIAn.h) {
					if (hn == false) {
						hn = true;
						GlobalStruct.bChoice = "ia";
						sH = IMG_Load("imgs/IAn.png");
						tHn = SDL_CreateTextureFromSurface(renderer, sH);
						
					}
					else if (hn == true) {
						hn = false;
						GlobalStruct.bChoice = "h";
						sH = IMG_Load("imgs/Hn.png");
						tHn = SDL_CreateTextureFromSurface(renderer, sH);
						
					}
				}
				if (Mx >= supPos.x && Mx <= supPos.x + supPos.w && My >= supPos.y && My <= supPos.y + supPos.h) {
					std::cout << "sup" << std::endl;
					if (count < 3200) {
						count += 100;
						sprintf_s(elo, "%d", count);
						GlobalStruct.eloChoice = std::to_string((count - 1200) / 100);
						std::cout << GlobalStruct.eloChoice << std::endl;
						sEloStockfish = TTF_RenderText_Solid(sans, elo, white);
						tEloStockfish = SDL_CreateTextureFromSurface(renderer, sEloStockfish);
					}
				}
				if (Mx >= infPos.x && Mx <= infPos.x + infPos.w && My >= infPos.y && My <= infPos.y + infPos.h) {
					std::cout << "inf" << std::endl;
					if (count > 1200) {
						count -= 100;
						sprintf_s(elo, "%d", count);
						GlobalStruct.eloChoice = std::to_string((count - 1200) /100);
						std::cout << GlobalStruct.eloChoice << std::endl;
						sEloStockfish = TTF_RenderText_Solid(sans, elo, white);
						tEloStockfish = SDL_CreateTextureFromSurface(renderer, sEloStockfish);
					}
					
				}
				if (Mx >= startGPos.x && Mx <= startGPos.x + startGPos.w && My >= startGPos.y && My <= startGPos.y + startGPos.h) {
					a = 0;
				}
					break;

			}
		}
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, background_texture, NULL, NULL);
		SDL_RenderCopy(renderer, bSTart_texture, NULL, &bStart_pos);
		SDL_RenderCopy(renderer, tTimeF, NULL, &TimeFpos);
		SDL_RenderCopy(renderer, bQuit_texture, NULL, &bQuit_pos);
		SDL_RenderCopy(renderer, tGameMode, NULL, &dstrect);
		SDL_RenderCopy(renderer, tH, NULL, &HorIAw);
		SDL_RenderCopy(renderer, tHn, NULL, &HorIAn);
		SDL_RenderCopy(renderer, tPlayers, NULL, &tdstrect);
		SDL_RenderCopy(renderer, tIA, NULL, &tIArect);
		SDL_RenderCopy(renderer, tLaw, NULL, &tLawRect);
		SDL_RenderCopy(renderer, tSupSign, NULL, &supPos);
		SDL_RenderCopy(renderer, tEloStockfish, NULL, &tEloRect);
		SDL_RenderCopy(renderer, tInfSign, NULL, &infPos);
		SDL_RenderCopy(renderer, tStartGame, NULL, &startGPos);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyTexture(tGameMode);
	SDL_FreeSurface(sGameMode);
	SDL_DestroyTexture(tStartGame);
	SDL_FreeSurface(sStartGame);
	SDL_DestroyTexture(tIA);
	SDL_FreeSurface(sIA);
	SDL_DestroyTexture(tH);
	SDL_FreeSurface(sH);
	SDL_DestroyTexture(tHn);
	SDL_FreeSurface(sHn);
	SDL_DestroyTexture(tPlayers);
	SDL_FreeSurface(sPlayers);
	SDL_DestroyTexture(tSupSign);
	SDL_FreeSurface(sSupSign);
	SDL_DestroyTexture(tInfSign);
	SDL_FreeSurface(sInfSign);
	SDL_DestroyTexture(bSTart_texture);
	SDL_FreeSurface(bStart_surface);
	SDL_DestroyTexture(tTimeF);
	SDL_FreeSurface(sTimeF);
	SDL_DestroyTexture(bQuit_texture);
	SDL_FreeSurface(bQuit_surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(background_texture);
	SDL_FreeSurface(background_surface);
	SDL_DestroyWindow(window);
	close();

	SetGame();
	runChess();
	return 0;
}





