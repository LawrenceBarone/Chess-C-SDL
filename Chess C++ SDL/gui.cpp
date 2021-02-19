#include "gui.hpp"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <list>

#include "utils.hpp"
#include "Connector.hpp"
#include "global.hpp"

extern void search(board::Game& game);

namespace gui {

using namespace defs;

constexpr int SCREEN_SIZE = 600;
static volatile bool aiThinking = false;
Uint32 mTicksCount;

TypeParty typeParty;
Choice aiChoice = defs::STOCKFISH;
std::string skill_lvl = "0";
vector<std::string> lastMovesCheck;

bool gameFinished = false;
bool hasVerify_player_canMove = false;
bool timerBool = true;

static int aiThreadSeach(void* data)
{

    ThreadData* dataThread = (ThreadData*) data;
    dataThread->gui->moveAI();

	//AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI ||
	if (typeParty == AI_vs_AI) {
		if (dataThread->gui->AI == BLACK)
		{
            aiChoice = GlobalStruct.wAIChoice;
			dataThread->gui->AI = WHITE;
		}
		else
		{
            aiChoice = GlobalStruct.bAIChoice;
			dataThread->gui->AI = BLACK;
		}
		dataThread->gui->render();
	}

    return 0;
}

Gui::Gui()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
		//create the window
        window = SDL_CreateWindow("Ludus Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN);

        if(window == NULL)
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

//Read png 
void Gui::initSurface()
{
	//initialise all images
    tileSurface[0] = IMG_Load("imgs/whiteSqr.png");
    tileSurface[1] = IMG_Load("imgs/blackSqr.png");
    tileSurface[2] = IMG_Load("imgs/lastmovelight.png");
    tileSurface[3] = IMG_Load("imgs/lastmovedark.png");
    //ajout tile pour clic droit (montrer un move)

    pieceSurface[0] = IMG_Load("imgs/wP.png");
    pieceSurface[1] = IMG_Load("imgs/wN.png");
    pieceSurface[2] = IMG_Load("imgs/wB.png");
    pieceSurface[3] = IMG_Load("imgs/wR.png");
    pieceSurface[4] = IMG_Load("imgs/wQ.png");
    pieceSurface[5] = IMG_Load("imgs/wK.png");
    pieceSurface[6] = IMG_Load("imgs/bP.png");
    pieceSurface[7] = IMG_Load("imgs/bN.png");
    pieceSurface[8] = IMG_Load("imgs/bB.png");
    pieceSurface[9] = IMG_Load("imgs/bR.png");
    pieceSurface[10] = IMG_Load("imgs/bQ.png");
    pieceSurface[11] = IMG_Load("imgs/bK.png");

	//textSurface 	//font || font || font || font || font || font ||

	SDL_Color textColor = { 0, 0, 0 };
	font = TTF_OpenFont("font/arial.ttf", 100);

	textSurface[0] = TTF_RenderText_Blended(font, "a", textColor);
	textSurface[1] = TTF_RenderText_Blended(font, "b", textColor);
	textSurface[2] = TTF_RenderText_Blended(font, "c", textColor);
	textSurface[3] = TTF_RenderText_Blended(font, "d", textColor);
	textSurface[4] = TTF_RenderText_Blended(font, "e", textColor);
	textSurface[5] = TTF_RenderText_Blended(font, "f", textColor);
	textSurface[6] = TTF_RenderText_Blended(font, "g", textColor);
	textSurface[7] = TTF_RenderText_Blended(font, "h", textColor);

    textSurface[8] = TTF_RenderText_Blended(font, "1", textColor);
	textSurface[9] = TTF_RenderText_Blended(font, "2", textColor);
	textSurface[10] = TTF_RenderText_Blended(font, "3", textColor);
	textSurface[11] = TTF_RenderText_Blended(font, "4", textColor);
	textSurface[12] = TTF_RenderText_Blended(font, "5", textColor);
	textSurface[13] = TTF_RenderText_Blended(font, "6", textColor);
	textSurface[14] = TTF_RenderText_Blended(font, "7", textColor);
	textSurface[15] = TTF_RenderText_Blended(font, "8", textColor);

    promoteSqrSurface = IMG_Load("imgs/promoteSqr.png");
    promoteTexture = SDL_CreateTextureFromSurface(renderer, promoteSqrSurface);
    lastmoveTextureLight = SDL_CreateTextureFromSurface(renderer, tileSurface[2]);
    lastmoveTextureDark = SDL_CreateTextureFromSurface(renderer, tileSurface[3]);

	//set icon
    SDL_SetWindowIcon(window, pieceSurface[7]);
}

void Gui::initBoard()
{
    bool isWhite = false;
    int tileSize = SCREEN_SIZE / 8;
	
    for(Sqr rank=RANK_8; rank>=RANK_1; --rank)
    {
        for(Sqr file=FILE_A; file<=FILE_H; ++file)
        {
            auto sqr = fRSqr(file, rank);

            tiles.push_back(Tile(tileSize * file, tileSize * rank, tileSize, file, rank, sqr, isWhite, renderer));
            isWhite = !isWhite;
        }
        isWhite = !isWhite;
    }

    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].initTexture(tileSurface, i);
    }
}

