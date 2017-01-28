// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pAlgorithm(nullptr)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(-105, -105, 210, 210);

    QIntValidator * validator = new QIntValidator(this);
    ui->lineEdit->setValidator(validator);

    m_pMapCoordPuzzle = new QMap<int, QPointF>({
                                                {1, QPointF(-70,-70)},
                                                {2, QPointF(0,-70)},
                                                {3, QPointF(70,-70)},
                                                {4, QPointF(-70,0)},
                                                {5, QPointF(0,0)},
                                                {6, QPointF(70,0)},
                                                {7, QPointF(-70,70)},
                                                {8, QPointF(0,70)},
                                                {9, QPointF(70,70)}
                                            });


    ui->comboBoxALGO->addItem("A* H1 (Number of Misplaced tiles)");
    ui->comboBoxALGO->addItem("DLS (Depth Limited Search)");

    parse();
    display();
    updateData();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pMapCoordPuzzle;
    delete m_pAlgorithm;
}


void MainWindow::on_btnSolve_clicked()
{
    if(parse())
    {
        ui->btnSolve->setEnabled(false);

        display();
        updateData();

        connect(&m_AlgorithmThread, &QThread::started, m_pAlgorithm, &IAlgorithm::runSolve);
        connect(m_pAlgorithm, &IAlgorithm::finished, &m_AlgorithmThread, &QThread::terminate);
        connect(m_pAlgorithm, &IAlgorithm::finished, this, &MainWindow::printResult);
        connect(m_pAlgorithm, &IAlgorithm::createdNodes, this, &MainWindow::printCountNodes);
        connect(m_pAlgorithm, &IAlgorithm::nodesInMemory, this, &MainWindow::printNodesInMemory);
        connect(ui->btnStop, &QPushButton::clicked, m_pAlgorithm, &IAlgorithm::stopSolve, Qt::DirectConnection);

        m_pAlgorithm->moveToThread(&m_AlgorithmThread);

        m_AlgorithmThread.start();

        ui->btnStop->setEnabled(true);
    }

}

bool MainWindow::parse()
{
    m_Numbers.clear();

    bool bOk(true);
    QString sNumbers = ui->lineEdit->displayText();
    foreach(auto ch, sNumbers)
    {
        QString str(ch);
        int number = str.toInt();
        if(m_Numbers.contains(number) || number == 9 || number < 0)
        {
            bOk = false;
            m_Numbers.clear();
            break;
        }
        m_Numbers.append(str.toInt());
    }

    if(bOk == false || m_Numbers.size() != SIZE)
    {
        bOk = false;
        m_Numbers.clear();
        QMessageBox * msg = new QMessageBox(QMessageBox::Critical,
                                "Not a valid input!",
                                "The numbers must not repeat.\n "
                                "Enter a unique 9 digits [0 - 8].",
                                QMessageBox::Ok);
        msg->exec();
        msg->deleteLater();
    }

    return bOk;
}

void MainWindow::display(const QVector<int> * const pPuzzleNumbers)
{
    m_listGraphicsItem.clear();
    scene->clear();

    if(pPuzzleNumbers != nullptr)
        m_Numbers = *pPuzzleNumbers;
    int index = 1;
    foreach(auto Num, m_Numbers)
    {
        if(Num > 0)
        {
            auto item = new Puzzle(QString::number(Num));
            item->setPos(m_pMapCoordPuzzle->value(index));
            scene->addItem(item);
            m_listGraphicsItem.append(item);
        }
        index++;
    }
}

