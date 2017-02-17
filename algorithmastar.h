// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#ifndef ALGORITHMASTAR_H
#define ALGORITHMASTAR_H

#include "ialgorithm.h"
#include <QString>
#include <QMultiMap>
#include <QSharedPointer>
#include <QWeakPointer>


class AlgorithmAstar : public IAlgorithm
{
    int m_SizeOfNode;

    struct Node{
        QWeakPointer<Node> shParent;
        QSharedPointer<Node> shLink_1;
        QSharedPointer<Node> shLink_2;
        QSharedPointer<Node> shLink_3;
        QSharedPointer<Node> shLink_4;
        QVector<int> nodeState;
        int nodeCost;
        int distance;
        int indexEmpty;

        Node()
        {
            nodeState.reserve(9);
        }
    };

    QSharedPointer<Node> m_shRoot;
    QMultiMap<int, QSharedPointer<Node>> m_MultiMapOpen;
    QList<QSharedPointer<Node>> m_ListClose;
    QVector<int> m_StartState;
    QVector<int> m_GoalState;
    QList<QVector<int>> m_ListResult;
    bool m_Success;

public:
    explicit AlgorithmAstar(const QVector<int> &State,
                            const int limitNodes,
                            const int limitTime,
                            const size_t limitMemory,
                            const modeHeuristic mode,
                            const QVector<int> FinalState = {1,2,3,4,5,6,7,8,0},
                            IAlgorithm * parent = nullptr);

    bool solvePuzzle() override;
    QList<QVector<int> > getSolution(int &moves) const override;

private:
    void makeRoot();
    int costCalculator(const QSharedPointer<Node> &shCurNode) const;
    void insertStates(QSharedPointer<Node> &shCurNode);
    bool makeMove(QSharedPointer<Node> &shCurNode, QSharedPointer<Node> &shChildNode, modeMove mode);
    bool checkNewNode(const QSharedPointer<Node> &shCurNode);
    bool isSuccess(const QSharedPointer<Node> &shCurNode);
    void buildResult(const QSharedPointer<Node> &shCurNode);
    bool checkLimits(time_t &start);
};

#endif // ALGORITHMASTAR_H
