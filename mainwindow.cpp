// ///////////////////////////////////////////////////////
// Copyright © 2017 Andriy Hudz. All rights reserved.
// email: andrey.hudz.90@gmail.com
// https://www.facebook.com/AndreyHudz90
// ///////////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(-105, -105, 210, 210);

    QIntValidator * validator = new QIntValidator(this);
    ui->lineEdit->setValidator(validator);

    m_MapCoordPuzzle = {{1, QPointF(-70,-70)},
                        {2, QPointF(0,-70)},
                        {3, QPointF(70,-70)},
                        {4, QPointF(-70,0)},
                        {5, QPointF(0,0)},
                        {6, QPointF(70,0)},
                        {7, QPointF(-70,70)},
                        {8, QPointF(0,70)},
                        {9, QPointF(70,70)}};


    ui->comboBoxALGO->addItem("A* H1 (Number of Misplaced tiles)");
    ui->comboBoxALGO->addItem("DLS (Depth Limited Search)");
    ui->comboBoxALGO->addItem("A* H2 (Manhattan Distance)");

    ui->comboBoxALGO->setCurrentIndex(2);

    parsingAndChecking();
    display();
    updateData();

    qsrand(time(0)); // for std::random_shuffle
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSolve_clicked()
{
    if(parsingAndChecking())
    {
        ui->btnSolve->setEnabled(false);

        display();
        updateData();

        connect(&m_AlgorithmThread, &QThread::started, m_shAlgorithm.data(), &IAlgorithm::runSolve);
        connect(m_shAlgorithm.data(), &IAlgorithm::finished, &m_AlgorithmThread, &QThread::terminate);
        connect(m_shAlgorithm.data(), &IAlgorithm::finished, this, &MainWindow::printResult);
        connect(m_shAlgorithm.data(), &IAlgorithm::createdNodes, this, &MainWindow::printCountNodes);
        connect(m_shAlgorithm.data(), &IAlgorithm::nodesInMemory, this, &MainWindow::printNodesInMemory);
        connect(ui->btnStop, &QPushButton::clicked, m_shAlgorithm.data(), &IAlgorithm::stopSolve, Qt::DirectConnection);

        m_shAlgorithm->moveToThread(&m_AlgorithmThread);

        m_AlgorithmThread.start();

        ui->btnStop->setEnabled(true);
    }

}

bool MainWindow::parsingAndChecking()
{
    bool bOk(true);
    if(!parsing())
    {
        bOk = false;
        m_StartState.clear();
        QMessageBox::critical(this,
                              "Not a valid input!",
                              "The numbers must not repeat.\n "
                              "Enter a unique 9 digits [0 - 8].",
                              QMessageBox::Ok);
    }
    else if(!checkSolvability(m_StartState))
    {
        bOk = false;
        m_StartState.clear();
        QMessageBox::critical(this,
                              "Unsolvable!",
                              "This set of puzzles unsolvable",
                              QMessageBox::Ok);
    }

    return bOk;
}

void MainWindow::display(const QVector<int> * const pPuzzleNumbers)
{
    m_listGraphicsItem.clear();

    if(pPuzzleNumbers != nullptr)
        m_StartState = *pPuzzleNumbers;
    int index = 1;
    foreach(auto Num, m_StartState)
    {
        if(Num > 0)
        {
            auto item = QSharedPointer<Puzzle>::create(QString::number(Num));
            item->setPos(m_MapCoordPuzzle.value(index));
            scene->addItem(item.data());
            m_listGraphicsItem.append(item);
        }
        index++;
    }
}

void MainWindow::updateData()
{
    resetResult();

    setLimits();

    setAlgorithm();
}

bool MainWindow::checkSolvability(const QVector<int> &startState)
{
    int inversions = 0;
    for (int i(0); i < (SIZE); ++i)
        for (int j(i); j < (SIZE); ++j)
            if (startState[i] > startState[j]
                    && startState[j] != 0)
                inversions++;

    return (inversions % 2 == 0);
}

bool MainWindow::parsing()
{
    m_StartState.clear();

    bool bOk(true);
    QString sNumbers = ui->lineEdit->displayText();
    foreach(auto ch, sNumbers)
    {
        QString str(ch);
        int number = str.toInt();
        if(m_StartState.contains(number) || number == 9 || number < 0)
        {
            bOk = false;
            m_StartState.clear();
            break;
        }
        m_StartState.append(str.toInt());
    }

    if(m_StartState.size() != SIZE)
        bOk = false;

    return bOk;
}

