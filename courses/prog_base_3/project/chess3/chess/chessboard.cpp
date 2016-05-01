#include "chessboard.h"
#include "resources.h"

#include <movethread.h>

#include <iostream>

using namespace std;


ChessBoard::ChessBoard(QWidget* Parent, const QPoint& Position, const QSize& Size) :
QSFMLCanvas(Parent, Position, Size)
{

}

void ChessBoard::OnInit()
{
    iSelectedFigure = -1;

    loadBoard ();
    loadFigures ();

    rs_Selected = RectangleShape ( Vector2f (boardTileWidth, boardTileHeight) );
    rs_Selected.setFillColor(Color (0, 255, 0, 127));

}


void ChessBoard::OnUpdate()
{
    this->clear(sf::Color(0, 0, 0));
    this->draw (s_Board);
    if (iSelectedFigure >= 0)
    {
        rs_Selected.setPosition(boardMarginLeft + boardTileWidth*iSelectedTileX + figureMarginX,
                                boardMarginTop + boardTileHeight*iSelectedTileY + figureMarginY);

        this->draw (rs_Selected);
    }
    for (int i = 0; i < 32; i++)
    {
        this->draw (s_Figures[i]);
    }


    /*for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
           if (indexTable[i][j] != -1)
               this->draw (s_Figures[indexTable[i][j]]);*/
}

void ChessBoard::mousePressEvent(QMouseEvent* mouseEvent)
{
    int x = mouseEvent->x();
    int y = mouseEvent->y();

    if (x <= boardMarginLeft || y <= boardMarginTop)
        return;
    if (x >= boardMarginLeft + boardWidth || y >= boardMarginTop + boardHeight)
        return;

    x -= boardMarginLeft + 5;
    y -= boardMarginTop;

    int tileX = x / boardTileWidth;
    int tileY = y / boardTileHeight;

    if (tileX == iSelectedTileX && tileY == iSelectedTileY)
    {
        iSelectedTileX = -1;
        iSelectedTileY = -1;
        iSelectedFigure = -1;

        return;
    }

    if (iSelectedFigure == -1)
    {
        iSelectedFigure = indexTable[tileX][tileY];
        iSelectedTileX = tileX;
        iSelectedTileY = tileY;
    }
    else
    {
        /*s_Figures[iSelectedFigure].setPosition ( Vector2f (boardMarginLeft + tileX*boardTileWidth + figureMarginX,
                                                           boardMarginTop + tileY*boardTileHeight + figureMarginY) );*/

        QRect rect;
        rect.setCoords(s_Figures[iSelectedFigure].getPosition().x,
                       s_Figures[iSelectedFigure].getPosition().y,
                       boardMarginLeft + tileX*boardTileWidth + figureMarginX,
                       boardMarginTop + tileY*boardTileHeight + figureMarginY);
        MoveThread* moveThread = new MoveThread(rect,
                                                &s_Figures[iSelectedFigure]);
        moveThread->start ();

        rect.setCoords(iSelectedTileX, iSelectedTileY, tileX, tileY);
        FigureMovedInfo fmi;
        memset (&fmi, 0, sizeof (FigureMovedInfo));
        fmi.s_Figure = &s_Figures[iSelectedFigure];
        fmi.figureIndex = iSelectedFigure;
        fmi.figure = figuresTable[iSelectedTileX][iSelectedTileY];
        fmi.figureMove = rect;


        int newX, newY;

        if (figuresTable[tileX][tileY] != 0)
        {
            if (figuresTable[tileX][tileY] < 10)
            {
                newX = 0;
                newY = (qWhiteDestroyed++)*figureHeight;
                //s_Figures[ indexTable[tileX][tileY] ].setPosition(0, (qWhiteDestroyed++)*figureHeight);
            }
            if (figuresTable[tileX][tileY] >= 10)
            {
                newX = boardMarginLeft + boardWidth + 5;
                newY = (qBlackDestroyed++)*figureHeight;
                //s_Figures[ indexTable[tileX][tileY] ].setPosition(boardMarginLeft + boardWidth + 5, (qBlackDestroyed++)*figureHeight);
            }

            rect.setCoords( s_Figures[ indexTable[tileX][tileY] ].getPosition().x,
                            s_Figures[ indexTable[tileX][tileY] ].getPosition().y,
                               newX,
                               newY);
            moveThread = new MoveThread(rect,
                                        &s_Figures[ indexTable[tileX][tileY] ]);
            moveThread->start ();
            rect.setCoords (tileX, tileY, -1, -1);
            fmi.s_ExtraFigure = &s_Figures[ indexTable[tileX][tileY] ];
            fmi.extraFigureIndex = indexTable[tileX][tileY];
            fmi.extraFigure = figuresTable [tileX][tileY];
            fmi.extraFigureMove = rect;
        }
        journal.push(fmi);

        figuresTable[tileX][tileY] = figuresTable[iSelectedTileX][iSelectedTileY];
        figuresTable[iSelectedTileX][iSelectedTileY] = 0;

        indexTable[iSelectedTileX][iSelectedTileY] = -1;
        indexTable[tileX][tileY] = iSelectedFigure;

        iSelectedFigure = -1;
    }

}

