// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "puzzle.h"

Puzzle::Puzzle(const QString sNum, int pointSize)
{
    m_font.setPointSize(pointSize);
    m_sNum = sNum;
    m_ID = m_sNum.toInt();
}

Puzzle::~Puzzle()
{

}

QRectF Puzzle::boundingRect() const
{
    return QRectF(-35,-35, 70, 70);
}

void Puzzle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();

    painter->setBrush(QBrush(Qt::green, Qt::Dense2Pattern));
    painter->setPen(QPen(Qt::darkBlue, 2));
    painter->drawRoundRect(-32, -32, 64, 64);

    painter->setFont(m_font);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_sNum);

    painter->restore();
}

int Puzzle::getID() const
{
    return m_ID;
}

void Puzzle::movePuzzle(QPointF point)
{
    int toX = x() - point.rx();
    int toY = y() - point.ry();

    if(toX != 0)
         if(toX > 0) //to left
         {
             for(int i(0); i < 35; i++)
             {
                setPos(mapToParent(-2, 0));
                sleep(20);
             }
         }
         else //to right
         {
             for(int i(0); i < 35; i++)
             {
                setPos(mapToParent(2, 0));
                sleep(20);
             }
         }
    else
        if(toY > 0) //to up
            for(int i(0); i < 35; i++)
            {
               setPos(mapToParent(0, -2));
               sleep(20);
            }
        else //to down
            for(int i(0); i < 35; i++)
            {
               setPos(mapToParent(0, 2));
               sleep(20);
            }
}

void Puzzle::sleep(int msec) const
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}
