#pragma once

#include <stdio.h>
#include <vector>
#include <SDL_ttf.h>
#include "tile.hpp"
#include "piece.hpp"
#include "board.hpp"
#include "defs.hpp"

namespace gui {

struct PieceMoving
{
    GuiPiece* pieceMoving = NULL;
    Tile* tile = NULL;
    int from = -1;
    int to = -1;
};

struct LastMovePostion
{
    SDL_Rect lastmovePosition[2];
    bool isWhiteFrom = false;
    bool isWhiteTo = false;
};

struct ThreadData;

class Gui
{
public:
    Gui();
    ~Gui();
    void init();
    void run();
    void playerCanPlay();
    void update_AI();
    void render();
    void initBoard();
    void initSurface();
    void initPieces();
    bool castleMove(Move move);
    void moveAI();
    void lawrenceMove(Move* AImove);
    void stockfishMove(Move* AImove);
    void clearPieces();
    void handleInput();
    void handleKeyDown(const SDL_Event& e);
    void handleMouseDown(const SDL_Event& e);
    void handleMouseMotion(const SDL_Event& e);
    void movePiece(const SDL_Event& e);
    void handlePromoteMove();
    void checkGameStatus();
    void newGame();
    void switchSide();
    void setLastMovePos(int from, int to);
    void checkPromotionMove();
    void updatePieceLocation(const Move& move, const int i);
	Sqr AI = defs::BLACK;
	TTF_Font* font = NULL;

private:
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    std::vector<Tile> tiles {};
    SDL_Surface* tileSurface[4];
	SDL_Surface* textSurface[16];
    SDL_Surface* pieceSurface[12];
    PieceMoving pieceMovingInfo {};
    board::Game game {};
    bool promoting = false;
    SDL_Rect promoteToPieces[4];
    SDL_Surface* promoteSqrSurface = NULL;
    SDL_Texture* promoteTexture = NULL;
	SDL_Texture* textTexture = NULL;
    SDL_Texture* lastmoveTextureLight = NULL;
    SDL_Texture* lastmoveTextureDark = NULL;
    int promotePieceIndex = -1;
    LastMovePostion lastmovePosition;
    bool lastMoveChecker = false;
    bool running = false;
    ThreadData *threadData = NULL;
};

struct ThreadData
{
    Gui* gui = NULL;
};

void checkGameStatus();

}