/*void ChessBoard::mouseMoveEvent (QMouseEvent* mouseEvent)
{
    int x = mouseEvent->x();
    int y = mouseEvent->y();

    if (x <= boardMarginLeft || y <= boardMarginTop)
        return;
    if (x >= boardMarginLeft + boardWidth || y >= boardMarginTop + boardHeight)
        return;
    if (iSelectedFigure == -1)
        return;


    if (mouseEvent->buttons() & Qt::LeftButton)
    {
        s_Figures[iSelectedFigure].setPosition ( Vector2f (x - figureWidth/2,
                                                           y - figureHeight/2) );
    }
}

void ChessBoard::mouseReleaseEvent(QMouseEvent* mouseEvent)
{

    int x = mouseEvent->x();
    int y = mouseEvent->y();

    if (iSelectedFigure == -1)
        return;
    if (x <= boardMarginLeft || y <= boardMarginTop)
        return;
    if (x >= boardMarginLeft + boardWidth || y >= boardMarginTop + boardHeight)
        return;

    x -= boardMarginLeft + 5;
    y -= boardMarginTop;

    int tileX = x / boardTileWidth;
    int tileY = y / boardTileHeight;


    //if (mouseEvent->buttons() & Qt::LeftButton)
    //{
        s_Figures[iSelectedFigure].setPosition ( Vector2f (boardMarginLeft + tileX*boardTileWidth + figureMarginX,
                                                           boardMarginTop + tileY*boardTileHeight + figureMarginY) );
        indexTable[iSelectedTileX][iSelectedTileY] = -1;
        indexTable[tileX][tileY] = iSelectedFigure;
        iSelectedFigure = -1;
    //}
}*/

void ChessBoard::loadBoard ()
{
    qWhiteDestroyed = 0;
    qBlackDestroyed = 0;

    memset (figuresTable, 0, sizeof(int)*64);

    int color = 10;
    figuresTable[0][0] = Rook + color;
    figuresTable[1][0] = Knight + color;
    figuresTable[2][0] = Bishop + color;
    figuresTable[3][0] = Queen + color;
    figuresTable[4][0] = King + color;
    figuresTable[5][0] = Bishop + color;
    figuresTable[6][0] = Knight + color;
    figuresTable[7][0] = Rook + color;

    for (int i = 0; i < 8; i++)
        figuresTable[i][1] = Pawn + color;

    color = 0;

    for (int i = 0; i < 8; i++)
        figuresTable[i][6] = Pawn + color;

    figuresTable[0][7] = Rook + color;
    figuresTable[1][7] = Knight + color;
    figuresTable[2][7] = Bishop + color;
    figuresTable[3][7] = Queen + color;
    figuresTable[4][7] = King + color;
    figuresTable[5][7] = Bishop + color;
    figuresTable[6][7] = Knight + color;
    figuresTable[7][7] = Rook + color;

    /*for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            cout << figuresTable[i][j] << " ";
        }
        cout << endl;
    }*/




    t_Board.loadFromFile(szBoard);
    s_Board.setTexture(t_Board);

    int widgetWidth = this->getSize().x;
    int widgetheight = this->getSize().y;

    int spriteWidth = s_Board.getLocalBounds().width;
    int spriteHeight = s_Board.getLocalBounds().height;

    s_Board.setScale (Vector2f (
                                     ((float)widgetWidth) / (spriteWidth),
                                     ((float)widgetheight) / spriteHeight));

    boardMarginLeft = 58;
    boardMarginRight = 64;
    boardMarginTop = 59;
    boardMarginBottom  = 63;

    boardWidth = (spriteWidth - boardMarginLeft - boardMarginRight)*s_Board.getScale().x;
    boardHeight = (spriteHeight - boardMarginBottom - boardMarginTop)*s_Board.getScale().y;

    boardTileWidth = boardWidth / 8;
    boardTileHeight = boardHeight / 8;
}