void Gui::initPieces()
{
    for (int i=0; i<64; i++)
    {
        if (game.getBoard().position[mailbox64[i]] != EMPTY)
        {
            tiles[i].setPiece(new GuiPiece(tiles[i].getPosition().x, tiles[i].getPosition().y,
                tiles[i].getPosition().w, tiles[i].getSqr(), game.getBoard().position[mailbox64[i]], renderer));
            tiles[i].getPiece()->initTexture(pieceSurface[game.getBoard().position[mailbox64[i]]-1]);
        }
    }
}

void Gui::init()
{
    skill_lvl = GlobalStruct.eloStockfish;

    extern int maxDepth;
    maxDepth = stoi(GlobalStruct.depthLawrence);

    ConnectToEngine("stockfish.exe", skill_lvl);

    if (GlobalStruct.wAIChoice == PLAYER) {
        if (GlobalStruct.bAIChoice == PLAYER)
            typeParty = P_vs_P;
        else {
            typeParty = P_vs_AI;
            aiChoice = GlobalStruct.bAIChoice;
        }
    }
    else {
        if (GlobalStruct.bAIChoice == PLAYER) {
            typeParty = P_vs_AI;
        }
        else {
            typeParty = AI_vs_AI;
        }
        aiChoice = GlobalStruct.wAIChoice;
    }

    timedGame = GlobalStruct.time != "0+0";

   
    //timer window
    if (timedGame && typeParty != AI_vs_AI) {
        cout << "IA VS IA timer" << endl;
        timePlayer1 = stoi(GlobalStruct.time.substr(0, GlobalStruct.time.find('+'))) * 60 * 1000;
        timePlayer2 = timePlayer1;

        secondForMovement = stoi(GlobalStruct.time.substr(GlobalStruct.time.find('+') + 1, GlobalStruct.time.size())) * 1000;

        timerWindow.init();
    }
    else {
        timerWindow.~timer();
    }
    
    SDL_Init(SDL_INIT_EVERYTHING);

	//text init
	TTF_Init();

    initSurface();

    for (int i = 0; i < 2; ++i)
    {
        lastmovePosition.lastmovePosition[i].w = SCREEN_SIZE / 8;
        lastmovePosition.lastmovePosition[i].h = SCREEN_SIZE / 8;
    }

    const std::string Startfen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    game.init();
    utils::loadFen(Startfen, game);
    game.setPositionKey();
    game.generateMove(false);

    initBoard();
    initPieces();

    threadData = new ThreadData;
    threadData->gui = this;

	//choix de la couleur || choix de la couleur || choix de la couleur || choix de la couleur || choix de la couleur || choix de la couleur || choix de la couleur || 
	if(typeParty == P_vs_AI && GlobalStruct.bAIChoice == PLAYER){
        switchSide();
	}

	//IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA ||
	if (typeParty == AI_vs_AI) {
        AI = WHITE;
	}
}