void MainWindow::setAlgorithm()
{
    int index = ui->comboBoxALGO->currentIndex();
    switch(index)
    {
    case 0:
        m_shAlgorithm = QSharedPointer<AlgorithmAstar>::create(m_StartState,
                                                               m_LimitStates,
                                                               m_LimitTime,
                                                               m_LimitMemory,
                                                               modeHeuristic::misplacedTiles);
        break;

    case 1:
        m_shAlgorithm = QSharedPointer<AlgorithmDLS>::create(m_StartState,
                                                             m_LimitStates,
                                                             m_LimitTime,
                                                             m_LimitMemory,
                                                             modeHeuristic::none);
        break;

    case 2:
        m_shAlgorithm = QSharedPointer<AlgorithmAstar>::create(m_StartState,
                                                               m_LimitStates,
                                                               m_LimitTime,
                                                               m_LimitMemory,
                                                               modeHeuristic::manhattenDistance);
        break;

    default:
        m_shAlgorithm.reset();
    }

    Q_ASSERT(!m_shAlgorithm.isNull());
}

void MainWindow::setLimits()
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
}

void MainWindow::resetResult()
{
    m_Time = 0;
    ui->lblTime->setText(QString::number(m_Time));

    m_Moves = 0;
    ui->lblNumMoves->setText(QString::number(m_Moves));

    ui->lblCreatedNodes->setText("0");

    if(!m_Solution.isEmpty())
        m_Solution.clear();
    ui->btnShowSolution->setEnabled(false);
    m_ShowedSolution = false;
}

void MainWindow::logging()
{
    if(!m_Logging.logingInFile(&m_Solution))
    {
        QMessageBox::warning(this,
                             "Error",
                             "Error while recording in file.",
                             QMessageBox::Ok);
    }
}

void MainWindow::on_btnShuffle_clicked()
{
    QVector<int> startState({1,2,3,4,5,6,7,8,0});

    do
    {
        std::random_shuffle(startState.begin(), startState.end());
    }
    while(!checkSolvability(startState));

    QString str("");

    foreach(auto integer, startState)
        str += QString::number(integer);

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
        puzzle = dynamic_cast<Puzzle *>(scene->itemAt(m_MapCoordPuzzle.value(indexNext),
                                                      QTransform()));
        if(puzzle != nullptr)
            puzzle->movePuzzle(m_MapCoordPuzzle.value(indexCurr));
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
    disconnect(&m_AlgorithmThread, &QThread::started, m_shAlgorithm.data(), &IAlgorithm::runSolve);
    disconnect(m_shAlgorithm.data(), &IAlgorithm::finished, &m_AlgorithmThread, &QThread::terminate);
    disconnect(m_shAlgorithm.data(), &IAlgorithm::finished, this, &MainWindow::printResult);
    disconnect(m_shAlgorithm.data(), &IAlgorithm::createdNodes, this, &MainWindow::printCountNodes);
    disconnect(m_shAlgorithm.data(), &IAlgorithm::nodesInMemory, this, &MainWindow::printNodesInMemory);
    disconnect(ui->btnStop, &QPushButton::clicked, m_shAlgorithm.data(), &IAlgorithm::stopSolve);

    ui->btnStop->setEnabled(false);

    m_Time = m_shAlgorithm->getTime();
    ui->lblTime->setText(QString::number(m_Time));

    if(success)
    {
        m_Solution = m_shAlgorithm->getSolution(m_Moves);
        ui->lblNumMoves->setText(QString::number(m_Solution.size() - 1));
        ui->btnShowSolution->setEnabled(true);

        QMessageBox::information(this,
                                 "Success!",
                                 "The 8-puzzle problem was solved.",
                                 QMessageBox::Ok);

        logging();
    }
    else
    {
        QMessageBox::warning(this,
                             "Stoped!",
                             "The search was stopped.",
                             QMessageBox::Ok);
        m_StartState = {1,2,3,4,5,6,7,8,0};
        display(&m_StartState);
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

void MainWindow::on_actionOpenLogging_triggered()
{
    if(!m_Logging.showLoggingFile())
    {
        QMessageBox::critical(this,
                             "ERROR",
                             "Error, loggin file not open!",
                             QMessageBox::Ok);
    }
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}
