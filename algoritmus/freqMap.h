#ifndef FREQMAP_H
#define FREQMAP_H

#include <QObject>
#include "preprocessing.h"
#include <math.h>
#include <QFileDialog>
#include <QThread>
#include <QtCore>
#include <QVector>
#include "persistence1d.h"


class FreqMap: public QObject {
    Q_OBJECT
    cv::Mat imgOriginal;
    cv::Mat orientMap;
    cv::Mat freqMap;
    int from, to, cols, block;
    int count = 1;
    int jobDoneCounter = 0;
    double GaussBlock, GaussSigma, width, height;
    QVector <QThread*> t;
    QVector <FreqMap*> s;

signals:
    void jobDone();
    void startSum();
    void allJobsDone();
    void drawImage(cv::Mat);

public:
    FreqMap(QWidget *parent = nullptr);

    void setParams(cv::Mat imgOriginal, cv::Mat orientMapQ, cv::Mat frequencyMap, int from, int to, int cols, int block);

    int getBlock() const;

    void run();

    void setBlock(int value);

    void setImgOriginal(const cv::Mat &value);

    void setOrientMap(const cv::Mat &value);

    void setGauss(double, double);

    void setOrientedWindow(double width, double height);

public slots:

    void calcFreq();
    void jobDoneSlot();
    void allJobsDoneSlot();

};

#endif // FREQMAP_H
