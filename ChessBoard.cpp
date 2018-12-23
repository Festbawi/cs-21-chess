//ChessBoard.cpp
#include <cmath>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <windows.h>
#include "ChessBoard.h"
using namespace std;

int rndNum(int limit){ // Returns random number
    return rand()%limit;
}

void setCol(int bg = 0, int fg = 7){ // Sets colors
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bg << 4 | fg);
    // Credit: http://stackoverflow.com/users/3011009/mcleod-ideafix
}

void strLower(string* str){ // Converts capitals to small letters
    int i;
    for (i = 0; (unsigned)i < (*str).length(); i++)
        (*str).at(i) = tolower((*str).at(i));
}

void strUpper(string* str){ // Capitalizes small letters
    int i;
    for (i = 0; (unsigned)i < (*str).length(); i++)
        (*str).at(i) = toupper((*str).at(i));
}

void ChessBoard::showMenu(){
    int i, rnd;
    string introText = "\n\t  ***    *   *   ****     ***      ***  \n\t *   *   *   *   *       *   *    *   * \n\t*        *   *   *      *        *      \
        \n\t*        *   *   *       *        *     \n\t*        *****   ***      ***      ***  \n\t*        *   *   *           *        * \
        \n\t*        *   *   *            *        *\n\t *   *   *   *   *       *   *    *   * \n\t  ***    *   *   ***      ***      ***  ";
    string response;
    do{
        system("cls");
        for (i = 0; i < introText.length(); i++){
            if (introText[i] == '*'){
                rnd = rndNum(14)+1;
                setCol(rnd, rnd);
            }
            cout << introText[i];
            setCol();
        }
        setCol();
        cout << "\n  Menu\n";
        cout << "  [1] - Play\n";
        cout << "  [2] - Exit\n";
        cout << "\n  > ";
        cin >> response;
        strLower(&response);
        if (cin.fail() || (response != "1" && response != "2")){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "   Invalid input! Enter a valid choice. ";
            system("pause");
        } else {
            if (response == "1")
                startGame();
        }
    } while (response != "2");
    cout << "\n\n  Thanks for playing!\n";
}

void ChessBoard::startGame(){ // If the player chooses to play,
    int rnd;
    string response;
    cout << "\n  Which color would you like to be?\n";
    cout << "  [W] - White (First)\n";
    cout << "  [B] - Black (Second)\n";
    cout << "  [R] - Random\n";
    do{
        cin.clear();
        cout << "\n  > ";
        cin >> response;
        strUpper(&response);
        if (cin.fail() || (response != "W" && response != "B" && response != "R")){
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "   Invalid input! Enter a valid response.\n";
        }
    } while (cin.fail() || (response != "W" && response != "B" && response != "R"));
    // If the player chooses random
    if (response == "R"){
        rnd = rndNum(100);
        if (rnd > 50)
            response = "W";
        else
            response = "B";
    }
    cout << "  You are the " << (response == "W" ? "WHITE" : "BLACK") << " player. ";
    initBoard(response);
    system("cls");
    printBoard();
    playGame(1);
}

