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


class AlgorithmAstar : public IAlgorithm
{
    int m_SizeOfNode;

    struct Node{
        Node * pLink_1;
        Node * pLink_2;
        Node * pLink_3;
        Node * pLink_4;
        Node * pParent;
        QVector<int> nodeState;
        int nodeCost;
        int distance;
        int indexEmpty;

        Node() : pLink_1(nullptr), pLink_2(nullptr), pLink_3(nullptr), pLink_4(nullptr), pParent(nullptr)
        {
            nodeState.reserve(9);
        }
    };

    Node * m_pRoot;
    QMultiMap<int, Node *> m_MultiMapOpen;
    QList<Node *> m_ListClose;
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
    ~AlgorithmAstar() override;

    bool solvePuzzle() override;
    QList<QVector<int> > getSolution(int &moves) const override;

private:
    void makeRoot();
    int costCalculator(const Node * const pCurNode) const;
    void insertStates(Node * const pCurNode);
    bool makeMove(Node * const pCurNode, Node * const pNodeChild, modeMove mode);
    bool checkNewNode(const Node * const pCurNode);
    bool isSuccess(const Node * const pCurNode);
    void buildResult(const Node * pCurNode);
    bool checkLimits(time_t &start);
};

#endif // ALGORITHMASTAR_H
