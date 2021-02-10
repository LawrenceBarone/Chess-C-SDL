#define GUI
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>

#ifdef GUI
#include "gui.hpp"
#else
#include "utils.hpp"
#endif

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Mix_Music* music;

bool soundOn = false;

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

int main(int argc, char* args[])
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

	/*bool btnG = false;
	bool quit = false;
	SDL_Event e;

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	while (!quit) {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("SDL couldnt init ! Error: %s\n", SDL_GetError());
		}
		else {
			//create window
			window = SDL_CreateWindow("Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
			if (window == NULL) {
				printf("Window could not be created! %s\n", SDL_GetError());
			}
			else {
				screenSurface = SDL_GetWindowSurface(window);
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));
				SDL_UpdateWindowSurface(window);
				SDL_Delay(2000);
			}

		}
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}
	}
	SDL_DestroyWindow(window);
	SDL_Quit();*/

	

#ifdef GUI
	//if (btnG == true) {
		gui::Gui gui;
		gui.run();
	//}
	
#else
	consoleInterface();
#endif
	return 0;
}