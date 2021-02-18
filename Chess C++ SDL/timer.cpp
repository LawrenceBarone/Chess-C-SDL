#include "timer.hpp"
#include <SDL_image.h>
#include <string> 

constexpr int SCREEN_WIDTH = 600;
constexpr int SCREEN_HEIGHT = 200;

namespace timer {

	void timer::init() {
		//text init
		TTF_Init();

		SDL_Color textColor = { 0, 0, 0 };
		font = TTF_OpenFont("font/arial.ttf", 100);

		textSurface[0] = TTF_RenderText_Solid(font, "00:00", textColor);
		textSurface[1] = TTF_RenderText_Solid(font, "00:00", textColor);

		bkSurface = IMG_Load("imgs/timerBK.png");
	}

	void timer::render(int player1, int player2) {
		int min;
		int second;

		SDL_RenderClear(renderer);

		SDL_FreeSurface(textSurface[0]);
		SDL_FreeSurface(textSurface[1]);

		SDL_Rect rect;
		rect.w = SCREEN_WIDTH;
		rect.h = SCREEN_HEIGHT;
		rect.x = 0;
		rect.y = 0;

		//render bk
		bkTexture = SDL_CreateTextureFromSurface(renderer, bkSurface);
		SDL_RenderCopy(renderer, bkTexture, 0, &rect);
		SDL_DestroyTexture(bkTexture);


		SDL_Color textColor = { 0, 0, 0 };
		rect.w = SCREEN_WIDTH/2;
		rect.h = SCREEN_HEIGHT-60;
		rect.y = 60;

		min = player1 / 60000;
		second = (player1 % 60000) / 1000;

		std::string minText = std::to_string(min);
		std::string secondText = std::to_string(second);

		if (minText.length() < 2) {
			minText = "0" + std::to_string(min);

		}
		if (secondText.length() < 2) {
			secondText = "0" + std::to_string(second);

		}

		std::string s = minText + ":" + secondText;
		const char* c = s.c_str();

		textSurface[0] = TTF_RenderText_Solid(font, c, textColor); //insert correct time
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface[0]);
		//afficher
		SDL_RenderCopy(renderer, textTexture, 0, &rect);
		//libérer
		SDL_DestroyTexture(textTexture);

		min = player2 / 60000;
		second = (player2 % 60000) / 1000;

		minText = std::to_string(min);
		secondText = std::to_string(second);

		if (minText.length() < 2) {
			minText = "0" + std::to_string(min);

		}
		if (secondText.length() < 2) {
			secondText = "0" + std::to_string(second);

		}

		s = minText + ":" + secondText;
		c = s.c_str();

		textSurface[1] = TTF_RenderText_Solid(font, c, textColor); //insert correct time
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface[1]);

		rect.x = SCREEN_WIDTH / 2;

		//afficher
		SDL_RenderCopy(renderer, textTexture, 0, &rect);
		//libérer
		SDL_DestroyTexture(textTexture);

		SDL_RenderPresent(renderer);
	}

	timer::timer() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//create the window
			window = SDL_CreateWindow("Chess Timer", 660, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

			if (window == NULL)
			{
				printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			}
			else
			{
				//create renderer
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			}
		}
	}

	timer::~timer() {

		SDL_FreeSurface(textSurface[0]);
		SDL_FreeSurface(textSurface[1]);
		SDL_DestroyTexture(textTexture);

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		//text close
		TTF_CloseFont(font);
	}
}
