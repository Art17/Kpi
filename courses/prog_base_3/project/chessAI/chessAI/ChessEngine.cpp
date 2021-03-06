#include "ChessEngine.h"
#include <memory.h>

#define min(a,b) a < b ? a : b
#define max(a,b) a > b ? a : b

ChessEngine::ChessEngine() : colorWhite(64)
{
    memset (&board, 0, sizeof(byte)*8*8);
}

void ChessEngine::engineMove(Move* pRes)
{
}

bool ChessEngine::makeMove (const Move& move)
{
    if ( isValidMove(move) )
    {
        int from = move.from;
        int xFrom = from % 8;
        int yFrom = from / 8;

        int to = move.to;
        int xTo = to % 8;
        int yTo = to / 8;

        if ( board[yFrom][xFrom] & Rook && yFrom == 7 && xFrom == 0 )
            bWhiteLeftRookMoved = true;
        if ( board[yFrom][xFrom] & Rook && yFrom == 7 && xFrom == 7 )
            bWhiteRightRookMoved = true;
        if ( board[yFrom][xFrom] & King && isWhite (from) )
        {
            bWhiteKingMoved = true;
            whiteKingPos = to;
        }

        if ( board[yFrom][xFrom] & Rook && yFrom == 0 && xFrom == 0 )
            bBlackLeftRookMoved = true;
        if ( board[yFrom][xFrom] & Rook && yFrom == 0 && xFrom == 7 )
            bBlackRightRookMoved = true;
        if ( board[yFrom][xFrom] & King && !isWhite (from) )
        {
            bBlackKingMoved = true;
            blackKingPos = to;
        }

        board[yTo][xTo] = board[yFrom][xFrom];
        board[yFrom][xFrom] = 0;

        if (yTo == 0 && board[yTo][xTo] & Pawn )
            board[yTo][xTo] = move.extra | colorWhite;
        if (yTo == 7 && board[yTo][xTo] & Pawn)
            board[yTo][xTo] = move.extra;


        whiteTurn = !whiteTurn;

        lastMoves.push (move);

        return true;
    }
    return false;
}

void ChessEngine::newGame()
{
    memset (&board, 0, sizeof(byte)*8*8);
    reload ();

    board[0][0] = Rook;
    board[0][1] = Knight;
    board[0][2] = Bishop;
    board[0][3] = Queen;
    board[0][4] = King;
    board[0][5] = Bishop;
    board[0][6] = Knight;
    board[0][7] = Rook;

    for (int i = 0; i < 8; i++)
        board[1][i] = Pawn;

    board[7][0] = Rook | colorWhite;
    board[7][1] = Knight | colorWhite;
    board[7][2] = Bishop | colorWhite;
    board[7][3] = Queen | colorWhite;
    board[7][4] = King | colorWhite;
    board[7][5] = Bishop | colorWhite;
    board[7][6] = Knight | colorWhite;
    board[7][7] = Rook | colorWhite;

    for (int i = 0; i < 8; i++)
        board[6][i] = Pawn | colorWhite;
}

void ChessEngine::getBoard (byte* pBoard)
{
    memcpy (pBoard, &board, sizeof(byte)*8*8);
}

void ChessEngine::setBoard(byte* pBoard)
{
    reload ();

    memcpy (&board, pBoard, sizeof (byte)*8*8);
}

byte* ChessEngine::getValidMoves (int from, byte moves[], int* lPtr) const
{
    if (from < 0 || from > 63)
        return (byte*)moves;

    int xFrom = from % 8;
    int yFrom = from / 8;

    bool bWhite = isWhite(from);
    if (bWhite != whiteTurn)
        return (byte*)moves;

    if ( board[yFrom][xFrom] == 0 )
        return (byte*)moves;
    else if ( board[yFrom][xFrom] & Pawn )
        getPawnMoves(from, moves, lPtr);
    else if ( board[yFrom][xFrom] & Knight )
        getKnightMoves(from, moves, lPtr);
    else if ( board[yFrom][xFrom] & Bishop )
        getBishopMoves(from, moves, lPtr);
    else if ( board[yFrom][xFrom] & Rook )
        getRookMoves(from, moves, lPtr);
    else if ( board[yFrom][xFrom] & Queen )
        getQueenMoves(from, moves, lPtr);
    else if ( board[yFrom][xFrom] & King )
        getKingMoves(from, moves, lPtr);

    return (byte*)moves;
}

