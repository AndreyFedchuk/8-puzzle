// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#ifndef ALGORITHMDLS_H
#define ALGORITHMDLS_H

#include "ialgorithm.h"
#include <QStack>
#include <QSharedPointer>
#include <QWeakPointer>

class AlgorithmDLS : public IAlgorithm
{
    int m_SizeOfNode;

    struct Node{
        QWeakPointer<Node> shParent;
        QVector<QSharedPointer<Node>> Childs;
        int depth;
        int indexEmpty;
        QVector<int> nodeState;

        Node() : depth(0)
        {
            Childs.resize(static_cast<int>(modeMove::size));
            nodeState.reserve(9);
        }
    };

    int m_DepthLimit;
    QSharedPointer<Node> m_shRoot;

    QVector<int> m_StartState;
    QVector<int> m_GoalState;
    QStack<QSharedPointer<Node>> m_StackOpen;
    QList<QSharedPointer<Node>> m_ListClose;
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

    bool solvePuzzle() override;
    QList<QVector<int>> getSolution(int &moves) const override;

private:
    void makeRoot();
    bool isSuccess(const QSharedPointer<Node> &shCurNode);
    void buildResult(const QSharedPointer<Node> &shCurNode);
    void makeStates(QSharedPointer<Node> &shCurNode);
    bool makeMove(QSharedPointer<Node> &shCurNode, QSharedPointer<Node> &shChildNode, modeMove mode);
    bool checkNewNode(const QSharedPointer<Node> &shCurNode);
    bool checkLimits(const time_t start);
};

#endif // ALGORITHMDLS_H
