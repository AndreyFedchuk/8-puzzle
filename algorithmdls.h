// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#ifndef ALGORITHMDLS_H
#define ALGORITHMDLS_H

#include "ialgorithm.h"
#include <QStack>

class AlgorithmDLS : public IAlgorithm
{
    int m_SizeOfNode;

    struct Node{
        Node * pParent;
        QVector<Node*> Childs;
        int depth;
        int indexEmpty;
        QVector<int> nodeState;

        Node() : pParent(nullptr), depth(0)
        {
            Childs.resize(static_cast<int>(modeMove::size));
            Childs.fill(nullptr);
            nodeState.reserve(9);
        }
    };

    int m_DepthLimit;
    Node * m_pRoot;

    QVector<int> m_StartState;
    QVector<int> m_GoalState;
    QStack<Node *> m_StackOpen;
    QList<Node *> m_ListClose;
    QList<QVector<int>> m_ListResult;
    bool m_Success;

public:
    explicit AlgorithmDLS(const QVector<int> &State,
                          const int limitNodes,
                          const int limitTime,
                          const size_t limitMemory,
                          const modeHeuristic mode,
                          const QVector<int> FinalState = {1,2,3,4,5,6,7,8,0},
                          IAlgorithm * parent = nullptr);
    ~AlgorithmDLS() override;

    bool solvePuzzle() override;
    QList<QVector<int>> getSolution(int &moves) const override;

private:
    void makeRoot();
    bool isSuccess(const Node * const pCurNode);
    void buildResult(const Node *pCurNode);
    void makeStates(Node * const pCurNode);
    bool makeMove(Node * const pCurNode, Node *const pNodeChild, modeMove mode);
    bool checkNewNode(const Node * const pCurNode);
    bool checkLimits(const time_t start);
};

#endif // ALGORITHMDLS_H
