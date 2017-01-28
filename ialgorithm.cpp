// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "ialgorithm.h"

IAlgorithm::IAlgorithm(const int limitNodes,
                       const int limitTime,
                       const size_t limitMemory,
                       QObject *parent) :
    QObject(parent),
    m_LimitNodes(limitNodes),
    m_LimitTime(limitTime),
    m_LimitMemory(limitMemory)
{
    m_Time = 0;
    m_CreatedNodes = 0;
    m_Stop = false;
}

void IAlgorithm::runSolve()
{
    bool bOk = solvePuzzle();
    emit finished(bOk);
}

void IAlgorithm::stopSolve()
{
    this->m_Stop = true;
}

int IAlgorithm::getTime() const
{
    return m_Time;
}