void ChessBoard::initBoard(const string &plSide){ // Initialize board
    int col, row;
    // Set board colors
    bgBD2 = 0;
    fgBD2 = 7;
    bgBG1 = 8;
    bgBG2 = 7;
    tClrB = 0;
    tClrW = 15;
    bgMV  = 2;
    // Initialize player/AI variables
    plCol = (plSide == "W" ? 1 : 0);
    aiCol = 1-plCol;
    aiLMx1 = -1;
    aiLMy1 = -1;
    aiLMx2 = -1;
    aiLMy2 = -1;
    numMoves = 0;
    checkMate = false;
    // Initialize piece names and their weights
    pcNames[0] = "Pawn";
    pcNames[1] = "Rook";
    pcNames[2] = "Knight";
    pcNames[3] = "Bishop";
    pcNames[4] = "Queen";
    pcNames[5] = "King";
    pcSymbs[0] = "P";
    pcSymbs[1] = "R";
    pcSymbs[2] = "N";
    pcSymbs[3] = "B";
    pcSymbs[4] = "Q";
    pcSymbs[5] = "K";
    pcWts[0] = 100;
    pcWts[1] = 500;
    pcWts[2] = 320;
    pcWts[3] = 330;
    pcWts[4] = 900;
    pcWts[5] = 20000;
    plMoves.resize(0);
    // Initialize piece bonuses (piece-square tables)
    pcBonuses = {{  0,  0,  0,  0,  0,  0,  0,  0, // Pawn
                   50, 50, 50, 50, 50, 50, 50, 50,
                   10, 10, 20, 30, 30, 20, 10, 10,
                    5,  5, 10, 27, 27, 10,  5,  5,
                    0,  0,  0, 25, 25,  0,  0,  0,
                    5, -5,-10,  0,  0,-10, -5,  5,
                    5, 10, 10,-25,-25, 10, 10,  5,
                    0,  0,  0,  0,  0,  0,  0,  0}, // Rook
                 {  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0},
                { -50,-40,-30,-30,-30,-30,-40,-50, // Knight
                  -40,-20,  0,  0,  0,  0,-20,-40,
                  -30,  0, 10, 15, 15, 10,  0,-30,
                  -30,  5, 15, 20, 20, 15,  5,-30,
                  -30,  0, 15, 20, 20, 15,  0,-30,
                  -30,  5, 10, 15, 15, 10,  5,-30,
                  -40,-20,  0,  5,  5,  0,-20,-40,
                  -50,-40,-20,-30,-30,-20,-40,-50},
                { -20,-10,-10,-10,-10,-10,-10,-20, // Bishop
                  -10,  0,  0,  0,  0,  0,  0,-10,
                  -10,  0,  5, 10, 10,  5,  0,-10,
                  -10,  5,  5, 10, 10,  5,  5,-10,
                  -10,  0, 10, 10, 10, 10,  0,-10,
                  -10, 10, 10, 10, 10, 10, 10,-10,
                  -10,  5,  0,  0,  0,  0,  5,-10,
                  -20,-10,-40,-10,-10,-40,-10,-20},
                {   0,  0,  0,  0,  0,  0,  0,  0, // Queen
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0},
                { -30,-40,-40,-50,-50,-40,-40,-30, // King
                  -30,-40,-40,-50,-50,-40,-40,-30,
                  -30,-40,-40,-50,-50,-40,-40,-30,
                  -30,-40,-40,-50,-50,-40,-40,-30,
                  -20,-30,-30,-40,-40,-30,-30,-20,
                  -10,-20,-20,-20,-20,-20,-20,-10,
                   20, 20,  0,  0,  0,  0, 20, 20,
                   20, 30, 10,  0,  0, 10, 30, 20}};
    // Initialize the board as empty
    for (col = 0; col < 8; col++)
        for (row = 0; row < 8; row++)
            Board[col][row] = NULL;
    // Place the player's pieces excluding the pawns
    addPiece(1, plCol, 0, 7);
    addPiece(1, plCol, 7, 7);
    addPiece(2, plCol, 1, 7);
    addPiece(2, plCol, 6, 7);
    addPiece(3, plCol, 2, 7);
    addPiece(3, plCol, 5, 7);
    addPiece(4, plCol, 4, 7);
    addPiece(5, plCol, 3, 7);
    // Place the AI's pieces excluding the pawns
    addPiece(1, aiCol, 0, 0);
    addPiece(1, aiCol, 7, 0);
    addPiece(2, aiCol, 1, 0);
    addPiece(2, aiCol, 6, 0);
    addPiece(3, aiCol, 2, 0);
    addPiece(3, aiCol, 5, 0);
    addPiece(4, aiCol, 4, 0);
    addPiece(5, aiCol, 3, 0);
    // Add the pawns of both sides
    for (col = 0; col < 8; col++){
        addPiece(0, plCol, col, 6);
        addPiece(0, aiCol, col, 1);
    }
    // Move the cursor to the player's king
    cx = 3;
    cy = 7;
}

void ChessBoard::addPiece(int type, int team, int x, int y){
    Piece* newPiece = new Piece;
    newPiece->type = type;
    newPiece->team = team;
    if (type == 1 || type == 5)
        newPiece->castleRight = true;
    else
        newPiece->castleRight = false;
    Board[x][y] = newPiece;
}

void ChessBoard::playGame(int side){
    int col, row;
    bool plWin = false;
    if (gameOver(Board) || checkMate){ // If checkmate or stalemate,
        system("cls");
        printBoard();
        cout << "\n  Game over!\n";
        if (checkMate){
            cout << "\n\n  Checkmate! Computer has won! ";
        } else {
            for (row = 0; row < 8; row++)
                for (col = 0; col < 8; col++)
                    if (Board[col][row] != NULL && Board[col][row]->type == 5)
                        if (Board[col][row]->team == plCol)
                            plWin = true;
            if (plWin)
                cout << "\n\n  Player has won! ";
            else
                cout << "\n\n  Computer has won! ";
        }
        cout << "\n\n  Thank you for playing the game! ";
        system("pause");
    } else if (numMoves > 50){
        cout << "\n  Game over!\n";
        cout << "\n\n  Stalemate due to fifty-move rule!\n   (50 moves without any capture or pawn move)";
        cout << "\n\n  Thank you for playing the game! ";
        system("pause");
    } else {
        if (side == plCol){
            plMove();
        } else {
            cout << "\n  Computer is thinking..";
            aiMove();
        }
        playGame(1-side);
    }
}

