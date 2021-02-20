#pragma once

#include <stdio.h>
#include <SDL_ttf.h>

namespace timer { 

	class timer
	{
	public:
		bool end = false;
		timer();
		~timer();
		void init();
		void render(int player1, int player2);
	private:
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;
		TTF_Font* font = NULL;
		SDL_Surface* bkSurface = NULL;
		SDL_Texture* bkTexture = NULL;
		SDL_Surface* textSurface[2] = { NULL,NULL };
		SDL_Texture* textTexture = NULL;
	};

}

