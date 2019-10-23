#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800,800);
    setWindowTitle("Game of life");

    mColumns = 20;
    mRows = 20;

    mGameTimer = new QTimer(this);
    mGameTimer->setInterval(300);
    connect(mGameTimer,SIGNAL(timeout()),this,SLOT(advanceGameState()));

    //Create ui elements
    mRunOneStepButton = new QPushButton(this);
    mRunOneStepButton->setText("Run One Step");
    connect(mRunOneStepButton,SIGNAL(clicked(bool)),this,SLOT(advanceGameState()));

    mRunGameTimerButton = new QPushButton(this);
    mRunGameTimerButton->move(100,0);
    mRunGameTimerButton->setText("Start");
    connect(mRunGameTimerButton,SIGNAL(clicked(bool)),this,SLOT(toggleGameTimer()));

    mGameTableSizeScrollBar = new QScrollBar(this);
    mGameTableSizeScrollBar->setOrientation(Qt::Horizontal);
    mGameTableSizeScrollBar->resize(200,30);
    mGameTableSizeScrollBar->move(200,0);
    mGameTableSizeScrollBar->setMinimum(10);
    mGameTableSizeScrollBar->setMaximum(25);
    mGameTableSizeScrollBar->setValue(20);
    connect(mGameTableSizeScrollBar,SIGNAL(valueChanged(int)),this,SLOT(updateGameTableSize(int)));

    //Create Table
    mGameTableView = new QTableView(this);
    mGameTableView->resize(1000,1000);
    mGameTableView->move(0,30);
    mGameTableView->setBaseSize(QSize(30,30));
    mGameTableView->showGrid();
    mGameTableView->horizontalHeader()->hide();
    mGameTableView->verticalHeader()->hide();

    mStandardModel = new QStandardItemModel(mGameTableView);
    mStandardModel->setRowCount(mRows);
    mStandardModel->setColumnCount(mColumns);
    mGameTableView->setModel(mStandardModel);
    connect(mGameTableView,SIGNAL(clicked(QModelIndex)),this,SLOT(itemClicked(QModelIndex)));

    setupTableItems();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setupTableItems()
{
    int rowCount = mGameTableView->model()->rowCount();
    int columnCount = mGameTableView->model()->columnCount();

    for (int row = 0; row < rowCount; row++)
    {
        mGameTableView->setRowHeight(row,30);
        for (int column = 0; column < columnCount; column++)
        {
            mGameTableView->setColumnWidth(column,30);
            QStandardItem* item = new QStandardItem();
            item->setBackground(QColor(Qt::white));
            mStandardModel->setItem(row,column,item);
            item->setSelectable(false);
        }
    }
}
void MainWindow::itemClicked(QModelIndex index)
{
    if(mStandardModel->item(index.row(),index.column())->background() == QColor(Qt::white))
    {
        mStandardModel->item(index.row(),index.column())->setBackground(QColor(Qt::red));
    }
    else
    {
        mStandardModel->item(index.row(),index.column())->setBackground(QColor(Qt::white));
    }
}
void MainWindow::advanceGameState()
{
    QList<QPoint> aliveCells = getAliveCells();
    QList<QPoint> deadCells = getDeadCells();
    QList<QPoint> dyingCells; //These are cells that are going to die at the end of the tick
    QList<QPoint> reproducingCells; //These are cells that are going to come alive at the end of the tick

    int nearbyLivingCells = 0;
    for( int i = 0; i <aliveCells.size();i++)
    {
        nearbyLivingCells = getNeabyLivingCellCount(aliveCells.at(i));

        //Any live cell with fewer than two live neighbours dies, as if by underpopulation.
        if(nearbyLivingCells < 2)
        {
            dyingCells.append(QPoint(aliveCells.at(i).x(),aliveCells.at(i).y()));
        }

        //Any live cell with more than three live neighbours dies, as if by overpopulation.
        if(nearbyLivingCells > 3)
        {
            dyingCells.append(QPoint(aliveCells.at(i).x(),aliveCells.at(i).y()));

        }

        nearbyLivingCells = 0;
    }

    for( int i = 0; i <deadCells.size();i++)
    {
        nearbyLivingCells = getNeabyLivingCellCount(deadCells.at(i));

        //Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
        if(nearbyLivingCells == 3)
        {
            reproducingCells.append(QPoint(deadCells.at(i).x(),deadCells.at(i).y()));
        }
    }

    //This is where the dying cells finally die
    for(int i = 0; i <dyingCells.size();i++)
    {
        mStandardModel->item(dyingCells.at(i).y(),dyingCells.at(i).x())->setBackground(QColor(Qt::white));
    }
    //This is where the reproduced cells come to life
    for(int i = 0; i< reproducingCells.size();i++)
    {
        mStandardModel->item(reproducingCells.at(i).y(),reproducingCells.at(i).x())->setBackground(QColor(Qt::red));
    }
}