void MainWindow::updateData()
{  
    if(ui->cbLimitNodex->isChecked())
        m_LimitStates = ui->spLimitNodes->value();
    else
        m_LimitStates = 0;

    if(ui->cbLimitTime->isChecked())
        m_LimitTime = ui->spLimitTime->value() * 60000; // min to ms
    else
        m_LimitTime = 0;

    if(ui->cbLimitMemory->isChecked())
        m_LimitMemory = ui->spLimitMemory->value() * 1000000; // mb to byte
    else
        m_LimitMemory = 0;

    m_ShowedSolution = false;

    m_Time = 0;
    ui->lblTime->setText(QString::number(m_Time));

    m_Moves = 0;
    ui->lblNumMoves->setText(QString::number(m_Moves));

    ui->lblCreatedNodes->setText("0");

    int index = ui->comboBoxALGO->currentIndex();
    switch(index)
    {
    case 0:
        if(m_pAlgorithm != nullptr)
            delete m_pAlgorithm;
        m_pAlgorithm = new AlgorithmAstar(m_Numbers, m_LimitStates, m_LimitTime, m_LimitMemory);
        break;

    case 1:
        if(m_pAlgorithm != nullptr)
            delete m_pAlgorithm;
        m_pAlgorithm = new AlgorithmDLS(m_Numbers, m_LimitStates, m_LimitTime, m_LimitMemory);
        break;

    default:
        m_pAlgorithm = nullptr;
    }

    if(!m_Solution.isEmpty())
        m_Solution.clear();

    ui->btnShowSolution->setEnabled(false);
}

void MainWindow::on_btnShuffle_clicked()
{
    QString str = "123456780";
    std::random_shuffle(str.begin(), str.end());
    ui->lineEdit->setText(str);
}

void MainWindow::on_btnShowSolution_clicked()
{
    ui->btnShowSolution->setEnabled(false);
    ui->btnSolve->setEnabled(false);

    if(m_ShowedSolution)
        display();

    Puzzle *puzzle = nullptr;
    int indexCurr(0), indexNext(0);
    for(auto it = m_Solution.begin(); it != m_Solution.end() - 1; ++it)
    {
        indexCurr = (*it).indexOf(0) + 1;
        indexNext = (*(it + 1)).indexOf(0) + 1;
        puzzle = dynamic_cast<Puzzle *>(scene->itemAt(m_pMapCoordPuzzle->value(indexNext),
                                                      QTransform()));
        if(puzzle != nullptr)
            puzzle->movePuzzle(m_pMapCoordPuzzle->value(indexCurr));
    }

    m_ShowedSolution = true;

    ui->btnSolve->setEnabled(true);
    ui->btnShowSolution->setEnabled(true);    
}

void MainWindow::on_actionAbout_triggered()
{
    About * about = new About(this);
    about->exec();
    about->deleteLater();
}

void MainWindow::printResult(bool success)
{
    disconnect(&m_AlgorithmThread, &QThread::started, m_pAlgorithm, &IAlgorithm::runSolve);
    disconnect(m_pAlgorithm, &IAlgorithm::finished, &m_AlgorithmThread, &QThread::terminate);
    disconnect(m_pAlgorithm, &IAlgorithm::finished, this, &MainWindow::printResult);
    disconnect(m_pAlgorithm, &IAlgorithm::createdNodes, this, &MainWindow::printCountNodes);
    disconnect(m_pAlgorithm, &IAlgorithm::nodesInMemory, this, &MainWindow::printNodesInMemory);
    disconnect(ui->btnStop, &QPushButton::clicked, m_pAlgorithm, &IAlgorithm::stopSolve);

    ui->btnStop->setEnabled(false);

    m_Time = m_pAlgorithm->getTime();
    ui->lblTime->setText(QString::number(m_Time));

    if(success)
    {
        m_Solution = m_pAlgorithm->getSolution(m_Moves);
        ui->lblNumMoves->setText(QString::number(m_Solution.size() - 1));
        ui->btnShowSolution->setEnabled(true);
    }
    else
    {
        QMessageBox::information(this,
                                 "Unsolvable!",
                                 "This set of puzzles unsolvable",
                                 QMessageBox::Ok);
        m_Numbers = {1,2,3,4,5,6,7,8,0};
        display(&m_Numbers);
    }

    ui->btnSolve->setEnabled(true);
}

void MainWindow::printCountNodes(int countNodes)
{
    ui->lblCreatedNodes->setText(QString::number(countNodes));
}

void MainWindow::printNodesInMemory(int nodesInMemory)
{
    ui->lblNodesInMemory->setText(QString::number(nodesInMemory));
}