//ChessBoard.h
#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <iostream>
#include <string>
#include <vector>

class Piece{
    public:
        int type; // 0 Pawn, 1 Rook, 2 Knight, 3 Bishop, 4 Queen, 5 King
        int team; // 0 Black, 1 White
        bool castleRight;
    private:
};

struct Coord{
    int x, y;
};

struct Move{
    Coord f, t; // from/to
};

class ChessBoard{
    public:
        // Player/AI variables
        int plCol, aiCol; // Player's/AI's color
        // Functions
        void showMenu();
        void startGame();
        void initBoard(const std::string &);
        void playGame(int);
    private:
        int cx, cy, mcx, mcy; // Player's cursor X and Y (normal and choosing move)
        std::vector<Move> plMoves;
        // Chess board variables
        Piece* Board[8][8];
        std::string pcNames[6], pcSymbs[6];
        int pcWts[6];
        bool checkMate;
        std::vector<std::vector<int>> pcBonuses;
        int aiLMx1, aiLMy1, aiLMx2, aiLMy2;
        // Design variables
        int bgBD2, fgBD2, bgBG1, bgBG2, tClrB, tClrW, bgMV;
        // Functions
        void addPiece(int, int, int, int);
        void movePiece(Piece* Brd[8][8], int, int, int, int);
        void plMove();
        void aiMove();
        int evaluateBrd(Piece* Brd[8][8], int);
        int negaMax(Piece* Brd[8][8], int, int, int, int);
        Move makeMove(int, int, int, int);
        int numMoves;
        std::vector<Move> genPieceMoves(Piece* Brd[8][8], int, int, int); // psuedo legal moves
        std::vector<Move> genPsuedoMoves(Piece* Brd[8][8], int, int = -1, int = -1); // psuedo legal moves
        std::vector<Move> genLegalMoves(Piece* Brd[8][8], int, int = -1, int = -1);
        bool endangersKing(Piece* Brd[8][8], int, int, int, int, int);
        bool endangersKing(Piece* Brd[8][8], int, Move);
        bool gameOver(Piece* Brd[8][8]);
        double getDist(int, int, int, int);
        double getDist(Move);
        void printBoard(bool = false);
        void printBrd(Piece* Brd[8][8]);
};

#endif
