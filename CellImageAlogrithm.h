#ifndef CELLIMAGEALOGRITHM_H
#define CELLIMAGEALOGRITHM_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QMap>
using namespace cv;

class CellImageAlogrithm : public QObject
{
    Q_OBJECT
public:
    explicit CellImageAlogrithm(QObject *parent = nullptr);

signals:
    void markCellsFinished();

public:
    enum BINAYR_METHOD {
        BINARY_OTSU = 0,
        BINARY_ADAPTIVE,
        BINARY_STAT,
    };

    enum FILTER_METHOD {
        MEDIAN_FILTER = 0,
        GAUSSIAN_FILTER,
        BILATERAL_FILTER,
        MEAN_FILTER,
    };

//    struct experiDataRes{
//        int cellConc;
//        int liveCellConc;
//        int deadCellConc;
//        double viability;
//        int totalCellNum;
//        int liveCellNum;
//        int deadCellNum;
//        int avgDiameter;
//        double avgCompactness;
//        double aggregateRate;
//    };

    void addBorder(Mat &img, int width, uchar value);
    void getBinaryImage(Mat &imgGray, Mat &imgBIn, int method);
    void filterImage(Mat &imgSrc, Mat &imgDst, int method);
    void enhanceContrast(Mat &imgSrc, Mat &imgDst);
    int  markCellsInCluster(Mat &cluster, Mat &img, Point ltPoint, int radius);
    void analyzeCellsBright(Mat &img, Mat &imgMarked);
    void analyzeCellsFL1(Mat &img, Mat &imgMarked);
    void analyzeCellsFL2(Mat &img, Mat &imgMarked);
    void setCellParameters(int minR, int maxR);
//    struct experiDataRes *getImageProcessResult();
    int getCellNum() const;
    int getLiveCellNum() const;
    int getDeadCellNum() const;
    double getAvgRadiu() const;
    double getCompactness() const;
private:
//    struct experiDataRes m_result;
    int minRadiu = 10;
    int maxRadiu = 30;


    int     cellNum;
    int     deadCellNum;
    int     liveCellNum;
    int     avgRadiu;
    double  aggregateRate;
    double  avgCompactness;
    QMap<int, int> radiuStat;
};

inline void CellImageAlogrithm::setCellParameters(int minR, int maxR)
{
    minRadiu = minR*0.8+0.4;
    maxRadiu = maxR*0.8+0.4;
    for (int i = minR; i <= maxR; i++) {
        radiuStat[i] = 0;
    }
}

inline int CellImageAlogrithm::getCellNum() const
{
    return cellNum;
}

inline int CellImageAlogrithm::getLiveCellNum() const
{
    return liveCellNum;
}

inline int CellImageAlogrithm::getDeadCellNum() const
{
    return deadCellNum;
}

inline double CellImageAlogrithm::getAvgRadiu() const
{
    return avgRadiu;
}

inline double CellImageAlogrithm::getCompactness() const
{
    return avgCompactness;
}



//inline CellImageAlogrithm::experiDataRes *CellImageAlogrithm::getImageProcessResult()
//{
//    return &m_result;
//}

#endif // CELLIMAGEALOGRITHM_H