//Launch General User interface
void Gui::run()
{
    init();

    running = true;

    while (running)
    {
        switch (typeParty)
        {
        case defs::P_vs_AI:

            handleInput();
            update_AI();

            break;
        case defs::P_vs_P:
            handleInput();

            break;
        case defs::AI_vs_AI:
            SDL_Event e;

            if (SDL_PollEvent(&e))
            {
                if (e.type == SDL_KEYDOWN)
                {
                    handleKeyDown(e);
                }     
            }
            update_AI();
            break;
        default:
            break;
        }

        render();

        if (typeParty != AI_vs_AI) {
            TimerUpdate();
        }
       
    }

    CloseConnection(); // close stockfish
}

void Gui::TimerUpdate() {
    if (timerBool) {
        timerBool = false;

        //timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode
        if (timedGame && !gameFinished) {
            float deltaTime = (SDL_GetTicks() - mTicksCount);
            mTicksCount = SDL_GetTicks();

            if (game.getBoard().side == WHITE) { //wich side is playing
                timePlayer1 -= deltaTime;
                if (timePlayer1 <= 0) {
                    //player 1 has no time left
                    gameFinished = true;

                    cout << "player 1 has no time left" << endl;
                }
            }
            else
            {
                timePlayer2 -= deltaTime;
                if (timePlayer2 <= 0) {
                    //player 2 has no time left
                    gameFinished = true;

                    cout << "player 2 has no time left" << endl;
                }
            }

            timerWindow.render(timePlayer1, timePlayer2);
        }
    }

    if (SDL_GetTicks() - mTicksCount > 1000)
        timerBool = true;
   
}

void Gui::checkGameStatus() {
    std::string lastMove = "";
    switch (checkMate(game.getHistoPos_stockfish()))
    {
    case CHECKMATE:
        std::cout << (game.getBoard().side == WHITE ? "white" : "black") << " checkmate" << std::endl;
        gameFinished = true;
        break;
    case STALEMATE:
        std::cout << "stalemate" << std::endl;
        gameFinished = true;
        break;
    case CHECK:
        

        if (lastMovesCheck.size() >= 6) {
            lastMovesCheck.erase(lastMovesCheck.begin());
            
        }

        for (int i = game.getHistoPos_stockfish().size() - 2; i > 0 && game.getHistoPos_stockfish()[i] != ' '; i--) {
            lastMove += game.getHistoPos_stockfish()[i];
        }
        lastMovesCheck.push_back(lastMove);

        if (lastMovesCheck.size() >= 6) {
            if ((lastMovesCheck[0] == lastMovesCheck[2] && lastMovesCheck[2] == lastMovesCheck[4]) &&
                (lastMovesCheck[1] == lastMovesCheck[3] && lastMovesCheck[3] == lastMovesCheck[5])) {

                std::cout << "stalemate" << std::endl;
                gameFinished = true;
            }
        }
        break;
    default:
        break;
    }
}

void Gui::handleInput()
{
    if (!hasVerify_player_canMove) {
        hasVerify_player_canMove = true;
        
        checkGameStatus();
    }

    SDL_Event e;

    if (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT || (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE))
        {
            running = false;
            return;
        }

        if (aiThinking) return;

        if (e.type == SDL_KEYDOWN)
        {
            handleKeyDown(e);
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
			if(!gameFinished)
			    handleMouseDown(e);
        }
        else if (e.type == SDL_MOUSEMOTION)
        {
            handleMouseMotion(e);

        }
        else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
        {
			if (promotePieceIndex != -1)
			{
				handlePromoteMove();
			}

			else if (pieceMovingInfo.pieceMoving != NULL)
			{
				movePiece(e);
			}
        }
    }
}

void Gui::handleKeyDown(const SDL_Event& e)
{
    switch(e.key.keysym.sym)
    {
        case SDLK_n:
        {
			//n for new game
            newGame();
            break;
        }
        case SDLK_SPACE:
        {
            running = false;
            break;
        }
    }
}

