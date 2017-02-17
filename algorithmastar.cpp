// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "algorithmastar.h"

AlgorithmAstar::AlgorithmAstar(const QVector<int> &State,
                               const int limitNodes,
                               const int limitTime, const size_t limitMemory,
                               const modeHeuristic mode,
                               const QVector<int> FinalState,
                               IAlgorithm *parent) :
    IAlgorithm(limitNodes, limitTime, limitMemory, mode, parent)
{
    m_StartState = State;
    m_GoalState = FinalState;
    m_Success = false;

    m_SizeOfNode = sizeof(int) * 12
            + sizeof(void*) * 2
            + sizeof(QSharedPointer<Node>) * 4
            + sizeof(QWeakPointer<Node>);        // incorrect!!!
}


QList<QVector<int>> AlgorithmAstar::getSolution(int &moves) const
{
    moves = m_ListResult.size() - 1;
    return m_ListResult;
}


bool AlgorithmAstar::solvePuzzle()
{
    makeRoot();

    m_MultiMapOpen.insert(m_shRoot->nodeCost, m_shRoot);

    QSharedPointer<Node> shCurNode;
    time_t finish, start = clock();
    while(!m_MultiMapOpen.isEmpty())
    {
        shCurNode = m_MultiMapOpen.first();
        m_MultiMapOpen.erase(m_MultiMapOpen.begin());
        if(isSuccess(shCurNode))
            break;

        insertStates(shCurNode);
        m_ListClose.push_back(shCurNode);

        emit nodesInMemory(m_MultiMapOpen.size());
        emit createdNodes(m_CreatedNodes);

        if(checkLimits(start))
            break;
    }
    finish = clock();
    m_Time = finish - start;

    return m_Success;
}


void AlgorithmAstar::makeRoot()
{
    if(!m_MultiMapOpen.isEmpty())
        m_MultiMapOpen.clear();
    if(!m_ListClose.isEmpty())
        m_ListClose.clear();

    m_shRoot = QSharedPointer<Node>::create();
    m_shRoot->nodeState = m_StartState;
    m_shRoot->nodeCost = costCalculator(m_shRoot);
    m_shRoot->distance = 0;

    m_shRoot->indexEmpty = m_shRoot->nodeState.indexOf(0);
}


int AlgorithmAstar::costCalculator(const QSharedPointer<Node> &shCurNode) const // h1 or h2
{
    int cost(0);

    if(m_Heuristic == modeHeuristic::misplacedTiles)
    {
        for(auto i(0); i < shCurNode->nodeState.size(); i++)
            if(shCurNode->nodeState[i] != m_GoalState.at(i))
                cost++;
    }
    else if(m_Heuristic == modeHeuristic::manhattenDistance)
    {
        int distances[9][9] ={

            //////*0 1 2 3 4 5 6 7 8*//////
            /*1*/{ 4,0,1,2,1,2,3,2,3 }/*1*/,
            /*2*/{ 0,1,0,1,2,1,2,3,2 }/*2*/,
            /*3*/{ 2,2,1,0,3,2,1,4,3 }/*3*/,
            /*4*/{ 3,1,2,3,0,1,2,1,2 }/*4*/,
            /*5*/{ 2,2,1,2,1,0,1,2,1 }/*5*/,
            /*6*/{ 1,3,2,1,2,1,0,3,2 }/*6*/,
            /*7*/{ 2,2,3,4,1,2,3,0,1 }/*7*/,
            /*8*/{ 1,3,2,3,2,1,2,1,0 }/*8*/,
            /*0*/{ 0,4,3,2,3,2,1,2,1 }/*0*/
            //////*0 1 2 3 4 5 6 7 8*//////
        };

        for(int i = 0; i < shCurNode->nodeState.size(); i++) {
            cost += distances[i][shCurNode->nodeState[i]];
        }
    }

    return cost;
}