void ChessBoard::movePiece(Piece* Brd[8][8], int pcX, int pcY, int toX, int toY){
    if (pcX < 0 || pcY < 0 || toX < 0 || toY < 0 || pcX > 7 || pcY > 7 || toX > 7 || toY > 7)
        return;
    Piece* currPiece = Brd[pcX][pcY];
    Piece* targPiece = Brd[toX][toY];
    if ((currPiece->type == 1 || currPiece->type == 5) && Brd == Board)
        currPiece->castleRight = false;
    if (currPiece->type == 0){
        if (Brd == Board){
            if (toY == 0 || toY == 7){
                currPiece->type = 4;
            }
        numMoves = 0;
        }
        Brd[pcX][pcY] = NULL;
        Brd[toX][toY] = currPiece;
    } else if (currPiece->type == 1 && targPiece != NULL && targPiece->type == 5 && targPiece->team == plCol){
        targPiece->castleRight = false;
        Brd[pcX][pcY] = NULL;
        Brd[toX][toY] = NULL;
        if (pcX == 0){
            Brd[2][pcY] = currPiece;
            Brd[1][pcY] = targPiece;
        } else if (pcX == 7){
            Brd[4][pcY] = currPiece;
            Brd[5][pcY] = targPiece;
        }
        if (Brd == Board)
            numMoves++;
    } else {
        if (Brd == Board)
            numMoves++;
        Brd[pcX][pcY] = NULL;
        Brd[toX][toY] = currPiece;
        if (targPiece != NULL && Brd == Board)
            numMoves = 0;
    }
}

void ChessBoard::plMove(){
    bool axisMvFound, attacked, castled = false, moveMade = false;
    double nrstDist;
    Piece* currPiece;
    Piece* targPiece;
    vector<Move>::iterator it, nrstIt;
    char key = 80, key2 = 80;
    do {
        if (key != 13 && key != -32){
            system("cls");
            printBoard();
            if (aiLMx1 != -1){
                cout << "\n  AI: ";
                setCol(0, 5);
                cout << char(aiLMx1+'A') << 8-aiLMy1;
                setCol();
                cout << " to ";
                setCol(0, 5);
                cout << char(aiLMx2+'A') << 8-aiLMy2;
                setCol();
                cout << ". ";
            }
            cout << "\n  It's your turn! Choose the piece you want to move with the\n  arrow keys then press enter. ";
        }
        key = _getch();
        if (key == 72 && cy > 0){ // Cursor moves up
            cy--;
        } else if (key == 75 && cx > 0){ // Left
            cx--;
        } else if (key == 77 && cx < 7){ // Right
            cx++;
        } else if (key == 80 && cy < 7){ // Down
            cy++;        } else if (key == 13){
            if (Board[cx][cy] != NULL){
                if (Board[cx][cy]->team == plCol){
                    plMoves = genLegalMoves(Board, plCol, cx, cy);
                    if (genLegalMoves(Board, plCol).size() == 0){
                        checkMate = true;
                        break;
                    }
                    if (plMoves.size() > 0){
                        nrstIt = plMoves.begin();
                        for (it = plMoves.begin(); it < plMoves.end(); it++)
                            if (getDist(cx, cy, (*it).t.x, (*it).t.y) < getDist(cx, cy, (*nrstIt).t.x, (*nrstIt).t.y))
                                nrstIt = it;
                        mcx = (*nrstIt).t.x;
                        mcy = (*nrstIt).t.y;
                        system("cls");
                        printBoard(true);
                        setCol();
                        cout << "\n  Choose a move. Selected move is in ";
                        setCol(0, 10);
                        cout << "bright green";
                        setCol();
                        cout << ". Press\n  backspace to cancel. ";
                        do {
                            axisMvFound = false;
                            nrstDist = getDist(mcx, mcy, (*nrstIt).t.x, (*nrstIt).t.y);
                            key2 = _getch();
                            if (key2 == 72){ // Up
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (mcx == (*it).t.x){
                                        axisMvFound = true;
                                        break;
                                    }
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (it != nrstIt && mcy > (*it).t.y && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                        nrstIt = it;
                                        nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                    }
                                if (axisMvFound){
                                    nrstDist += 1;
                                    for (it = plMoves.begin(); it < plMoves.end(); it++)
                                        if (it != nrstIt && mcx == (*it).t.x && mcy > (*it).t.y && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                            nrstIt = it;
                                            nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                        }
                                }
                            } else if (key2 == 75){ // Left
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (mcy == (*it).t.y){
                                        axisMvFound = true;
                                        break;
                                    }
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (it != nrstIt && mcx > (*it).t.x && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                        nrstIt = it;
                                        nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                    }
                                if (axisMvFound){
                                    nrstDist += 1;
                                    for (it = plMoves.begin(); it < plMoves.end(); it++)
                                        if (it != nrstIt && mcy == (*it).t.y && mcx > (*it).t.x && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                            nrstIt = it;
                                            nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                        }
                                }
                            } else if (key2 == 77){ // Right
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (mcy == (*it).t.y){
                                        axisMvFound = true;
                                        break;
                                    }
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (it != nrstIt && mcx < (*it).t.x && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                        nrstIt = it;
                                        nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                    }
                                if (axisMvFound){
                                    nrstDist += 1;
                                    for (it = plMoves.begin(); it < plMoves.end(); it++)
                                        if (it != nrstIt && mcy == (*it).t.y && mcx < (*it).t.x && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                            nrstIt = it;
                                            nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                        }
                                }
                            } else if (key2 == 80){ // Down
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (mcx == (*it).t.x){
                                        axisMvFound = true;
                                        break;
                                    }
                                for (it = plMoves.begin(); it < plMoves.end(); it++)
                                    if (it != nrstIt && mcy < (*it).t.y && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                        nrstIt = it;
                                        nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                    }
                                if (axisMvFound){
                                    nrstDist += 1;
                                    for (it = plMoves.begin(); it < plMoves.end(); it++)
                                        if (it != nrstIt && mcx == (*it).t.x && mcy < (*it).t.y && (nrstDist == 0 || getDist(mcx, mcy, (*it).t.x, (*it).t.y) < nrstDist)){
                                            nrstIt = it;
                                            nrstDist = getDist(mcx, mcy, (*it).t.x, (*it).t.y);
                                        }
                                }
                            }
                            if (key2 == 72 || key2 == 75 || key2 == 77 || key2 == 80){
                                mcx = (*nrstIt).t.x;
                                mcy = (*nrstIt).t.y;
                                system("cls");
                                printBoard(true);
                                setCol();
                                cout << "\n  Choose a move. Selected move is in ";
                                setCol(0, 10);
                                cout << "bright green";
                                setCol();
                                cout << ". Press\n  backspace to cancel. ";
                            }
                        } while (key2 != 13 && key2 != 8);
                        plMoves.clear();
                        if (key2 == 13){
                            attacked = false;
                            currPiece = Board[cx][cy];
                            targPiece = Board[mcx][mcy];
                            if (targPiece != NULL){
                                if (!castled && targPiece->type == 5 && currPiece->type == 1 && targPiece->team == plCol)
                                    castled = true;
                                else if (targPiece->team == aiCol)
                                    attacked = true;
                            }
                            movePiece(Board, cx, cy, mcx, mcy);
                            moveMade = true;
                            system("cls");
                            printBoard();
                            if (castled){
                                setCol(0, (plCol==1 ? tClrW : tClrB));
                                cout << "\n  Rook";
                                setCol(0, 13);
                                cout << " castled ";
                                setCol(0, (plCol==1 ? tClrW : tClrB));
                                cout << "King";
                                setCol();
                                cout << ".";
                                castled = false;
                            } else {
                                setCol();
                                cout << "\n  ";
                                setCol((plCol==1 ? 0 : 7), (plCol==1 ? tClrW : tClrB));
                                cout << pcNames[currPiece->type];
                                if (attacked){
                                    setCol(0, 12);
                                    cout << " attacked ";
                                    setCol((aiCol==1 ? 0 : 7), (aiCol==1 ? tClrW : tClrB));
                                    cout << pcNames[targPiece->type];
                                    setCol(0, 5);
                                    cout << " (" << char(cx+'A') << 8-cy << ")";
                                    setCol();
                                    cout << " from ";
                                    setCol(0, 5);
                                    cout << char(mcx+'A') << 8-mcy;
                                } else {
                                    setCol();
                                    cout << " moved from ";
                                    setCol(0, 5);
                                    cout << char(mcx+'A') << 8-mcy;
                                    setCol();
                                    cout << " to ";
                                    setCol(0, 5);
                                    cout << char(cx+'A') << 8-cy;
                                }
                                setCol();
                                cout << ". ";
                            }
                            cx = mcx;
                            cy = mcy;
                        }
                    } else {
                        setCol(0, 12);
                        cout << "That piece has no moves available! ";
                    }
                } else {
                    setCol(0, 12);
                    cout << "That piece is not yours! ";
                }
            } else {
                setCol(0, 12);
                cout << "There is no piece at that position! ";
            }
        }
    } while (key != 13 || !moveMade);
}

