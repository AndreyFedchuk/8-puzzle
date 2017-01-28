// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "algorithmastar.h"

AlgorithmAstar::AlgorithmAstar(const QVector<int> &State,
                               const int limitNodes,
                               const int limitTime, const size_t limitMemory,
                               const QVector<int> FinalState,
                               IAlgorithm *parent) :
    IAlgorithm(limitNodes, limitTime, limitMemory, parent)
{
    m_StartState = State;
    m_GoalState = FinalState;
    m_Success = false;

    m_SizeOfNode = (sizeof(int) * 12) + (sizeof(Node*) * 7);
}

AlgorithmAstar::~AlgorithmAstar()
{
    if(!m_MultiMapOpen.isEmpty())
        qDeleteAll(m_MultiMapOpen);
    if(!m_ListClose.isEmpty())
        qDeleteAll(m_ListClose);
}


QList<QVector<int>> AlgorithmAstar::getSolution(int &moves) const
{
    moves = m_ListResult.size() - 1;
    return m_ListResult;
}


bool AlgorithmAstar::solvePuzzle()
{
    makeRoot();

    m_MultiMapOpen.insert(m_pRoot->nodeCost, m_pRoot);

    Node * pCurNode;
    time_t finish, start = clock();
    while(!m_MultiMapOpen.isEmpty())
    {
        pCurNode = m_MultiMapOpen.first();
        m_MultiMapOpen.erase(m_MultiMapOpen.begin());
        if(isSuccess(pCurNode))
            break;

        insertStates(pCurNode);
        m_ListClose.push_back(pCurNode);

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
        qDeleteAll(m_MultiMapOpen);
    if(!m_ListClose.isEmpty())
        qDeleteAll(m_ListClose);

    m_pRoot = new Node();
    m_pRoot->nodeState = m_StartState;
    m_pRoot->nodeCost = costCalculator(m_pRoot);
    m_pRoot->distance = 0;

    m_pRoot->indexEmpty = m_pRoot->nodeState.indexOf(0);
}


int AlgorithmAstar::costCalculator(const Node * const pCurNode) const // h1
{
    int cost(0);
    for(auto i(0); i < pCurNode->nodeState.size(); i++)
        if(pCurNode->nodeState[i] != m_GoalState.at(i))
            cost++;

    return cost;
}


void AlgorithmAstar::insertStates(Node * const pCurNode)
{
    // 1st LINK
    Node * pNodeChild = new Node();
    if(makeMove(pCurNode, pNodeChild, modeMove::up)
            && checkNewNode(pNodeChild))
    {
        pCurNode->pLink_1 = pNodeChild;
        m_MultiMapOpen.insert(pNodeChild->nodeCost, pNodeChild);
        m_CreatedNodes++;
    }
    else
    {
        delete pNodeChild;
    }

    // 2st LINK
    pNodeChild = new Node();
    if(makeMove(pCurNode, pNodeChild, modeMove::down)
            && checkNewNode(pNodeChild))
    {
        pCurNode->pLink_2 = pNodeChild;
        m_MultiMapOpen.insert(pNodeChild->nodeCost, pNodeChild);
        m_CreatedNodes++;
    }
    else
    {
        delete pNodeChild;
    }

    // 3st LINK
    pNodeChild = new Node();
    if(makeMove(pCurNode, pNodeChild, modeMove::right)
            && checkNewNode(pNodeChild))
    {
        pCurNode->pLink_3 = pNodeChild;
        m_MultiMapOpen.insert(pNodeChild->nodeCost, pNodeChild);
        m_CreatedNodes++;
    }
    else
    {
        delete pNodeChild;
    }

    // 4st LINK
    pNodeChild = new Node();
    if(makeMove(pCurNode, pNodeChild, modeMove::left)
            && checkNewNode(pNodeChild))
    {
        pCurNode->pLink_4 = pNodeChild;
        m_MultiMapOpen.insert(pNodeChild->nodeCost, pNodeChild);
        m_CreatedNodes++;
    }
    else
    {
        delete pNodeChild;
    }
}


bool AlgorithmAstar::makeMove(Node * const pCurNode,
                              Node * const pNodeChild,
                              AlgorithmAstar::modeMove mode) // MAKE AND INIT children
{
    bool moveCompleted(true);
    bool incorrectMove(false);
    pNodeChild->nodeState = pCurNode->nodeState;
    pNodeChild->indexEmpty = pNodeChild->nodeState.indexOf(0);

    if(pCurNode->distance > 2 &&
            pCurNode->pParent->indexEmpty == pNodeChild->indexEmpty) // check loop
    {
        incorrectMove = true;
    }

    switch(mode)
    {
    case modeMove::up:
        if(pNodeChild->indexEmpty > 2 && !incorrectMove)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty - 3]);

            pNodeChild->distance = pCurNode->distance + 1;
            pNodeChild->nodeCost = costCalculator(pNodeChild) + pNodeChild->distance;
            pNodeChild->pParent = pCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::down:
        if(pNodeChild->indexEmpty < 6 && !incorrectMove)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty + 3]);

            pNodeChild->distance = pCurNode->distance + 1;
            pNodeChild->nodeCost = costCalculator(pNodeChild) + pNodeChild->distance;
            pNodeChild->pParent = pCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::right:
        if(pNodeChild->indexEmpty%3 != 2 && !incorrectMove)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty + 1]);

            pNodeChild->distance = pCurNode->distance + 1;
            pNodeChild->nodeCost = costCalculator(pNodeChild) + pNodeChild->distance;
            pNodeChild->pParent = pCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::left:
        if(pNodeChild->indexEmpty%3 != 0 && !incorrectMove)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty - 1]);

            pNodeChild->distance = pCurNode->distance + 1;
            pNodeChild->nodeCost = costCalculator(pNodeChild) + pNodeChild->distance;
            pNodeChild->pParent = pCurNode;
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


bool AlgorithmAstar::checkNewNode(const Node * const pCurNode)
{
    bool bOk(true);

    foreach(auto node, m_MultiMapOpen)
        if(pCurNode->nodeState == node->nodeState
                && pCurNode->distance >= node->distance)
            bOk = false;

    foreach(auto node, m_ListClose)
        if(pCurNode->nodeState == node->nodeState)
            if(pCurNode->distance >= node->distance)
                bOk = false;
            else
                m_ListClose.removeOne(node);

    return bOk;
}


bool AlgorithmAstar::isSuccess(const Node * const pCurNode)
{

    if(pCurNode->nodeState == m_GoalState)
    {
        m_Success = true;
        buildResult(pCurNode);
    }

    return m_Success;
}


void AlgorithmAstar::buildResult(const Node *pCurNode)
{
    m_ListResult.clear();

    while(pCurNode != nullptr)
    {
        m_ListResult.push_front(pCurNode->nodeState);
        pCurNode = pCurNode->pParent;
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


