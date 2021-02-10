#include "gui.hpp"

#include <SDL_image.h>
#include <algorithm>

#include "utils.hpp"
#include "Connector.hpp"
#include "global.hpp"

extern void search(board::Game& game);

namespace gui {

using namespace defs;

constexpr int SCREEN_SIZE = 600;
static volatile bool aiThinking = false;

AIChoice aiChoice = defs::STOCKFISH;
std::string skill_lvl = "0";

bool iaCantPlay = false;
bool hasVerify_player_canMove = false;
bool hasVerify_ia_canMove = false;

static int aiThreadSeach(void* data)
{

    ThreadData* dataThread = (ThreadData*) data;
    dataThread->gui->moveAI();

	//AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI || AI vs AI ||
	/*if (iavsia) {
		if (dataThread->gui->AI == BLACK)
		{
			dataThread->gui->AI = WHITE;
		}
		else
		{
			dataThread->gui->AI = BLACK;
		}
		dataThread->gui->render();
		printf("ai move");
	}*/

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
        tiles[i].initTexture(tileSurface);
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
    aiChoice = GlobalStruct.bAIChoice;
    skill_lvl = GlobalStruct.eloChoice;

    std::cout << "iachoice : " << aiChoice << ", skill_lvl=" << skill_lvl << std::endl;

    SDL_Init(SDL_INIT_EVERYTHING);
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
	//if(colorChoose == BLACK){
	//switchside();
	//}

	//IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA ||
	/*if (iavsia) {
		AI = WHITE;
	}*/
}

//Launch General User interface
void Gui::run()
{
    init();

    ConnectToEngine("stockfish.exe", skill_lvl);

    running = true;

    while (running)
    {
        handleInput();
        update_AI();
        render();

        SDL_Delay(10);
    }

    CloseConnection(); // close stockfish
}

void Gui::handleInput()
{
    if (!hasVerify_player_canMove) {
        hasVerify_player_canMove = true;
        
        switch (checkMate(game.getHistoPos_stockfish()))
        {
        case CHECKMATE:
            std::cout << "player checkmate" << std::endl;
            break;
        case STALEMATE:
            std::cout << "player stalemate" << std::endl;
            break;
        default:
            break;
        }
    }

    SDL_Event e;

    if (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
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
			//IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA ||
			//if (!iavsia) {
				handleMouseDown(e);
			//}

        }
        else if (e.type == SDL_MOUSEMOTION)
        {
			//IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA ||
			//if (!iavsia) {
            handleMouseMotion(e);
			//}
        }
        else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
        {
			//IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA || //IA vs IA ||
			//if (!iavsia) {
				if (promotePieceIndex != -1)
				{
					handlePromoteMove();
				}

				else if (pieceMovingInfo.pieceMoving != NULL)
				{
					movePiece(e);
				}
			//}
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
        case SDLK_s: // s to change side || s to change side || s to change side || s to change side || s to change side || s to change side || s to change side || s to change side || s to change side || 
        {
			//s for switch side
            switchSide();
        }
        case SDLK_p:
        {
            //active stockfish

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
        std::reverse(tiles.begin(), tiles.end());
        AI = WHITE;
    }
    else
    {
        std::reverse(tiles.begin(), tiles.end());
        AI = BLACK;
    }
    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].initTexture(tileSurface);
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
					//printf("move\n");
					updatePieceLocation(move, i);
					//two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode || two player mode ||
					/*if(twoplayer){
					switchSide();
					}*/


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

                    hasVerify_ia_canMove = false;
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

        hasVerify_ia_canMove = false;
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
	//if(twoplayer){
	//  switchSide();
	//}

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
    switch (checkMate(game.getHistoPos_stockfish()))
    {
    case CHECKMATE:
        std::cout << "ia checkmate" << std::endl;
        break;
    case STALEMATE:
        std::cout << "ia stalemate" << std::endl;
        break;
    default:
        break;
    }

    Move AImove = 0;

    switch (aiChoice)
    {
    case defs::STOCKFISH:

        stockfishMove(&AImove);

        std::cout << game.getHistoPos_stockfish() << std::endl;

        break;
    case defs::LAWRENCE:

        lawrenceMove(&AImove);
        break;
    default:

        lawrenceMove(&AImove);
        break;
    }

    if (iaCantPlay) // IA have lost
    {
        std::cout << "AI checkMate" << std::endl;
    }
    
    if (AImove == 0) {
        std::cout << "error ai move = 0" << std::endl;
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

        hasVerify_player_canMove = false;
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
        game.addToHistoPos_stockfish(str_move);

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
            }
            else {
                if (promote == "q")
                    piece = bQ;
                if (promote == "n")
                    piece = bN;
            }
            addPromoteBits(*AImove, piece);
        }
        
        *AImove = utils::parseMove(*AImove, game);
    }
    else
        iaCantPlay = true;
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

    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].render();
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
        const int size = SCREEN_SIZE / 8;

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

    SDL_FreeSurface(promoteSqrSurface);
    SDL_DestroyTexture(promoteTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}