void Gui::newGame()
{
    clearPieces();
    for(int i=wP; i<=bK; i++)
    {
        game.getBoard().pieces[i].clear();
    }
    game.init();
    game.getBoard().histHash.clear();
    game.getBoard().moveHistory.clear();
    game.getBoard().moves.clear();

    const std::string Startfen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    utils::loadFen(Startfen, game);
    game.setPositionKey();
    game.generateMove(false);
    initPieces();
    lastMoveChecker = false;
    render();
}

void Gui::switchSide()
{
    if (AI == BLACK)
    {
        AI = WHITE;
    }
    else
    {
        AI = BLACK;
    }
    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].invert(SCREEN_SIZE/8);
    }
    lastMoveChecker = false;
    clearPieces();
    initPieces();
    render();
}

void Gui::handleMouseDown(const SDL_Event& e)
{
    if (e.button.button != SDL_BUTTON_LEFT) return;

    if (promoting)
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        SDL_Rect mousePos {mouseX, mouseY, 1, 1};

        for (int i=0; i<4; i++)
        {
           if (!(
                ( ( mousePos.y + mousePos.h ) < ( promoteToPieces[i].y ) ) ||
                ( mousePos.y > ( promoteToPieces[i].y + promoteToPieces[i].h ) ) ||
                ( ( mousePos.x + mousePos.w ) < promoteToPieces[i].x ) ||
                ( mousePos.x > ( promoteToPieces[i].x + promoteToPieces[i].w ) )
            ))
            {
                promotePieceIndex = i;
                break;
            }
        }
    }
    else if (game.getBoard().side != AI)
    {
        for (int i=0; i<64; i++)
        {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            SDL_Rect mousePos {mouseX, mouseY, 1, 1};

            if (tiles[i].getPiece() == NULL) continue;
            if (pieceColor[tiles[i].getPiece()->type] != game.getBoard().side) continue;

            if (tiles[i].isCollide(mousePos))
            {
                pieceMovingInfo.from = mailbox64[i];
                pieceMovingInfo.pieceMoving = tiles[i].getPiece();
                pieceMovingInfo.tile = &tiles[i];
                break;
            }
        }
    }
}

void Gui::movePiece(const SDL_Event& e) //when player move a piece
{
    for (int i=0; i<64; i++)
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        SDL_Rect mousePos {mouseX, mouseY, 1, 1};

        if (!tiles[i].isCollide(mousePos)) continue;

        pieceMovingInfo.to = mailbox64[i];

        if (isPawn[pieceMovingInfo.pieceMoving->type] && isPromotionSqr(pieceMovingInfo.to))
        {
            checkPromotionMove();
        }
        else
        {
            Move move = 0;
            moveFromTo(move, pieceMovingInfo.from, pieceMovingInfo.to);

            move = utils::parseMove(move, game);

            if (move != 0)
            {
				//printf("maybe\n");
				if (game.makeMove(move)) {

					//timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode 
					if (game.getBoard().side != WHITE) { //invert because of game.makeMove
						timePlayer1 += secondForMovement;
						
					}
					else
					{
						timePlayer2 += secondForMovement;
						
					}

					updatePieceLocation(move, i);

					//two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode ||
					if(typeParty == P_vs_P){
					    switchSide();
					}


                    //store move into histoPosition_stockfish
                    std::string fromPos = "";
                    std::string toPos = "";
                    for (int i = 0; i < 64; i++) {
                        if (mailbox64[i] == pieceMovingInfo.from)
                            fromPos = sqrChar[i];
                        if (mailbox64[i] == pieceMovingInfo.to)
                            toPos = sqrChar[i];
                    }
                    game.addToHistoPos_stockfish(fromPos + toPos);
				}
				else {
					pieceMovingInfo.tile->alignPiece();
				}
            }
            else
            {
                pieceMovingInfo.tile->alignPiece();
            }

            pieceMovingInfo.pieceMoving = NULL;
            pieceMovingInfo.tile = NULL;
            pieceMovingInfo.from = -1;
            pieceMovingInfo.to = -1;
        }

        return;
    }
}

