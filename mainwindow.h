// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QMap>
#include <QList>
#include <QVector>
#include <QThread>
#include <QSharedPointer>

#include "about.h"
#include "puzzle.h"
#include "algorithmastar.h"
#include "algorithmdls.h"
#include "logging.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    const int SIZE = 9;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnSolve_clicked();
    void on_btnShuffle_clicked();
    void on_btnShowSolution_clicked();
    void on_actionAbout_triggered();

    void printResult(bool success);
    void printCountNodes(int countNodes);
    void printNodesInMemory(int nodesInMemory);

    void on_actionOpenLogging_triggered();

    void on_actionAboutQt_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    QMap<int, QPointF> m_MapCoordPuzzle;
    QList<QSharedPointer<QGraphicsItem>> m_listGraphicsItem;
    QVector<int> m_StartState;
    bool m_ShowedSolution;
    Logging m_Logging;

    QSharedPointer<IAlgorithm> m_shAlgorithm;
    QThread m_AlgorithmThread;
    QList<QVector<int>> m_Solution;
    int m_Time;
    int m_Moves;
    int m_LimitStates;
    int m_LimitTime;
    size_t m_LimitMemory;

    bool parsingAndChecking();
    void display(const QVector<int> * const pPuzzleNumbers = nullptr);
    void updateData();
    bool checkSolvability(const QVector<int> &startState);
    bool parsing();
    void setAlgorithm();
    void setLimits();
    void resetResult();
    void logging();
};

#endif // MAINWINDOW_H