byte* ChessEngine::getRookMoves (int from, byte moves[], int* lPtr) const
{
    int c = 0;

    int yFrom = from / 8;
    int xFrom = from % 8;

    for (int i = xFrom + 1; i < 8; i++)
        {
            if (board[yFrom][i] == 0)
                moves[c++] = yFrom*8 + i;
            else if (isDifferentColor (board[yFrom][xFrom], board[yFrom][i]) )
            {
                moves[c++] = cti(i, yFrom);
                break;
            }
            else
                break;

        }
        for (int i = xFrom - 1; i >= 0; i--)
        {
            if (board[yFrom][i] == 0)
                moves[c++] = yFrom*8 + i;
            else if (isDifferentColor (board[yFrom][xFrom], board[yFrom][i]) )
            {
                moves[c++] = cti (i, yFrom);
                break;
            }
            else
                break;

        }
        for (int i = yFrom + 1; i < 8; i++)
        {
            if (board[i][xFrom] == 0)
                moves[c++] = i*8 + xFrom;
            else if (isDifferentColor (board[yFrom][xFrom], board[i][xFrom]) )
            {
                moves[c++] = cti (xFrom, i);
                break;
            }
            else
                break;

        }
        for (int i = yFrom - 1; i >= 0; i--)
        {
            if (board[i][xFrom] == 0)
                moves[c++] = i*8 + xFrom;
            else if (isDifferentColor (board[yFrom][xFrom], board[i][xFrom]) )
            {
                moves[c++] = cti (xFrom, i);
                break;
            }
            else
                break;

        }
    *lPtr = c;
    return (byte*)moves;
}
byte* ChessEngine::getKnightMoves (int from, byte moves[], int* lPtr) const
{
    int c = 0;
    *lPtr = 0;

    int yFrom = from / 8;
    int xFrom = from % 8;

    int x [] = {1, 2, -1, -2, 1, 2, -1, -2};
    int y [] = {2, 1, -2, -1, -2, -1, 2, 1};

    for (int i = 0; i < 8; i++)
    {
        if (xFrom + x[i] >= 0 && xFrom + x[i] < 8 && yFrom + y[i] >= 0 && yFrom + y[i] < 8)
        {
            if ( board[yFrom + y[i]][xFrom + x[i]] == 0 ||
                isDifferentColor(board[yFrom][xFrom], board[yFrom + y[i]][xFrom + x[i]]) )
                moves[c++] = (yFrom+y[i])*8 + xFrom + x[i];
        }
    }
    *lPtr = 0;
    return (byte*)moves;
}
byte* ChessEngine::getBishopMoves (int from, byte moves[], int* lPtr) const
{
    int c = 0;
    *lPtr = 0;

    int yFrom = from / 8;
    int xFrom = from % 8;

    for (int i = xFrom + 1; i < 8; i++)
            for (int j = yFrom + 1; j < 8; j++)
                {
                    if (board[j][i] == 0)
                        moves[c++] = cti (i, j);
                    else if (isDifferentColor (board[yFrom][xFrom], board[j][i]) )
                    {
                        moves[c++] = cti(i, j);
                        break;
                    }
                    else
                        break;

                }
        for (int i = xFrom + 1; i < 8; i++)
            for (int j = yFrom - 1; j >= 0; j--)
                {
                    if (board[j][i] == 0)
                        moves[c++] = cti (i, j);
                    else if (isDifferentColor (board[yFrom][xFrom], board[j][i]) )
                    {
                        moves[c++] = cti(i, j);
                        break;
                    }
                    else
                        break;

                }
        for (int i = xFrom - 1; i >= 0; i--)
            for (int j = yFrom + 1; j < 8; j++)
                {
                    if (board[j][i] == 0)
                        moves[c++] = cti (i, j);
                    else if (isDifferentColor (board[yFrom][xFrom], board[j][i]) )
                    {
                        moves[c++] = cti(i, j);
                        break;
                    }
                    else
                        break;

                }
        for (int i = xFrom - 1; i >= 0; i--)
            for (int j = yFrom - 1; j >= 0; j--)
                {
                    if (board[j][i] == 0)
                        moves[c++] = cti (i, j);
                    else if (isDifferentColor (board[yFrom][xFrom], board[j][i]) )
                    {
                        moves[c++] = cti(i, j);
                        break;
                    }
                    else
                        break;

                }
    *lPtr = c;
    return (byte*)moves;
}
byte* ChessEngine::getQueenMoves (int from, byte moves[], int* lPtr) const
{
    byte moves1[16];
    byte moves2[16];
    int l1 = 0;
    int l2 = 0;

    getRookMoves (from, moves1, &l1);
    getBishopMoves (from, moves2, &l2);

    int c = l1 + l2;

    for (int i = 0; i < l1; i++)
        moves[i] = moves1[i];
    for (int i = l1; i < l1 + l2; i++)
        moves[i] = moves2[i-l1];
    *lPtr = c;
}
byte* ChessEngine::getKingMoves (int from, byte moves[], int* lPtr) const
{
    int c = 0;
    int yFrom = from / 8;
    int xFrom = from % 8;

    int x[] = {xFrom + 1, xFrom + 1, xFrom,     xFrom - 1, xFrom - 1, xFrom - 1, xFrom,     xFrom + 1};
    int y[] = {yFrom,     yFrom - 1, yFrom - 1, yFrom - 1, yFrom,     yFrom + 1, yFrom + 1, yFrom + 1};

    for (int i = 0; i < 8; i++)
    {
        if (x[i] >= 0 && x[i] < 8 && y[i] >= 0 && y[i] < 8)
        {
            if ( board[ y[i] ][ x[i] ] == 0 || isDifferentColor ( board[yFrom][xFrom], board[ y[i] ][ x[i] ] ) )
            {
                if ( !isBeaten ( cti (x[i], y[i]), !isWhite (board[yFrom][xFrom] )) )
                    moves[c++] = cti (x[i], y[i]);
            }
        }
    }

    bool bWhite = isWhite(from);

    if (bWhite)
    {
        if ( !bWhiteKingMoved )
        {
            if ( !bWhiteLeftRookMoved )
            {
                bool canCastling = true;
                for (int i = 1; i < 4; i++)
                    if ( board[7][i] != 0 || isBeaten(cti(i, 7), false) )
                        canCastling = false;
                if ( isBeaten ( cti(4, 7), false ) )
                    canCastling = false;

                if (canCastling)
                {
                    moves[c++] = 58;
                }

            }
            if ( !bWhiteRightRookMoved )
            {
                bool canCastling = true;
                for (int i = 5; i < 7; i++)
                    if ( board[7][i] != 0 || isBeaten(cti(i, 7), false) )
                        canCastling = false;
                if ( isBeaten ( cti(4, 7), false ) )
                    canCastling = false;

                if (canCastling)
                {
                    moves[c++] = 62;
                }
            }
        }
    }
    else
    {
        if ( !bBlackKingMoved )
        {
            if ( !bBlackLeftRookMoved )
            {
                bool canCastling = true;
                for (int i = 1; i < 4; i++)
                    if ( board[0][i] != 0 || isBeaten(cti(i, 0), true) )
                        canCastling = false;
                if ( isBeaten ( cti(4, 0), true ) )
                    canCastling = false;

                if (canCastling)
                {
                    moves[c++] = 2;
                }

            }
            if ( !bBlackRightRookMoved )
            {
                bool canCastling = true;
                for (int i = 5; i < 7; i++)
                    if ( board[0][i] != 0 || isBeaten(cti(i, 7), true) )
                        canCastling = false;
                if ( isBeaten ( cti(4, 0), true ) )
                    canCastling = false;

                if (canCastling)
                {
                    moves[c++] = 6;
                }
            }
        }
    }

    *lPtr = c;
    return (byte*)moves;
}
byte* ChessEngine::getPawnMoves (int from, byte moves[], int* lPtr) const
{
    int c = 0;
    *lPtr = 0;
    int yFrom = from / 8;
    int xFrom = from % 8;

    Move lastMove = lastMoves.top ();
    int lastFrom = lastMove.from;
    int lastTo = lastMove.to;
    int lastXFrom = lastFrom % 8;
    int lastYFrom = lastFrom / 8;
    int lastXTo = lastTo % 8;
    int lastYTo = lastTo / 8;

    if (isWhite (from))
        {
            if (board[yFrom - 1][xFrom] == 0)
            {
                moves[c++] = cti (xFrom, yFrom-1);

                if ((from / 8) == 6 && board[yFrom - 2][xFrom] == 0 )
                    moves[c++] = (xFrom, yFrom-2);
            }
            if (xFrom+1 < 8 && !isWhite (board[yFrom-1][xFrom+1]))
            {
                moves[c++] = cti (xFrom, yFrom - 1);
            }
            if (xFrom - 1 >= 0 && !isWhite (board[yFrom-1][xFrom-1]) )
            {
                moves[c++] = cti (xFrom, yFrom - 1);
            }
            if ( yFrom == 3 )
            {
                if (xFrom+1 < 8 && lastYFrom == 1 && lastXFrom == xFrom+1 && lastYTo == 3 && lastXTo == xFrom+1)
                {
                    if ( board[3][xFrom+1] & Pawn )
                        moves[c++] = cti (xFrom+1, yFrom-1);
                }
                if (xFrom-1 >= 0 && lastYFrom == 1 && lastXFrom == xFrom-1 && lastYTo == 3 && lastXTo == xFrom-1)
                {
                    if ( board[3][xFrom-1] & Pawn )
                        moves[c++] = cti (xFrom-1, yFrom-1);
                }
            }
        }
        else
        {
            if (board[yFrom + 1][xFrom] == 0)
            {
                moves[c++] = cti (xFrom, yFrom+1);

                if ((from / 8) == 1 && board[yFrom + 2][xFrom] == 0 )
                    moves[c++] = cti (xFrom, yFrom+2);
            }
            if (xFrom+1 < 8 && !isWhite (board[yFrom+1][xFrom+1]))
            {
                moves[c++] = cti (xFrom, yFrom+1);
            }
            if (xFrom - 1 >= 0 && !isWhite (board[yFrom+1][xFrom-1]) )
            {
                moves[c++] = cti (xFrom, yFrom + 1);
            }
            if ( yFrom == 4 )
            {
                if (xFrom+1 < 8 && lastYFrom == 6 && lastXFrom == xFrom+1 && lastYTo == 4 && lastXTo == xFrom+1)
                {
                    if ( board[4][xFrom+1] & Pawn )
                        moves[c++] = cti (xFrom+1, yFrom+1);
                }
                if (xFrom-1 >= 0 && lastYFrom == 6 && lastXFrom == xFrom-1 && lastYTo == 4 && lastXTo == xFrom-1)
                {
                    if ( board[4][xFrom-1] & Pawn )
                        moves[c++] = cti (xFrom-1, yFrom+1);
                }
            }
        }
    *lPtr = c;
    return (byte*)moves;
}