void AlgorithmAstar::insertStates(QSharedPointer<Node> &shCurNode)
{
    // 1st LINK
    QSharedPointer<Node> shChildNode = QSharedPointer<Node>::create();
    if(makeMove(shCurNode, shChildNode, modeMove::up)
            && checkNewNode(shChildNode))
    {
        shCurNode->shLink_1 = shChildNode;
        m_MultiMapOpen.insert(shChildNode->nodeCost, shChildNode);
        m_CreatedNodes++;
    }

    // 2st LINK
    shChildNode = QSharedPointer<Node>::create();
    if(makeMove(shCurNode, shChildNode, modeMove::down)
            && checkNewNode(shChildNode))
    {
        shCurNode->shLink_2 = shChildNode;
        m_MultiMapOpen.insert(shChildNode->nodeCost, shChildNode);
        m_CreatedNodes++;
    }

    // 3st LINK
    shChildNode = QSharedPointer<Node>::create();
    if(makeMove(shCurNode, shChildNode, modeMove::right)
            && checkNewNode(shChildNode))
    {
        shCurNode->shLink_3 = shChildNode;
        m_MultiMapOpen.insert(shChildNode->nodeCost, shChildNode);
        m_CreatedNodes++;
    }

    // 4st LINK
    shChildNode = QSharedPointer<Node>::create();
    if(makeMove(shCurNode, shChildNode, modeMove::left)
            && checkNewNode(shChildNode))
    {
        shCurNode->shLink_4 = shChildNode;
        m_MultiMapOpen.insert(shChildNode->nodeCost, shChildNode);
        m_CreatedNodes++;
    }
}


bool AlgorithmAstar::makeMove(QSharedPointer<Node> &shCurNode,
                              QSharedPointer<Node> &shChildNode,
                              AlgorithmAstar::modeMove mode) // MAKE AND INIT children
{
    bool moveCompleted(true);
    bool incorrectMove(false);
    shChildNode->nodeState = shCurNode->nodeState;
    shChildNode->indexEmpty = shChildNode->nodeState.indexOf(0);

    if(shCurNode->distance > 2 &&
            shCurNode->shParent.lock()->indexEmpty == shChildNode->indexEmpty) // check loop
    {
        incorrectMove = true;
    }

    switch(mode)
    {
    case modeMove::up:
        if(shChildNode->indexEmpty > 2 && !incorrectMove)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty - 3]);

            shChildNode->distance = shCurNode->distance + 1;
            shChildNode->nodeCost = costCalculator(shChildNode) + shChildNode->distance;
            shChildNode->shParent = shCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::down:
        if(shChildNode->indexEmpty < 6 && !incorrectMove)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty + 3]);

            shChildNode->distance = shCurNode->distance + 1;
            shChildNode->nodeCost = costCalculator(shChildNode) + shChildNode->distance;
            shChildNode->shParent = shCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::right:
        if(shChildNode->indexEmpty%3 != 2 && !incorrectMove)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty + 1]);

            shChildNode->distance = shCurNode->distance + 1;
            shChildNode->nodeCost = costCalculator(shChildNode) + shChildNode->distance;
            shChildNode->shParent = shCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::left:
        if(shChildNode->indexEmpty%3 != 0 && !incorrectMove)
        {
            std::swap(shChildNode->nodeState[shChildNode->indexEmpty],
                    shChildNode->nodeState[shChildNode->indexEmpty - 1]);

            shChildNode->distance = shCurNode->distance + 1;
            shChildNode->nodeCost = costCalculator(shChildNode) + shChildNode->distance;
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


bool AlgorithmAstar::checkNewNode(const QSharedPointer<Node> &shCurNode)
{
    bool bOk(true);

    foreach(auto node, m_MultiMapOpen)
        if(shCurNode->nodeState == node->nodeState
                && shCurNode->distance >= node->distance)
            bOk = false;

    foreach(auto node, m_ListClose)
        if(shCurNode->nodeState == node->nodeState)
            if(shCurNode->distance >= node->distance)
                bOk = false;
            else
                m_ListClose.removeOne(node);

    return bOk;
}


bool AlgorithmAstar::isSuccess(const QSharedPointer<Node> &shCurNode)
{

    if(shCurNode->nodeState == m_GoalState)
    {
        m_Success = true;
        buildResult(shCurNode);
    }

    return m_Success;
}


void AlgorithmAstar::buildResult(const QSharedPointer<Node> &shCurNode)
{
    m_ListResult.clear();

    auto shTmp = shCurNode;
    while(!shTmp.isNull())
    {
        m_ListResult.push_front(shTmp->nodeState);
        shTmp = shTmp->shParent;
    }
}

bool AlgorithmAstar::checkLimits(time_t &start)
{
    bool bOk(false);

    if(m_LimitMemory != 0)
    {
        size_t sizeAll = (m_MultiMapOpen.size() + m_ListClose.size()) * m_SizeOfNode;
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


