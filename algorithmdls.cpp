// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "algorithmdls.h"

AlgorithmDLS::AlgorithmDLS(const QVector<int> &State,
                           const int limitNodes,
                           const int limitTime,
                           const size_t limitMemory,
                           const modeHeuristic mode,
                           const QVector<int> FinalState,
                           IAlgorithm *parent) :
    IAlgorithm(limitNodes, limitTime, limitMemory, mode, parent)
{
    m_StartState = State;
    m_GoalState = FinalState;
    m_shRoot = nullptr;
    m_Success = false;
    m_DepthLimit = 26;

    m_SizeOfNode = sizeof(int) * 11
            + sizeof(void*) * 3
            + sizeof(QSharedPointer<Node>) * 4
            + sizeof(QWeakPointer<Node>);        // incorrect!!!
}


bool AlgorithmDLS::solvePuzzle()
{
    makeRoot();

    m_StackOpen.push(m_shRoot);
    QSharedPointer<Node> shCurNode;

    time_t finish, start = clock();

    while(!m_StackOpen.isEmpty())
    {
        shCurNode = m_StackOpen.pop();
        m_ListClose.push_back(shCurNode);

        if(isSuccess(shCurNode)
                || checkLimits(start))
            break;

        emit nodesInMemory(m_StackOpen.size());
        emit createdNodes(m_CreatedNodes);

        if(shCurNode->depth >= m_DepthLimit)
            continue;

        makeStates(shCurNode);
    }
    finish = clock();
    m_Time = finish - start;

    return m_Success;
}

QList<QVector<int> > AlgorithmDLS::getSolution(int &moves) const
{
    moves = m_ListResult.size() - 1;
    return m_ListResult;
}


void AlgorithmDLS::makeRoot()
{
    if(!m_StackOpen.isEmpty())
        m_StackOpen.clear();
    if(!m_ListClose.isEmpty())
        m_ListClose.clear();

    m_shRoot = QSharedPointer<Node>::create();
    m_shRoot->depth = 0;
    m_shRoot->nodeState = m_StartState;
    m_shRoot->indexEmpty = m_shRoot->nodeState.indexOf(0);
}


bool AlgorithmDLS::isSuccess(const QSharedPointer<Node> &shCurNode)
{

    if(shCurNode->nodeState == m_GoalState)
    {
        m_Success = true;
        buildResult(shCurNode);
    }

    return m_Success;
}


void AlgorithmDLS::buildResult(const QSharedPointer<Node> &shCurNode)
{
    m_ListResult.clear();

    auto shTmp = shCurNode;
    while(!shTmp.isNull())
    {
        m_ListResult.push_front(shTmp->nodeState);
        shTmp = shTmp->shParent;
    }
}


void AlgorithmDLS::makeStates(QSharedPointer<Node> &shCurNode)
{
    for(int i(0); i < shCurNode->Childs.size(); i++)
    {
        shCurNode->Childs[i] = QSharedPointer<Node>::create();
        if(makeMove(shCurNode, shCurNode->Childs[i], static_cast<modeMove>(i))
                && checkNewNode(shCurNode->Childs[i]))
        {
            m_StackOpen.push(shCurNode->Childs[i]);
            m_CreatedNodes++;
        }
        else
        {
            shCurNode->Childs[i].reset();
        }
    }
}

bool AlgorithmDLS::makeMove(QSharedPointer<Node> &shCurNode,
                            QSharedPointer<Node> &shChildNode,
                            modeMove mode)
{
    bool moveCompleted(true);

    shChildNode->nodeState = shCurNode->nodeState;
    shChildNode->indexEmpty = shCurNode->indexEmpty;
    shChildNode->depth = shCurNode->depth + 1;

    switch(mode)
    {
    case modeMove::up:
        if(shChildNode->indexEmpty > 2)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty - 3]);

            shChildNode->indexEmpty -= 3;
            shChildNode->shParent = shCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::down:
        if(shChildNode->indexEmpty < 6)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty + 3]);

            shChildNode->indexEmpty += 3;
            shChildNode->shParent = shCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::right:
        if(shChildNode->indexEmpty%3 != 2)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty + 1]);

            shChildNode->indexEmpty += 1;
            shChildNode->shParent = shCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::left:
        if(shChildNode->indexEmpty%3 != 0)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty - 1]);

            shChildNode->indexEmpty -= 1;
            shChildNode->shParent = shCurNode;
        }
        else
            moveCompleted = false;
        break;

    default:
        moveCompleted = false;
        break;
    }

    return moveCompleted;
}


bool AlgorithmDLS::checkNewNode(const QSharedPointer<Node> &shCurNode)
{
    bool bOk(true);

    foreach(auto node, m_StackOpen)
        if(shCurNode->nodeState == node->nodeState
                && shCurNode->depth >= node->depth)
            bOk = false;

    foreach(auto node, m_ListClose)
        if(shCurNode->nodeState == node->nodeState)
            if(shCurNode->depth >= node->depth)
                bOk = false;
            else
                m_ListClose.removeOne(node);

    return bOk;
}

bool AlgorithmDLS::checkLimits(const time_t start)
{
    bool bOk(false);

    if(m_LimitMemory != 0)
    {
        size_t sizeAll = (m_StackOpen.size() + m_ListClose.size()) * m_SizeOfNode;
        if(sizeAll >= m_LimitMemory)
            bOk = true;
    }

    if(m_LimitTime != 0)
    {
        time_t step = clock();
        m_Time = step - start;
        if(m_Time >= m_LimitTime)
            bOk = true;
    }

    if((m_LimitNodes != 0 && m_LimitNodes <= m_CreatedNodes)
            || m_Stop)
        bOk = true;

    return bOk;
}