void ChessBoard::loadFigures ()
{
    t_Figures.loadFromFile (szFigures);
    t_Figures.setSmooth(true);
    int figuresWidth = t_Figures.getSize().x;
    int figuresHeight = t_Figures.getSize().y;

    const int qFiguresPerWidth = 6;
    const int qFiguresPerHeight = 2;
    const int imageSequece[] = {King, Queen, Bishop, Knight, Rook, Pawn};

    figureWidth = figuresWidth / qFiguresPerWidth;
    figureHeight = figuresHeight / qFiguresPerHeight;

    figureMarginX = 7;
    figureMarginY = 6;

    int figurePos = 0;
    int currentSprite = 0;
    int qFigures = 32;


    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            indexTable[i][j] = -1;

        while (currentSprite < qFigures)
        {
            bool isBlack = figurePos / 6;
            if ( imageSequece[figurePos % qFiguresPerWidth] == King || imageSequece[figurePos % qFiguresPerWidth] == Queen)
            {
                s_Figures[currentSprite].setTexture (t_Figures);
                s_Figures[currentSprite].setTextureRect ( IntRect (
                                                    (figurePos % qFiguresPerWidth)*figuresWidth/qFiguresPerWidth,
                                                    (int)(figurePos / qFiguresPerWidth)*figuresHeight/qFiguresPerHeight,
                                                    figureWidth,
                                                    figureHeight ) );

                s_Figures[currentSprite].setScale (Vector2f (
                                              (float)(boardTileWidth) / figureWidth,
                                              (float)(boardTileHeight) / figureHeight ));

                if ( imageSequece[figurePos % qFiguresPerWidth] == King )
                {
                    if (isBlack)
                    {
                        s_Figures[currentSprite].setPosition ( Vector2f (
                                                                         boardMarginLeft + 4*boardTileWidth + figureMarginX,
                                                                         boardMarginTop  + figureMarginY) );
                        indexTable[4][0] = currentSprite;
                    }
                    else
                    {
                        s_Figures[currentSprite].setPosition ( Vector2f (
                                                                         boardMarginLeft + 4*boardTileWidth + figureMarginX,
                                                                         boardMarginTop + boardHeight - boardTileHeight + figureMarginY) );
                        indexTable[4][7] = currentSprite;
                    }
                }
                else if ( imageSequece[figurePos % qFiguresPerWidth] == Queen )
                {
                    if (isBlack)
                    {
                        s_Figures[currentSprite].setPosition ( Vector2f (
                                                                         boardMarginLeft + 3*boardTileWidth + figureMarginX,
                                                                         boardMarginTop + figureMarginY) );
                        indexTable[3][0] = currentSprite;
                    }
                    else
                    {
                        s_Figures[currentSprite].setPosition ( Vector2f (
                                                                         boardMarginLeft + 3*boardTileWidth + figureMarginX,
                                                                         boardMarginTop + boardHeight - boardTileHeight + figureMarginY) );
                        indexTable[3][7] = currentSprite;
                    }
                }

                currentSprite++;
            }
            else if ( imageSequece[figurePos % qFiguresPerWidth] == Pawn)
            {
                for (int i = 0; i < 8; i++)
                {
                    s_Figures[currentSprite].setTexture (t_Figures);
                    s_Figures[currentSprite].setTextureRect ( IntRect (
                                                    (figurePos % qFiguresPerWidth)*figuresWidth/qFiguresPerWidth,
                                                    (int)(figurePos / qFiguresPerWidth)*figuresHeight/qFiguresPerHeight,
                                                    figureWidth,
                                                    figureHeight ) );

                    s_Figures[currentSprite].setScale (Vector2f (
                                                  (float)(boardTileWidth) / figureWidth,
                                                  (float)(boardTileHeight) / figureHeight ));

                    if (isBlack)
                    {
                        s_Figures[currentSprite].setPosition ( Vector2f (
                                                                         boardMarginLeft + i*boardTileWidth  + figureMarginX,
                                                                         boardMarginTop + boardTileHeight + figureMarginY) );
                        indexTable[i][1] = currentSprite;
                    }
                    else
                    {
                        s_Figures[currentSprite].setPosition ( Vector2f (
                                                                         boardMarginLeft + i*boardTileWidth + figureMarginX,
                                                                         boardMarginTop + boardHeight - 2*boardTileHeight + figureMarginY) );
                        indexTable[i][6] = currentSprite;
                    }
                    currentSprite++;
                }
            }
            else
            {
                for (int i = 0; i < 2; i++)
                {
                    s_Figures[currentSprite].setTexture (t_Figures);
                    s_Figures[currentSprite].setTextureRect ( IntRect (
                                                    (figurePos % qFiguresPerWidth)*figuresWidth/qFiguresPerWidth,
                                                    ((int)(figurePos / qFiguresPerWidth))*figuresHeight/qFiguresPerHeight,
                                                    figureWidth,
                                                    figureHeight ) );
                    s_Figures[currentSprite].setScale (Vector2f (
                                                  (float)(boardTileWidth) / figureWidth,
                                                  (float)(boardTileHeight) / figureHeight ));

                    if ( imageSequece[figurePos % qFiguresPerWidth] == Rook )
                    {
                        if (isBlack)
                        {
                            s_Figures[currentSprite].setPosition ( Vector2f (
                                                                             boardMarginLeft + i*(boardWidth - boardTileWidth) + figureMarginX,
                                                                             boardMarginTop + figureMarginY) );
                            indexTable[i*7][0] = currentSprite;
                        }
                        else
                        {
                            s_Figures[currentSprite].setPosition ( Vector2f (
                                                                             boardMarginLeft + i*(boardWidth - boardTileWidth) + figureMarginX,
                                                                             boardMarginTop + boardHeight - boardTileHeight + figureMarginY) );
                            indexTable[i*7][7] = currentSprite;

                        }
                    }
                    else if ( imageSequece[figurePos % qFiguresPerWidth] == Knight )
                    {
                        if (isBlack)
                        {
                            s_Figures[currentSprite].setPosition ( Vector2f (
                                                                             boardMarginLeft + 1*boardTileWidth + 5*i*boardTileWidth + figureMarginX,
                                                                             boardMarginTop + figureMarginY) );
                            indexTable[1 + 5*i][0] = currentSprite;
                        }
                        else
                        {
                            s_Figures[currentSprite].setPosition ( Vector2f (
                                                                             boardMarginLeft + 1*boardTileWidth + 5*i*boardTileWidth + figureMarginX,
                                                                             boardMarginTop + boardHeight - boardTileHeight + figureMarginY) );
                            indexTable[1 + 5*i][7] = currentSprite;
                        }
                    }
                    else
                    {
                        if (isBlack)
                        {
                            s_Figures[currentSprite].setPosition ( Vector2f (
                                                                             boardMarginLeft + 2*boardTileWidth + 3*i*boardTileWidth + figureMarginX,
                                                                             boardMarginTop + figureMarginY) );
                            indexTable[2 + 3*i][0] = currentSprite;
                        }
                        else
                        {
                            s_Figures[currentSprite].setPosition ( Vector2f (
                                                                             boardMarginLeft + 2*boardTileWidth + 3*i*boardTileWidth + figureMarginX,
                                                                             boardMarginTop + boardHeight - boardTileHeight + figureMarginY) );
                            indexTable[2 + 3*i][7] = currentSprite;
                        }
                    }

                    currentSprite++;
                }
            }
            figurePos++;
        }
        figureWidth *= s_Figures[0].getScale().x;
        figureHeight *= s_Figures[0].getScale().y;
}

