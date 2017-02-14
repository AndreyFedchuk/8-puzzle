// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#ifndef PUZZLE_H
#define PUZZLE_H

#include <QGraphicsObject>
#include <QFont>
#include <QPainter>
#include <QString>
#include <QTimer>
#include <QEventLoop>

class Puzzle : public QGraphicsObject
{
    Q_OBJECT
public:
    Puzzle(const QString sNum, int pointSize = 48);
    ~Puzzle();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int getID() const;
    void movePuzzle(QPointF point);

private:
    QFont m_font;
    QString m_sNum;
    int m_ID;

    void sleep(int msec) const;
};

#endif // PUZZLE_H