void ChessBoard::aiMove(){
    int bestMvVal = -999999, score, col, row;
    Piece* Brd[8][8];
    vector<Move> aiMoves = genLegalMoves(Board, aiCol);
    vector<Move>::iterator it;
    vector<Move>::iterator bestMv;
    for (it = aiMoves.begin(); it < aiMoves.end(); it++){
        for (row = 0; row < 8; row++)
            for (col = 0; col < 8; col++)
                Brd[col][row] = Board[col][row];
        movePiece(Brd, (*it).f.x, (*it).f.y, (*it).t.x, (*it).t.y);
        score = -negaMax(Brd, aiCol, 3, -999999, 999999);
        if (score > bestMvVal){
            bestMvVal = score;
            bestMv = it;
        }
    }
    movePiece(Board, (*bestMv).f.x, (*bestMv).f.y, (*bestMv).t.x, (*bestMv).t.y);
    aiLMx1 = (*bestMv).f.x;
    aiLMy1 = (*bestMv).f.y;
    aiLMx2 = (*bestMv).t.x;
    aiLMy2 = (*bestMv).t.y;
}

// AI Code

int ChessBoard::evaluateBrd(Piece* Brd[8][8], int side){ // Returns evaluative score of board
    int matScore = 0, col, row;
    // Check for pieces
    for (row = 0; row < 8; row++)
        for (col = 0; col < 8; col++)
            if (Brd[col][row] != NULL){
                matScore += pcWts[Brd[col][row]->type] * (Brd[col][row]->team == side ? 1 : -1);
                if (Brd[col][row]->type != 1 && Brd[col][row]->type != 4){
                    if (Brd[col][row]->team == plCol)
                        matScore -= pcBonuses[Brd[col][row]->type][row*8+col];
                    else
                        matScore -= pcBonuses[Brd[col][row]->type][(7-row)*8+(7-col)];
                }
            }
    return matScore;
}


