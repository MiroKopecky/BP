#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "preprocessing.h"
#include "freqMap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void RunPreprocessing();
private:
    Ui::MainWindow *ui;
    Preprocessing p;
    int jobDoneCounter = 0;
    QString path;

    FreqMap f;
    
private slots:
    void preprocessingSlot(PREPROCESSING_ALL_RESULTS);
    void on_pushButton_3_clicked();
    void on_OpenImage_clicked();
    void on_pushButton_clicked();
    void drawImageSlot(cv::Mat fMap);
};


#endif // MAINWINDOW_H