int MainWindow::getNeabyLivingCellCount(QPoint cellPosition)
{
    int nearbyLivingCells = 0;

    //These four booleans exists for making sure we dont go out of bounds when checking nearby cell colors near the edges of the grid
    bool ignoreTop = false;
    bool ignoreBottom = false;
    bool ignoreLeft = false;
    bool ignoreRight = false;

    if(cellPosition.y() == 0)
    {
        ignoreTop = true;
    }
    if(cellPosition.y() == mRows-1)
    {
        ignoreBottom = true;
    }
    if (cellPosition.x() == mColumns-1)
    {
        ignoreRight = true;
    }

    if (cellPosition.x() == 0)
    {
        ignoreLeft = true;
    }

    //Check left
    if(ignoreLeft == false && ignoreTop == false && mStandardModel->item(cellPosition.y()-1,cellPosition.x()-1)->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }
    if(ignoreLeft == false && mStandardModel->item(cellPosition.y(),cellPosition.x()-1)->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }
    if(ignoreBottom == false && ignoreLeft == false && mStandardModel->item(cellPosition.y()+1,cellPosition.x()-1)->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }


    //check middle
    if(ignoreTop == false && mStandardModel->item(cellPosition.y()-1,cellPosition.x())->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }
    if(mStandardModel->item(cellPosition.y(),cellPosition.x())->background() == QColor(Qt::red))
    {
        //Do nothing since main cell is not part of the calculations
    }
    if(ignoreBottom == false && mStandardModel->item(cellPosition.y()+1,cellPosition.x())->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }

    //check right
    if(ignoreTop == false && ignoreRight == false && mStandardModel->item(cellPosition.y()-1,cellPosition.x()+1)->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }
    if(ignoreRight == false && mStandardModel->item(cellPosition.y(),cellPosition.x()+1)->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }
    if(ignoreBottom == false && ignoreRight == false && mStandardModel->item(cellPosition.y()+1,cellPosition.x()+1)->background() == QColor(Qt::red))
    {
        nearbyLivingCells++;
    }

    return nearbyLivingCells;
}

QList<QPoint> MainWindow::getDeadCells() //Get list of cells that are currently dead
{
    QList<QPoint> deadCellsList;

    int rowCount = mGameTableView->model()->rowCount();
    int columnCount = mGameTableView->model()->columnCount();

    for (int row = 0; row < rowCount; row++)
    {
        for (int column = 0; column < columnCount; column++)
        {
            if(mStandardModel->item(row,column)->background() == QColor(Qt::white))
            {
                deadCellsList.append(QPoint(column,row));
            }
        }
    }

    return deadCellsList;
}

QList<QPoint> MainWindow::getAliveCells() //Get list of cells that are currently alive
{
    QList<QPoint> aliveCellsList;

    int rowCount = mGameTableView->model()->rowCount();
    int columnCount = mGameTableView->model()->columnCount();

    for (int row = 0; row < rowCount; row++)
    {
        for (int column = 0; column < columnCount; column++)
        {
            if(mStandardModel->item(row,column)->background() == QColor(Qt::red))
            {
                aliveCellsList.append(QPoint(column,row));
            }
        }
    }

    return aliveCellsList;
}
void MainWindow::toggleGameTimer()
{
    if(mGameTimer->isActive())
    {
        mGameTimer->stop();
        qDebug() << "stopping timer";
        mRunGameTimerButton->setText("Start");
        mRunOneStepButton->setEnabled(true);
        mGameTableSizeScrollBar->setEnabled(true);
    }
    else
    {
        mGameTimer->start();
        mRunGameTimerButton->setText("Stop");
        mRunOneStepButton->setEnabled(false);
        mGameTableSizeScrollBar->setEnabled(false);
        qDebug() << "starting timer";

    }
}
void MainWindow::updateGameTableSize(int size)
{
    mRows = size;
    mColumns = size;
    mStandardModel->clear();
    mStandardModel->setRowCount(mRows);
    mStandardModel->setColumnCount(mColumns);
    setupTableItems();
}