int ChessBoard::negaMax(Piece* Brd2[8][8], int side, int depth, int alpha, int beta){
    int mx = -999999, score, col, row;
    Piece* Brd[8][8];
    // Checks if board is in game over state
    if (gameOver(Brd2) || depth == 0)
        return evaluateBrd(Brd2, side);
    vector<Move> brdMoves = genLegalMoves(Brd2, side);
    vector<Move>::iterator it;
    for (it = brdMoves.begin(); it < brdMoves.end(); it++){
        for (row = 0; row < 8; row++)
            for (col = 0; col < 8; col++)
                Brd[col][row] = Brd2[col][row];
        movePiece(Brd, (*it).f.x, (*it).f.y, (*it).t.x, (*it).t.y);
        score = negaMax(Brd, 1-side, depth-1, -beta, -alpha);
        if (score > mx)
            mx = score;
        if (score > alpha)
            alpha = score;
        if (alpha >= beta)
            return alpha;
    }
    return mx;
}

// AI Code ends

Move ChessBoard::makeMove(int x1, int y1, int x2, int y2){
    Move newMove;
    newMove.f.x = x1;
    newMove.f.y = y1;
    newMove.t.x = x2;
    newMove.t.y = y2;
    return newMove;
}

vector<Move> ChessBoard::genPieceMoves(Piece* Brd[8][8], int side, int x, int y){ // Generates moves per piece
    int col, row;
    vector<Move> pieceMoves;
    Piece* checkPiece = Brd[x][y];
    if (checkPiece != NULL && checkPiece->team == side){
        if (checkPiece->type == 0){ // Pawn
            if (side == plCol){
                if (y == 6 && Brd[x][y-1] == NULL && Brd[x][y-2] == NULL)
                    pieceMoves.push_back(makeMove(x, y, x, y-2));
                if (y > 0){
                    if (Brd[x][y-1] == NULL)
                        pieceMoves.push_back(makeMove(x, y, x, y-1));
                    if (x > 0 && Brd[x-1][y-1] != NULL && Brd[x-1][y-1]->team != side)
                        pieceMoves.push_back(makeMove(x, y, x-1, y-1));
                    if (x < 7 && Brd[x+1][y-1] != NULL && Brd[x+1][y-1]->team != side)
                        pieceMoves.push_back(makeMove(x, y, x+1, y-1));
                }
            } else {
                if (y == 1 && Brd[x][y+1] == NULL && Brd[x][y+2] == NULL)
                    pieceMoves.push_back(makeMove(x, y, x, y+2));
                if (y > 0){
                    if (Brd[x][y+1] == NULL)
                        pieceMoves.push_back(makeMove(x, y, x, y+1));
                    if (x > 0 && Brd[x-1][y+1] != NULL && Brd[x-1][y+1]->team != side)
                        pieceMoves.push_back(makeMove(x, y, x-1, y+1));
                    if (x < 7 && Brd[x+1][y+1] != NULL && Brd[x+1][y+1]->team != side)
                        pieceMoves.push_back(makeMove(x, y, x+1, y+1));
                }
            }
        } else if (checkPiece->type == 2){ // Knight
            if (y > 0){ // One space forward
                if (x > 1 && (Brd[x-2][y-1] == NULL || Brd[x-2][y-1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x-2, y-1));
                if (x < 6 && (Brd[x+2][y-1] == NULL || Brd[x+2][y-1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x+2, y-1));
            }
            if (y > 1){ // Two spaces forward
                if (x > 0 && (Brd[x-1][y-2] == NULL || Brd[x-1][y-2]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x-1, y-2));
                if (x < 7 && (Brd[x+1][y-2] == NULL || Brd[x+1][y-2]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x+1, y-2));
            }
            if (y < 7){ // One space backward
                if (x > 1 && (Brd[x-2][y+1] == NULL || Brd[x-2][y+1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x-2, y+1));
                if (x < 6 && (Brd[x+2][y+1] == NULL || Brd[x+2][y+1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x+2, y+1));
            }
            if (y < 6){ // Two spaces backward
                if (x > 0 && (Brd[x-1][y+2] == NULL || Brd[x-1][y+2]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x-1, y+2));
                if (x < 7 && (Brd[x+1][y+2] == NULL || Brd[x+1][y+2]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x+1, y+2));
            }
        } else if (checkPiece->type == 5){ // King
            if (y > 0){ //Upward
                if (Brd[x][y-1] == NULL || Brd[x][y-1]->team != side)
                    pieceMoves.push_back(makeMove(x, y, x, y-1));
                if (x > 0 && (Brd[x-1][y-1] == NULL || Brd[x-1][y-1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x-1, y-1));
                if (x < 7 && (Brd[x+1][y-1] == NULL || Brd[x+1][y-1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x+1, y-1));
            }
            if (y < 7){ // Downward
                if (Brd[x][y+1] == NULL || Brd[x][y+1]->team != side)
                    pieceMoves.push_back(makeMove(x, y, x, y+1));
                if (x > 0 && (Brd[x-1][y+1] == NULL || Brd[x-1][y+1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x-1, y+1));
                if (x < 7 && (Brd[x+1][y+1] == NULL || Brd[x+1][y+1]->team != side))
                    pieceMoves.push_back(makeMove(x, y, x+1, y+1));
            }
            if (x > 0 && (Brd[x-1][y] == NULL || Brd[x-1][y]->team != side)) // Left-ward
                pieceMoves.push_back(makeMove(x, y, x-1, y));
            if (x < 7 && (Brd[x+1][y] == NULL || Brd[x+1][y]->team != side)) // Right-ward
                pieceMoves.push_back(makeMove(x, y, x+1, y));
        }
        if (checkPiece->type == 1 || checkPiece->type == 4){ // Rook/Queen
            for (row = y-1; row >= 0; row--){ // Upward
                pieceMoves.push_back(makeMove(x, y, x, row));
                if (Brd[x][row] != NULL){
                    if (Brd[x][row]->team == side)
                        pieceMoves.pop_back();
                    break;
                }
            }
            for (row = y+1; row < 8; row++){ // Downward
                pieceMoves.push_back(makeMove(x, y, x, row));
                if (Brd[x][row] != NULL){
                    if (Brd[x][row]->team == side)
                        pieceMoves.pop_back();
                    break;
                }
            }
            for (col = x-1; col >= 0; col--){ // Left-ward
                pieceMoves.push_back(makeMove(x, y, col, y));
                if (Brd[col][y] != NULL){
                    if (Brd[col][y]->team == side && (checkPiece->type == 4 || checkPiece->castleRight == false || Brd[col][y]->castleRight == false || Brd[col][y]->type != 5))
                        pieceMoves.pop_back();
                    break;
                }
            }
            for (col = x+1; col < 8; col++){ // Right-ward
                pieceMoves.push_back(makeMove(x, y, col, y));
                if (Brd[col][y] != NULL){
                    if (Brd[col][y]->team == side && (checkPiece->type == 4 || checkPiece->castleRight == false || Brd[col][y]->castleRight == false || Brd[col][y]->type != 5))
                        pieceMoves.pop_back();
                    break;
                }
            }
        }
        if (checkPiece->type == 3 || checkPiece->type == 4){ // Bishop/Queen
            for (col = x-1, row = y-1; col >= 0 && row >= 0; col--, row--){ // Up-left Diagonal
                pieceMoves.push_back(makeMove(x, y, col, row));
                if (Brd[col][row] != NULL){
                    if (Brd[col][row]->team == side)
                        pieceMoves.pop_back();
                    break;
                }
            }
            for (col = x+1, row = y-1; col < 8 && row >= 0; col++, row--){ // Up-Right Diagonal
                pieceMoves.push_back(makeMove(x, y, col, row));
                if (Brd[col][row] != NULL){
                    if (Brd[col][row]->team == side)
                        pieceMoves.pop_back();
                    break;
                }
            }
            for (col = x-1, row = y+1; col >= 0 && row < 8; col--, row++){ // Down-Left Diagonal
                pieceMoves.push_back(makeMove(x, y, col, row));
                if (Brd[col][row] != NULL){
                    if (Brd[col][row]->team == side)
                        pieceMoves.pop_back();
                    break;
                }
            }
            for (col = x+1, row = y+1; col < 8 && row < 8; col++, row++){ // Down-Right Diagonal
                pieceMoves.push_back(makeMove(x, y, col, row));
                if (Brd[col][row] != NULL){
                    if (Brd[col][row]->team == side)
                        pieceMoves.pop_back();
                    break;
                }
            }
        }
    }
    return pieceMoves;
}

vector<Move> ChessBoard::genPsuedoMoves(Piece* Brd[8][8], int side, int sx, int sy){ // Available moves
    int row, col;
    vector<Move> psuedoMoves;
    vector<Move> pieceMoves;
    vector<Move>::iterator it;
    if (sx != -1){
        psuedoMoves = genPieceMoves(Brd, side, sx, sy);
    } else {
        for (row = 7; row >= 0; row--){
            for (col = 0; col < 8; col++){
                pieceMoves = genPieceMoves(Brd, side, col, row);
                for (it = pieceMoves.begin(); it < pieceMoves.end(); it++)
                    psuedoMoves.push_back(*it);
            }
        }
    }
    return psuedoMoves;
}

/*
  Generates legal moves
  Prevents endangering of the side's king
  Input: Board, side to check, specific coordinate to check
  Output: Vector of moves
*/
vector<Move> ChessBoard::genLegalMoves(Piece* Brd[8][8], int side, int sx, int sy){
    vector<Move> legalMoves = genPsuedoMoves(Brd, side, sx, sy);
    vector<Move>::iterator it;
    // Iterates through all the possible moves and checks if the side's king is in danger
    for (it = legalMoves.begin(); it < legalMoves.end();)
        if (endangersKing(Brd, side, *it))
            legalMoves.erase(it);
        else
            it++;
    return legalMoves;
}

bool ChessBoard::endangersKing(Piece* Brd2[8][8], int side, int x, int y, int x2, int y2){ // Checks if the player's move will endanger the player's king
    int col, row;
    int kingX = -1, kingY; // King coordinates
    Piece* Brd[8][8];
    Piece* currPiece;
    // Check if move will win the game
    if (Brd2[x2][y2] != NULL && Brd2[x2][y2]->type == 5 && Brd2[x2][y2]->team != side)
        return false;
    // Copy board
    for (row = 0; row < 8; row++)
        for (col = 0; col < 8; col++)
            Brd[col][row] = Brd2[col][row];
    // Perform move
    movePiece(Brd, x, y, x2, y2);
    // Find king and enemies
    for (row = 0; row < 8; row++){
        for (col = 0; col < 8; col++){
            currPiece = Brd[col][row];
            if (currPiece != NULL && currPiece->team == side && currPiece->type == 5){
                kingX = col;
                kingY = row;
                break;
            }
        }
        if (kingX != -1)
            break;
    }
    if (kingX == -1)
        return true;
    // Check dangers
    if (side == plCol && kingY != 0){
        if (kingX != 7 && Brd[kingX+1][kingY-1] != NULL && Brd[kingX+1][kingY-1]->type == 0 && Brd[kingX+1][kingY-1]->team != side)
            return true;
        if (kingX != 0 && Brd[kingX-1][kingY-1] != NULL && Brd[kingX-1][kingY-1]->type == 0 && Brd[kingX-1][kingY-1]->team != side)
            return true;
    } else if (side != aiCol && kingY != 7){
        if (kingX != 7 && Brd[kingX+1][kingY+1] != NULL && Brd[kingX+1][kingY+1]->type == 0 && Brd[kingX+1][kingY+1]->team != side)
            return true;
        if (kingX != 0 && Brd[kingX-1][kingY+1] != NULL && Brd[kingX-1][kingY+1]->type == 0 && Brd[kingX-1][kingY+1]->team != side)
            return true;
    }
    // Rook/Queen
    for (row = kingY-1; row >= 0; row--) // Upwards
        if (Brd[kingX][row] != NULL){
            if ((Brd[kingX][row]->type == 1 || Brd[kingX][row]->type == 4) && Brd[kingX][row]->team != side)
                return true;
            break;
        }
    for (row = kingY+1; row < 8; row++) // Downwards
        if (Brd[kingX][row] != NULL){
            if ((Brd[kingX][row]->type == 1 || Brd[kingX][row]->type == 4) && Brd[kingX][row]->team != side)
                return true;
            break;
        }
    for (col = kingX-1; col >= 0; col--) // Left-ward
        if (Brd[col][kingY] != NULL){
            if ((Brd[col][kingY]->type == 1 || Brd[col][kingY]->type == 4) && Brd[col][kingY]->team != side)
                return true;
            break;
        }
    for (col = kingX+1; col < 8; col++) // Right-ward
        if (Brd[col][kingY] != NULL){
            if ((Brd[col][kingY]->type == 1 || Brd[col][kingY]->type == 4) && Brd[col][kingY]->team != side)
                return true;
            break;
        }
    // Knight
    if (kingY > 1){
        if (kingX < 7 && Brd[kingX+1][kingY-2] != NULL && Brd[kingX+1][kingY-2]->type == 2 && Brd[kingX+1][kingY-2]->team != side)
            return true;
        if (kingX > 0 && Brd[kingX-1][kingY-2] != NULL && Brd[kingX-1][kingY-2]->type == 2 && Brd[kingX-1][kingY-2]->team != side)
            return true;
    }
    if (kingY < 6){
        if (kingX < 7 && Brd[kingX+1][kingY+2] != NULL && Brd[kingX+1][kingY+2]->type == 2 && Brd[kingX+1][kingY+2]->team != side)
            return true;
        if (kingX > 0 && Brd[kingX-1][kingY+2] != NULL && Brd[kingX-1][kingY+2]->type == 2 && Brd[kingX-1][kingY+2]->team != side)
            return true;
    }
    if (kingY > 0){
        if (kingX < 6 && Brd[kingX+2][kingY-1] != NULL && Brd[kingX+2][kingY-1]->type == 2 && Brd[kingX+2][kingY-1]->team != side)
            return true;
        if (kingX > 1 && Brd[kingX-2][kingY-1] != NULL && Brd[kingX-2][kingY-1]->type == 2 && Brd[kingX-2][kingY-1]->team != side)
            return true;
    }
    if (kingY < 7){
        if (kingX < 6 && Brd[kingX+2][kingY+1] != NULL && Brd[kingX+2][kingY+1]->type == 2 && Brd[kingX+2][kingY+1]->team != side)
            return true;
        if (kingX > 1 && Brd[kingX-2][kingY+1] != NULL && Brd[kingX-2][kingY+1]->type == 2 && Brd[kingX-2][kingY+1]->team != side)
            return true;
    }
    // Bishop/Queen
    for (col = kingX-1, row = kingY-1; col >= 0 && row >= 0; col--, row--) // Up-left diagonal
        if (Brd[col][row] != NULL){
            if ((Brd[col][row]->type == 3 || Brd[col][row]->type == 4) && Brd[col][row]->team != side)
                return true;
            break;
        }
    for (col = kingX+1, row = kingY-1; col < 8 && row >= 0; col++, row--) // Up-right diagonal
        if (Brd[col][row] != NULL){
            if ((Brd[col][row]->type == 3 || Brd[col][row]->type == 4) && Brd[col][row]->team != side)
                return true;
            break;
        }
    for (col = kingX-1, row = kingY+1; col >= 0 && row < 8; col--, row++) // Down-left diagonal
        if (Brd[col][row] != NULL){
            if ((Brd[col][row]->type == 3 || Brd[col][row]->type == 4) && Brd[col][row]->team != side)
                return true;
            break;
        }
    for (col = kingX+1, row = kingY+1; col < 8 && row < 8; col++, row++) // Down-right diagonal
        if (Brd[col][row] != NULL){
            if ((Brd[col][row]->type == 3 || Brd[col][row]->type == 4) && Brd[col][row]->team != side)
                return true;
            break;
        }
    return false;
}

bool ChessBoard::endangersKing(Piece* Brd[8][8], int side, Move mv){
    return endangersKing(Brd, side, mv.f.x, mv.f.y, mv.t.x, mv.t.y);
}

bool ChessBoard::gameOver(Piece* Brd[8][8]){ // Checks if checkmate or stalemate
    int kingsFound = 0, col, row;
    for (row = 0; row < 8; row++)
        for (col = 0; col < 8; col++)
            if (Brd[col][row] != NULL && Brd[col][row]->type == 5)
                kingsFound++;
    return (kingsFound != 2);
}

double ChessBoard::getDist(int x1, int y1, int x2, int y2){
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

double ChessBoard::getDist(Move mv){
    return getDist(mv.f.x, mv.f.y, mv.t.x, mv.t.y);
}

void ChessBoard::printBoard(bool move){
    bool selected = false;
    int i, row, col, bgClr;
    Piece* currPiece;
    vector<Move>::iterator it;
    // Print top border
    setCol(0, 7);
    cout << "\n  ";
    setCol(bgBD2, fgBD2);
    cout << "   ";
    for (i = 'A'; i <= 'H'; i++)
        cout << " " << char(i) << " ";
    cout << "   \n";
    // Print entire board
    for (row = 0; row < 8; row++){
        setCol(0, 7);
        cout << "  ";
        setCol(bgBD2, fgBD2);
        cout << " " << 8-row << " ";
        for (col = 0; col < 8; col++){
            if (!move)
                selected = (row == cy && col == cx);
            else
                selected = (row == mcy && col == mcx);
            bgClr = (selected ? 15 : ((col+row+(1-plCol))%2==0 ? bgBG1 : bgBG2));
            currPiece = Board[col][row];
            for (it = plMoves.begin(); it < plMoves.end(); it++){
                if ((*it).t.y == row && (*it).t.x == col){
                    if (currPiece != NULL){
                        if (currPiece->team == plCol)
                            bgClr = 5;
                        else
                            bgClr = 4;
                        if (mcy == row && mcx == col){
                            if (currPiece->team == plCol)
                                bgClr = 13;
                            else
                                bgClr = 12;
                        }
                    } else {
                        bgClr = 2;
                        if (mcy == row && mcx == col)
                            bgClr = 10;
                    }
                    break;
                }
            }
            setCol(bgClr, 7);
            if (currPiece != NULL){
                setCol(bgClr, (selected ? (currPiece->team == plCol ? 8 : aiCol) : (currPiece->team == 0) ? tClrB : tClrW));
                cout << " " << pcSymbs[currPiece->type] << " ";
            } else {
                cout << "   ";
            }
        }
        setCol(bgBD2, fgBD2);
        cout << " " << 8-row << " ";
        setCol(0, 15);
        cout << "\n";
    }
    // Print bottom border
    setCol(0, 7);
    cout << "  ";
    setCol(bgBD2, fgBD2);
    cout << "   ";
    for (i = 'A'; i <= 'H'; i++)
        cout << " " << char(i) << " ";
    cout << "   \n";
    // Revert colors
    setCol();
}
