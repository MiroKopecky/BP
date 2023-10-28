#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "persistence1d.h"
#include <QThread>
#include <QVector>
#include <QDebug>
#include <iostream>
#include <QObject>
#include <math.h>
#include <unistd.h>
#include <QFileDialog>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<PREPROCESSING_ALL_RESULTS >("PREPROCESSING_ALL_RESULTS");
    connect(&p, SIGNAL(preprocessingDoneSignal(PREPROCESSING_ALL_RESULTS)), this, SLOT(preprocessingSlot(PREPROCESSING_ALL_RESULTS)));
    connect(&this->f,&FreqMap::drawImage,this,&MainWindow::drawImageSlot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::preprocessingSlot(PREPROCESSING_ALL_RESULTS res) {

    ui->label_11->setPixmap(QPixmap::fromImage(QImage(res.imgOrientationMap.data, res.imgOrientationMap.cols, res.imgOrientationMap.rows, res.imgOrientationMap.step, QImage::Format_RGB888)));

    this->f.setBlock(ui->BlockSize->value());
    this->f.setImgOriginal(res.imgOriginal);
    this->f.setOrientMap(res.orientationMap);
    this->f.setGauss(ui->GaussBlock->value(),ui->GausSigma->value());
    this->f.setOrientedWindow(ui->width->value(),ui->height->value());
    this->f.run();

}

void MainWindow::RunPreprocessing() {
    QListWidgetItem *itm = ui->listWidget->currentItem();
    QString name = itm->text();
    p.loadInput(name);
    p.setFeatures(true);
    p.setPreprocessingParams(ui->BlockSize->value());
    //p.setCPUOnly(true);
    p.start();
}

void MainWindow::on_OpenImage_clicked()
{
    QListWidgetItem *itm = ui->listWidget->currentItem();
    QString name = itm->text();
    QPixmap pix;
    pix.load(name);
    pix = pix.scaled(pix.width(), pix.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_10->setPixmap(pix);
}

void MainWindow::on_pushButton_3_clicked()
{
    this->RunPreprocessing();
}

void MainWindow::on_pushButton_clicked()
{
    QDir directory = QFileDialog::getExistingDirectory(this, tr("select directory"));
    QDir::setCurrent(directory.absolutePath());
    QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.JPG" << "*.png" << "*.PNG" << "*.tif" << "*.TIF",QDir::Files);

    ui->listWidget->clear();
    foreach(QString filename, images) {
        filename = QFileInfo(filename).absoluteFilePath();
        ui->listWidget->addItem(filename);
    }
    QPixmap pix;
    ui->label->setPixmap(pix);
}

void MainWindow::drawImageSlot(cv::Mat fMap)
{
    QImage imdisplay((uchar*)fMap.data, fMap.cols, fMap.rows, fMap.step, QImage::Format_Grayscale8);
    ui->label_9->setPixmap(QPixmap::fromImage(imdisplay));
}
