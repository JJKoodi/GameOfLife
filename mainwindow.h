#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QList>
#include <QStandardItemModel>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <QScrollBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTableView *mGameTableView;
    QStandardItemModel *mStandardModel;
    QPushButton *mRunOneStepButton;
    QPushButton *mRunGameTimerButton;
    QScrollBar *mGameTableSizeScrollBar;

    QTimer *mGameTimer;
    int mColumns;
    int mRows;

    void setupTableItems();
    int getNeabyLivingCellCount(QPoint cellPosition);
    QList<QPoint> getAliveCells();
    QList<QPoint> getDeadCells();

private slots:
    void itemClicked(QModelIndex index);
    void advanceGameState();
    void toggleGameTimer();
    void updateGameTableSize(int size);


};

#endif // MAINWINDOW_H