inline bool ChessEngine::isDifferentColor(int x1, int x2) const
{
    return (((x1 & colorWhite) ^ (x2 & colorWhite)) == colorWhite);
}

inline bool ChessEngine::isWhite (int from) const
{
    return from & colorWhite;
}

bool ChessEngine::isBeaten (int from, bool bWhite) const
{
    int xFrom = from % 8;
    int yFrom = from / 8;

    /* Knight */
    int x [] = {1, 2, -1, -2, 1, 2, -1, -2};
    int y [] = {2, 1, -2, -1, -2, -1, 2, 1};

    for (int i = 0; i < 8; i++)
    {
        if (xFrom + x[i] >= 0 && xFrom + x[i] < 8 && yFrom + y[i] >= 0 && yFrom + y[i] < 8)
        {
            if ( board[yFrom + y[i]][xFrom + x[i]] & Knight && isWhite (board[yFrom + y[i]][xFrom + x[i]]) == bWhite )
                return true;
        }
    }
    /* ----- */

    /* Rook and Queen */
    for (int i = xFrom + 1; i < 8; i++)
        {
            if (board[yFrom][i] == 0)
                continue;
            else if (board[yFrom][i] & Rook || board[yFrom][i] & Queen)
            {
                if (isWhite (board[yFrom][i]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }
    for (int i = xFrom - 1; i >= 0; i--)
        {
            if (board[yFrom][i] == 0)
                continue;
            else if (board[yFrom][i] & Rook || board[yFrom][i] & Queen)
            {
                if (isWhite (board[yFrom][i]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }
    for (int i = yFrom + 1; i < 8; i++)
        {
            if (board[i][xFrom] == 0)
                continue;
            else if (board[i][xFrom] & Rook || board[i][xFrom] & Queen)
            {
                if (isWhite (board[i][xFrom]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }
    for (int i = yFrom - 1; i >= 0; i--)
        {
            if (board[i][xFrom] == 0)
                continue;
            else if (board[i][xFrom] & Rook || board[i][xFrom] & Queen)
            {
                if (isWhite (board[i][xFrom]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }

    /* ---- */

    /* Pawn */
    if (bWhite)
    {
        if ( yFrom+1 < 8 && xFrom+1 < 8 && board[yFrom+1][xFrom+1] & Pawn && bWhite == isWhite (board[yFrom+1][xFrom+1]))
            return true;
        if ( yFrom+1 < 8 && xFrom-1 >= 0 && board[yFrom+1][xFrom-1] & Pawn && bWhite == isWhite (board[yFrom+1][xFrom-1]) )
            return true;
    }
    else
    {
        if ( yFrom-1 >= 0 && xFrom+1 < 8 && board[yFrom-1][xFrom+1] & Pawn && bWhite == isWhite (board[yFrom-1][xFrom+1]))
            return true;
        if ( yFrom-1 >= 0 && xFrom-1 >= 0 && board[yFrom-1][xFrom-1] & Pawn && bWhite == isWhite (board[yFrom-1][xFrom-1]) )
            return true;
    }
    /* --- */

    /* King */
    {
        int x[] = {xFrom + 1, xFrom + 1, xFrom,     xFrom - 1, xFrom - 1, xFrom - 1, xFrom,     xFrom + 1};
        int y[] = {yFrom,     yFrom - 1, yFrom - 1, yFrom - 1, yFrom,     yFrom + 1, yFrom + 1, yFrom + 1};

        for (int i = 0; i < 8; i++)
        {
            if (x[i] >= 0 && x[i] < 8 && y[i] >= 0 && y[i] < 8 &&
                board[ y[i] ][ x[i] ] & King && bWhite == isWhite ( board[ y[i] ][ x[i] ] ) )
                return true;
        }
    }
    /* --- */

    /* Bishop and queen */
    for (int i = xFrom + 1; i < 8; i++)
        for (int j = yFrom + 1; j < 8; j++)
        {
            if (board[j][i] == 0)
                continue;
            else if (board[j][i] & Bishop || board[j][i] & Queen)
            {
                if (isWhite (board[j][i]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }
    for (int i = xFrom - 1; i >= 0; i--)
        for (int j = yFrom + 1; j < 8; j++)
        {
            if (board[j][i] == 0)
                continue;
            else if (board[j][i] & Bishop || board[j][i] & Queen)
            {
                if (isWhite (board[j][i]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }
    for (int i = xFrom + 1; i < 8; i++)
        for (int j = yFrom - 1; j >= 0; j--)
        {
            if (board[j][i] == 0)
                continue;
            else if (board[j][i] & Bishop || board[j][i] & Queen)
            {
                if (isWhite (board[j][i]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }
    for (int i = xFrom - 1; i >= 0; i--)
        for (int j = yFrom - 1; j >= 0; j--)
        {
            if (board[j][i] == 0)
                continue;
            else if (board[j][i] & Bishop || board[j][i] & Queen)
            {
                if (isWhite (board[j][i]) == bWhite)
                    return true;
                else
                    break;
            }
            else
                break;
        }
    /* --- */

    return false;
}

bool ChessEngine::isValidMove (const Move& move) const
{
    int from = move.from;
    int to = move.to;

    if (from < 0 || from > 63 || to < 0 || to > 63 )
        return false;
    byte moves[32];
    int length;
    getValidMoves(from, moves, &length);

    for (int i = 0; i < length; i++)
        if ( to == moves[i] )
            return true;
    return false;
}

int ChessEngine::cti (int x, int y) const
{
    return y*8 + x;
}

void ChessEngine::reload ()
{
    whiteTurn = true;
    bWhiteKingMoved = false, bWhiteLeftRookMoved = false, bWhiteRightRookMoved = false;
    bBlackKingMoved = false, bBlackLeftRookMoved = false, bBlackRightRookMoved = false;

    whiteKingPos = cti (4, 7);
    blackKingPos = cti (4, 0);
}