void Gui::updatePieceLocation(const Move& move, const int i)
{
    setLastMovePos(pieceMovingInfo.from, pieceMovingInfo.to);

    if (ISCAP(move))
    {
        if (ENPASSCAP(move))
        {
            delete tiles[mailbox[ENPASSCAP(move)]].getPiece();
            tiles[mailbox[ENPASSCAP(move)]].setPiece(NULL);
        }
        else
        {
            delete tiles[i].getPiece();
            tiles[i].setPiece(NULL);
        }
    }

    if (!castleMove(move))
    {
        tiles[i].setPiece(pieceMovingInfo.pieceMoving);
        tiles[i].setPiece(pieceMovingInfo.pieceMoving);
        tiles[i].alignPiece();
        pieceMovingInfo.pieceMoving = NULL;
        pieceMovingInfo.tile->setPiece(NULL);
    }

    render();
    game.getBoard().moves.clear();
    game.generateMove(false);
}

void Gui::checkPromotionMove()
{
    bool validPromotionSqr = (AI == BLACK) ? isPromotionSqrForWhite(pieceMovingInfo.to) : isPromotionSqrForBlack(pieceMovingInfo.to);
    bool validCapturePromote = false;

    if (AI == BLACK)
    {
        if (pieceMovingInfo.to == pieceMovingInfo.from + 9 || pieceMovingInfo.to == pieceMovingInfo.from + 11)
        {
            if (game.getBoard().position[pieceMovingInfo.to] != EMPTY)
            {
                if (pieceColor[game.getBoard().position[pieceMovingInfo.to]] == BLACK)
                    validCapturePromote = true;
            }
        }
    }
    else
    {
        if (pieceMovingInfo.to == pieceMovingInfo.from - 9 || pieceMovingInfo.to == pieceMovingInfo.from - 11)
        {
            if (game.getBoard().position[pieceMovingInfo.to] != EMPTY)
            {
                if (pieceColor[game.getBoard().position[pieceMovingInfo.to]] == WHITE)
                    validCapturePromote = true;
            }
        }
    }

    validPromotionSqr = validPromotionSqr &&
        (moveOneStraightSqr(pieceMovingInfo.from, !AI) == pieceMovingInfo.to || validCapturePromote);

    if (!validPromotionSqr)
    {
        pieceMovingInfo.tile->alignPiece();

        pieceMovingInfo.pieceMoving = NULL;
        pieceMovingInfo.tile = NULL;
        pieceMovingInfo.from = -1;
        pieceMovingInfo.to = -1;
    }
    else
    {
        promoting = true;
    }
}

void Gui::handlePromoteMove()
{
    Move move = 0;
    moveFromTo(move, pieceMovingInfo.from, pieceMovingInfo.to);

    Piece p;
    if (game.getBoard().side == WHITE)
    {
        p = wQ;
    }
    else
    {
        p = bQ;
    }

    addPromoteBits(move, p - promotePieceIndex);

    move = utils::parseMove(move, game);

    if (move != 0 && game.makeMove(move))
    {
        setLastMovePos(pieceMovingInfo.from, pieceMovingInfo.to);

        tiles[mailbox[pieceMovingInfo.to]].setPiece(tiles[mailbox[pieceMovingInfo.from]].getPiece());
        tiles[mailbox[pieceMovingInfo.from]].setPiece(NULL);
        tiles[mailbox[pieceMovingInfo.to]].alignPiece();

        tiles[mailbox[pieceMovingInfo.to]].promote(PROMOTE(move), pieceSurface[(PROMOTE(move))-1]);

        //add to histoPosition for stockfish
        std::string fromPos = "";
        std::string toPos = "";
        std::string promoteSign = "";

        for (int i = 0; i < 64; i++) {
            if (mailbox64[i] == pieceMovingInfo.from)
                fromPos = sqrChar[i];
            if (mailbox64[i] == pieceMovingInfo.to)
                toPos = sqrChar[i];
        }
        //precise what figure
        if (p - promotePieceIndex == bQ || p - promotePieceIndex == wQ)     promoteSign = "q";
        if (p - promotePieceIndex == bR || p - promotePieceIndex == wR)     promoteSign = "r";
        if (p - promotePieceIndex == bB || p - promotePieceIndex == wB)     promoteSign = "b";
        if (p - promotePieceIndex == bN || p - promotePieceIndex == wN)     promoteSign = "n";

        game.addToHistoPos_stockfish(fromPos + toPos + promoteSign);
    }   

    pieceMovingInfo.pieceMoving = NULL;
    pieceMovingInfo.tile = NULL;
    pieceMovingInfo.from = -1;
    pieceMovingInfo.to = -1;
    promoting = false;
    promotePieceIndex = -1;

    render();

    game.getBoard().moves.clear();
    game.generateMove(false);

	//two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode ||
	if(typeParty == P_vs_P){
	    switchSide();
	}

}