void ChessBoard::undo()
{
    if (journal.isEmpty())
        return;
    FigureMovedInfo fmi = journal.pop();
    int tileX1, tileY1, tileX2, tileY2;
    fmi.figureMove.getCoords(&tileX1, &tileY1, &tileX2, &tileY2);
    fmi.s_Figure->setPosition(Vector2f (boardMarginLeft + tileX1*boardTileWidth  + figureMarginX,
                                        boardMarginTop + tileY1*boardTileHeight + figureMarginY));

    indexTable[tileX2][tileY2] = -1;
    indexTable[tileX1][tileY1] = fmi.figureIndex;

    figuresTable[tileX2][tileY2] = 0;
    figuresTable[tileX1][tileY1] = fmi.figure;

    if ( fmi.extraFigureIndex != 0 )
    {
        cout << "WHAT THE FUCK" << endl;
        int tileX1, tileY1, tileX2, tileY2;
        fmi.extraFigureMove.getCoords(&tileX1, &tileY1, &tileX2, &tileY2);
        fmi.s_ExtraFigure->setPosition( Vector2f (
                                                  boardMarginLeft + tileX1*boardTileWidth + figureMarginX,
                                                  boardMarginTop + tileY1*boardTileHeight + figureMarginY)  );
        figuresTable[tileX1][tileY1] = fmi.extraFigure;
        indexTable[tileX1][tileY1] = fmi.extraFigureIndex;
    }

    iSelectedFigure = -1;
    iSelectedTileX = -1;
    iSelectedTileY = -1;
}
