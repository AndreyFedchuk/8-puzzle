// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#ifndef IALGORITHM_H
#define IALGORITHM_H

#include <QObject>
#include <QVector>
#include <QList>
#include <ctime>

class IAlgorithm : public QObject
{
    Q_OBJECT
public:
    IAlgorithm(const int limitNodes,
               const int limitTime, const size_t limitMemory,
               QObject * parent = nullptr);

    virtual bool solvePuzzle() = 0;
    virtual QList<QVector<int> > getSolution(int &moves) const = 0;

    int getTime() const;

public slots:
    void runSolve();
    void stopSolve();

signals:
    void finished(bool);
    void createdNodes(int);
    void nodesInMemory(int);

protected:
    enum class modeMove{up, down, right, left, size};

    int m_Time;
    bool m_Stop;
    int m_LimitNodes;
    int m_LimitTime;
    size_t m_LimitMemory;
    int m_CreatedNodes;
};

#endif // IALGORITHM_H