void Gui::setLastMovePos(int from, int to)
{
    lastmovePosition.lastmovePosition[0].x = tiles[mailbox[from]].getPosition().x;
    lastmovePosition.lastmovePosition[0].y = tiles[mailbox[from]].getPosition().y;
    lastmovePosition.lastmovePosition[1].x = tiles[mailbox[to]].getPosition().x;
    lastmovePosition.lastmovePosition[1].y = tiles[mailbox[to]].getPosition().y;

    lastmovePosition.isWhiteFrom = tiles[mailbox[from]].isWhite();
    lastmovePosition.isWhiteTo = tiles[mailbox[to]].isWhite();

    lastMoveChecker = true;
}

void Gui::handleMouseMotion(const SDL_Event& e)
{
    if (pieceMovingInfo.pieceMoving != NULL && !promoting)
    {
        int tileSize = SCREEN_SIZE / 8;
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        pieceMovingInfo.pieceMoving->position.x = mouseX - tileSize/ 2;
        pieceMovingInfo.pieceMoving->position.y = mouseY - tileSize / 2;
    }
}

void Gui::moveAI()
{
    checkGameStatus();


    if (!gameFinished) {
        Move AImove = 0;

        switch (aiChoice)
        {
        case defs::STOCKFISH:

            stockfishMove(&AImove);

            break;
        case defs::LAWRENCE:

            lawrenceMove(&AImove);
            break;
        default:

            lawrenceMove(&AImove);
            break;
        }

        if (AImove == 0) { // no move possible for IA
            std::cout << (game.getBoard().side == WHITE ? "white" : "black") << "  ia a putain de bugger checkMate" << std::endl;
            gameFinished = true;
        }
        else {
            game.makeMove(AImove);

            if (ISCAP(AImove))
            {
                if (ENPASSCAP(AImove))
                {
                    delete tiles[mailbox[ENPASSCAP(AImove)]].getPiece();
                    tiles[mailbox[ENPASSCAP(AImove)]].setPiece(NULL);
                }
                else
                {
                    delete tiles[mailbox[TO(AImove)]].getPiece();
                    tiles[mailbox[TO(AImove)]].setPiece(NULL);
                }
            }

            setLastMovePos(FROM(AImove), TO(AImove));

            if (!castleMove(AImove))
            {
                tiles[mailbox[TO(AImove)]].setPiece(tiles[mailbox[FROM(AImove)]].getPiece());
                tiles[mailbox[FROM(AImove)]].setPiece(NULL);
                tiles[mailbox[TO(AImove)]].alignPiece();

                if (PROMOTE(AImove))
                {
                    tiles[mailbox[TO(AImove)]].promote(PROMOTE(AImove), pieceSurface[(PROMOTE(AImove)) - 1]);
                }
            }

            game.getBoard().moves.clear();
            game.generateMove(false);
            aiThinking = false;
        }
        hasVerify_player_canMove = false;

        //timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode || timemode 
        if (game.getBoard().side != WHITE) { //invert because of game.makeMove
            timePlayer1 += secondForMovement;
        }
        else
        {
            timePlayer2 += secondForMovement;
        }
    }
}

