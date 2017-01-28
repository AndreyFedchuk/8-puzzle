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
                           const QVector<int> FinalState,
                           IAlgorithm *parent) :
    IAlgorithm(limitNodes, limitTime, limitMemory, parent)
{
    m_StartState = State;
    m_GoalState = FinalState;
    m_pRoot = nullptr;
    m_Success = false;
    m_DepthLimit = 26;

    m_SizeOfNode = (sizeof(int) * 11) + (sizeof(Node*) * 8);
}

AlgorithmDLS::~AlgorithmDLS()
{
    if(!m_StackOpen.isEmpty())
        qDeleteAll(m_StackOpen);

    if(!m_ListClose.isEmpty())
        qDeleteAll(m_ListClose);
}

bool AlgorithmDLS::solvePuzzle()
{
    makeRoot();

    m_StackOpen.push(m_pRoot);
    Node * pCurNode;

    time_t finish, start = clock();

    while(!m_StackOpen.isEmpty())
    {
        pCurNode = m_StackOpen.pop();
        m_ListClose.push_back(pCurNode);

        if(isSuccess(pCurNode)
                || checkLimits(start))
            break;

        emit nodesInMemory(m_StackOpen.size());
        emit createdNodes(m_CreatedNodes);

        if(pCurNode->depth >= m_DepthLimit)
            continue;

        makeStates(pCurNode);
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
    m_pRoot = new Node();
    m_pRoot->depth = 0;
    m_pRoot->nodeState = m_StartState;
    m_pRoot->indexEmpty = m_pRoot->nodeState.indexOf(0);
}


bool AlgorithmDLS::isSuccess(const Node * const pCurNode)
{

    if(pCurNode->nodeState == m_GoalState)
    {
        m_Success = true;
        buildResult(pCurNode);
    }

    return m_Success;
}


void AlgorithmDLS::buildResult(const Node *pCurNode)
{
    m_ListResult.clear();

    while(pCurNode != nullptr)
    {
        m_ListResult.push_front(pCurNode->nodeState);
        pCurNode = pCurNode->pParent;
    }
}


void AlgorithmDLS::makeStates(AlgorithmDLS::Node * const pCurNode)
{
    for(int i(0); i < pCurNode->Childs.size(); i++)
    {
        pCurNode->Childs[i] = new Node();
        if(makeMove(pCurNode, pCurNode->Childs[i], static_cast<modeMove>(i))
                && checkNewNode(pCurNode->Childs[i]))
        {
            m_StackOpen.push(pCurNode->Childs[i]);
            m_CreatedNodes++;
        }
        else
        {
            delete pCurNode->Childs[i];
            pCurNode->Childs[i] = nullptr;
        }
    }
}

bool AlgorithmDLS::makeMove(AlgorithmDLS::Node * const pCurNode,
                            AlgorithmDLS::Node * const pNodeChild,
                            modeMove mode)
{
    bool moveCompleted(true);

    pNodeChild->nodeState = pCurNode->nodeState;
    pNodeChild->indexEmpty = pCurNode->indexEmpty;
    pNodeChild->depth = pCurNode->depth + 1;

    switch(mode)
    {
    case modeMove::up:
        if(pNodeChild->indexEmpty > 2)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty - 3]);

            pNodeChild->indexEmpty -= 3;
            pNodeChild->pParent = pCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::down:
        if(pNodeChild->indexEmpty < 6)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty + 3]);

            pNodeChild->indexEmpty += 3;
            pNodeChild->pParent = pCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::right:
        if(pNodeChild->indexEmpty%3 != 2)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty + 1]);

            pNodeChild->indexEmpty += 1;
            pNodeChild->pParent = pCurNode;
        }
        else
            moveCompleted = false;
        break;

    case modeMove::left:
        if(pNodeChild->indexEmpty%3 != 0)
        {
            std::swap(pNodeChild->nodeState[pNodeChild->indexEmpty],
                    pNodeChild->nodeState[pNodeChild->indexEmpty - 1]);

            pNodeChild->indexEmpty -= 1;
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


bool AlgorithmDLS::checkNewNode(const AlgorithmDLS::Node * const pCurNode)
{
    bool bOk(true);

    foreach(auto node, m_StackOpen)
        if(pCurNode->nodeState == node->nodeState
                && pCurNode->depth >= node->depth)
            bOk = false;

    foreach(auto node, m_ListClose)
        if(pCurNode->nodeState == node->nodeState)
            if(pCurNode->depth >= node->depth)
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

















