#include "freqMap.h"

void FreqMap::setBlock(int value)
{
    block = value;
}

int FreqMap::getBlock() const
{
    return block;
}

void FreqMap::setImgOriginal(const cv::Mat &value)
{
    imgOriginal = value;
}

void FreqMap::setOrientMap(const cv::Mat &value)
{
    orientMap = value;
}

void FreqMap::setGauss(double GaussBlock, double GaussSigma)
{
    this->GaussBlock = GaussBlock;
    this->GaussSigma = GaussSigma;
}

void FreqMap::setOrientedWindow(double width, double height)
{
    this->width = width;
    this->height = height;
}

FreqMap::FreqMap(QWidget *parent):QObject(parent) {

    connect(this, SIGNAL(allJobsDone()), this, SLOT(allJobsDoneSlot()));

}

void FreqMap::setParams(cv::Mat imgOriginal, cv::Mat orientMap, cv::Mat frequencyMap, int from, int to, int cols, int block) {
    this->imgOriginal = imgOriginal;
    this->orientMap = orientMap;
    this->freqMap = frequencyMap;
    this->from = from;
    this->to = to;
    this->cols = cols;
    this->block = block;
};

void FreqMap::calcFreq() {

    //oriented window size
    int orientWinHeight = this->height;
    int orientWinWidth = this->width;

    //main loop
    for (int yOffset = from + (block / 2); yOffset < to + (block / 2); yOffset = yOffset + block) {

        for (int xOffset = (block / 2); xOffset < cols + (block / 2); xOffset = xOffset + block) {

            cv::Mat img;

            //get degrees
            float radian = orientMap.row(yOffset).col(xOffset).at<float>();
            if (isnan(radian)) radian = 0;
            float degree = radian * (180.0/M_PI);

            //rotate
            cv::Size dsize(imgOriginal.cols, imgOriginal.rows);
            cv::Point center(imgOriginal.cols/2, imgOriginal.rows/2);
            cv::Mat m = cv::getRotationMatrix2D(center, degree, 1);
            cv::warpAffine(imgOriginal, img, m, dsize);

            //get new xy point
            int imgCenterX, imgCenterY;
            imgCenterX = imgOriginal.cols / 2;
            imgCenterY = imgOriginal.rows / 2;

            int xNew = round((xOffset - imgCenterX) * cos(radian) - (-yOffset + imgCenterY) * sin(radian) + imgCenterX);
            int yNew = round((xOffset - imgCenterX) * sin(radian) + (-yOffset + imgCenterY) * cos(radian) - imgCenterY) * -1;

            if (xNew < 0) {
                xNew = 0;
            }
            else if (xNew >= cols) {
                xNew = cols - 1;
            }

            if (yNew < 0) {
                yNew = 0;
            }
            else if (yNew >= imgOriginal.rows) {
                yNew = imgOriginal.rows - 1;
            }

            //avg of color values for every col; 17x33
            int array[orientWinWidth];
            int avg;
            int count = 0;
            for (int i = xNew - orientWinWidth/2 - 1; i <= orientWinWidth/2 - 1 + xNew; i++) {
                avg = 0;
                for (int j = yNew - orientWinHeight/2 - 1; j <= orientWinHeight/2 - 1 + yNew; j++) {
                    avg = avg + (int)img.at<uchar>(j,i);
                }
                array[count] = avg/orientWinHeight;
                count++;
            }

            //create vector for p1d library
            vector< float > data;

            for (int i = 0; i < orientWinWidth; i++) {
                data.push_back(array[i]);
            }

            //Run persistence on data
            p1d::Persistence1D p;
            p.RunPersistence(data);

            //Get all extrema with a persistence larger than 5
            vector< p1d::TPairedExtrema > Extrema;
            p.GetPairedExtrema(Extrema, 5);

            vector <int> maxima;
            for(vector< p1d::TPairedExtrema >::iterator it = Extrema.begin(); it != Extrema.end(); it++) {
                maxima.push_back((*it).MaxIndex);
            }

            sort(maxima.begin(), maxima.end(), std::greater<int>());

            int sum = 0;
            for (int i = 0; i < (int)maxima.size() - 1; i++) {
                sum = maxima[i] - maxima[i + 1] - 1 + sum;
            }

            double f = sum;
            if (maxima.size() > 1) {
                f = (((double)maxima.size() - 1.0) / (double)sum) * 255 * 2;
            }

            freqMap.rowRange(yOffset - block/2, yOffset + block/2 + 1).colRange(xOffset - block/2, xOffset + block/2 + 1).setTo(f);
        }
    }
    emit jobDone();
}

void FreqMap::jobDoneSlot()
{
    jobDoneCounter++;
    if (jobDoneCounter == count) {
        jobDoneCounter = 0;
        emit allJobsDone();
    }
}

void FreqMap::allJobsDoneSlot()
{
    cv::Mat afterBlur;
    cv::GaussianBlur(freqMap,afterBlur,cv::Size(this->GaussBlock,this->GaussBlock),this->GaussSigma);

    qDeleteAll(s.begin(),s.end());
    s.clear();
    t.clear();

    emit drawImage(afterBlur);
}

void FreqMap::run() {

    freqMap = cv::Mat(imgOriginal.rows, imgOriginal.cols, CV_8UC1, cv::Scalar(1));

    count = QThread::idealThreadCount();

    int rows = orientMap.rows;
    rows = rows - (rows % block);

    float thread_rows = (float)rows / (float)count;
    int offset = (int)(roundf(thread_rows / block) * block + 0.5);

    if (offset * (count - 1) > rows) {
        count = 1;
        offset = rows;
    }

    int cols = orientMap.cols;
    cols = cols - (cols % block);

    for (int i = 0; i < count; i++) {
        t.push_back(new QThread);
        s.push_back(new FreqMap);
        //s.last()->setParams(res, &freqMap, i * offset, (i+1) * offset, cols, block);
        if (i == count - 1) {
            s.last()->setParams(imgOriginal, orientMap, freqMap, i * offset, rows, cols, block);
            s.last()->setOrientedWindow(width, height);
        }
        else {
            s.last()->setParams(imgOriginal, orientMap, freqMap, i * offset, (i + 1) * offset, cols, block);
            s.last()->setOrientedWindow(width, height);
        }
        s.last()->moveToThread(t.last());
        connect(this,SIGNAL(startSum()),s.last(),SLOT(calcFreq()));
        connect(s.last(),SIGNAL(jobDone()), this, SLOT(jobDoneSlot()));
        t.last()->start();
    }

    emit startSum();

}