void Gui::lawrenceMove(Move* AImove) {

    search(game);

    if (game.getBoard().pv[0].m == 0)
    {
        aiThinking = false;
        return;
    }

    *AImove = game.getBoard().pv[0].m;

    //store move into histoPosition_stockfish
    std::string fromPos = "";
    std::string toPos = "";
    for (int i = 0; i < 64; i++) {
        if (mailbox64[i] == FROM(*AImove))
            fromPos = sqrChar[i];
        if (mailbox64[i] == TO(*AImove))
            toPos = sqrChar[i];
        // /!\ add contition for promote parameter ::todo
    }
    game.addToHistoPos_stockfish(fromPos + toPos);

}

void Gui::stockfishMove(Move* AImove) {
    std::string str_move = getNextMove(game.getHistoPos_stockfish());

    if (str_move != "(none")
    {
        std::string oldPos_str = { str_move[0], str_move[1] };
        std::string newPos_str = { str_move[2], str_move[3] };
        std::string promote = { str_move[4] };

        pieceMovingInfo.from = -1;
        pieceMovingInfo.to = -1;

        int i = 0;
        for (i; i < 64; i++) {
            if (sqrChar[i] == oldPos_str)
                pieceMovingInfo.from = mailbox64[i];
            if (sqrChar[i] == newPos_str)
                pieceMovingInfo.to = mailbox64[i];
        }

        moveFromTo(*AImove, pieceMovingInfo.from, pieceMovingInfo.to);
        
        //promote pawn
        if (promote != " ") {
            Piece piece;
            if (AI == WHITE) {
                if (promote == "q")
                    piece = wQ;
                if (promote == "n")
                    piece = wN;
                if (promote == "b")
                    piece = wB;
                if (promote == "r")
                    piece = wR;
            }
            else {
                if (promote == "q")
                    piece = bQ;
                if (promote == "n")
                    piece = bN;
                if (promote == "b")
                    piece = bB;
                if (promote == "r")
                    piece = bR;
            }
            addPromoteBits(*AImove, piece);
        }        
        *AImove = utils::parseMove(*AImove, game);

        game.addToHistoPos_stockfish(oldPos_str + newPos_str + (promote != " " ? promote : ""));
    }
}

bool Gui::castleMove(Move move)
{
    if (CASTBITS(move))
    {
        uint8_t dir = CASTBITS(move);
        if (WKSC(dir))
        {
            tiles[mailbox[g1]].setPiece(tiles[mailbox[e1]].getPiece());
            tiles[mailbox[g1]].alignPiece();
            tiles[mailbox[e1]].setPiece(NULL);

            tiles[mailbox[f1]].setPiece(tiles[mailbox[h1]].getPiece());
            tiles[mailbox[f1]].alignPiece();
            tiles[mailbox[h1]].setPiece(NULL);

            return true;
        }
        else if (WQSC(dir))
        {
            tiles[mailbox[c1]].setPiece(tiles[mailbox[e1]].getPiece());
            tiles[mailbox[c1]].alignPiece();
            tiles[mailbox[e1]].setPiece(NULL);

            tiles[mailbox[d1]].setPiece(tiles[mailbox[a1]].getPiece());
            tiles[mailbox[d1]].alignPiece();
            tiles[mailbox[a1]].setPiece(NULL);

            return true;
        }
        else if (BKSC(dir))
        {
            tiles[mailbox[g8]].setPiece(tiles[mailbox[e8]].getPiece());
            tiles[mailbox[g8]].alignPiece();
            tiles[mailbox[e8]].setPiece(NULL);

            tiles[mailbox[f8]].setPiece(tiles[mailbox[h8]].getPiece());
            tiles[mailbox[f8]].alignPiece();
            tiles[mailbox[h8]].setPiece(NULL);

            return true;
        }
        else if (BQSC(dir))
        {
            tiles[mailbox[c8]].setPiece(tiles[mailbox[e8]].getPiece());
            tiles[mailbox[c8]].alignPiece();
            tiles[mailbox[e8]].setPiece(NULL);

            tiles[mailbox[d8]].setPiece(tiles[mailbox[a8]].getPiece());
            tiles[mailbox[d8]].alignPiece();
            tiles[mailbox[a8]].setPiece(NULL);

            return true;
        }
    }

    return false;
}

void Gui::update_AI()
{
    if (!promoting && game.getBoard().side == AI && !aiThinking)
    {
        aiThinking = true;

        SDL_CreateThread(aiThreadSeach, "AIMoveThread", threadData);
    }
}

void Gui::render()
{
    if (aiThinking) return;

    SDL_RenderClear(renderer);
	const int size = SCREEN_SIZE / 8;

    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].render();
    }

    //boucle pour render le texte des coordonée
    SDL_Rect rect;
    rect.w = size / 7;
    rect.h = size / 7;

    //lettres
    for (int i = 0; i < 8; i++)
    {
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface[i]);

        //coordonée
        rect.x = i * size + size - 10;
        rect.y = size * 7 + size - 10;

        //afficher
        SDL_RenderCopy(renderer, textTexture, 0, &rect);

        //libérer
        SDL_DestroyTexture(textTexture);
    }

    //chiffres
    for (int i = 8; i < 16; i++)
    {
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface[i]);

        //coordonée
        rect.x = 0;
        rect.y = size * (GlobalStruct.bAIChoice != PLAYER ? (7 - (i - 8)) : (i - 8));

        //afficher
        SDL_RenderCopy(renderer, textTexture, 0, &rect);

        //libérer
        SDL_DestroyTexture(textTexture);
    }

    if (lastMoveChecker)
    {
        SDL_Texture* lastmovetex = lastmovePosition.isWhiteFrom ? lastmoveTextureLight : lastmoveTextureDark;
        SDL_RenderCopy(renderer, lastmovetex, NULL, &lastmovePosition.lastmovePosition[0]);
        lastmovetex = lastmovePosition.isWhiteTo ? lastmoveTextureLight : lastmoveTextureDark;
        SDL_RenderCopy(renderer, lastmovetex, NULL, &lastmovePosition.lastmovePosition[1]);
    }

    for (int i=0; i<tiles.size(); i++)
    {
        if (tiles[i].getPiece() != NULL)
            tiles[i].getPiece()->render();
    }

    if (pieceMovingInfo.pieceMoving != NULL)
    {
        pieceMovingInfo.pieceMoving->render();
    }

	
    if (promoting)
    {
        for (int i=0; i<4; i++)
        {
            promoteToPieces[i].w = size;
            promoteToPieces[i].h = size;
            promoteToPieces[i].x = SCREEN_SIZE / 2 - size * (2 - i);
            promoteToPieces[i].y = SCREEN_SIZE / 2 - promoteToPieces[i].h / 2;

            SDL_Texture* pTexture = NULL;

            if (game.getBoard().side == WHITE)
            {
                pTexture = SDL_CreateTextureFromSurface(renderer, pieceSurface[wQ-1-i]);

            }
            else if (game.getBoard().side == BLACK)
            {
                pTexture = SDL_CreateTextureFromSurface(renderer, pieceSurface[bQ-1-i]);
            }

            SDL_RenderCopy(renderer, promoteTexture, NULL, &promoteToPieces[i]);
            SDL_RenderCopy(renderer, pTexture, NULL, &promoteToPieces[i]);
            SDL_DestroyTexture(pTexture);
        }
    }

    SDL_RenderPresent(renderer);
}

void Gui::clearPieces()
{
    for (int i=0; i<64; i++)
    {
        tiles[i].destroyPiece();
    }
}

Gui::~Gui()
{
    clearPieces();

    for (int i=0; i<3; i++)
    {
        SDL_FreeSurface(tileSurface[i]);
    }

    for (int i=0; i<12; i++)
    {
        SDL_FreeSurface(pieceSurface[i]);
    }

	for (int i = 0; i < 16; i++) {
		SDL_FreeSurface(textSurface[i]);
	}

    SDL_FreeSurface(promoteSqrSurface);
    SDL_DestroyTexture(promoteTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);


	//text close
	TTF_CloseFont(font);
	TTF_Quit();

    SDL_Quit();
}

}
