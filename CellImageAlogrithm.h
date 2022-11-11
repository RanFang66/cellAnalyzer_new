#ifndef CELLIMAGEALOGRITHM_H
#define CELLIMAGEALOGRITHM_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

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

    struct experiDataRes{
        int cellConc;
        int liveCellConc;
        int deadCellConc;
        double viability;
        int totalCellNum;
        int liveCellNum;
        int deadCellNum;
        int avgDiameter;
        double avgCompactness;
        double aggregateRate;
    };

    void addBorder(Mat &img, int width, uchar value);
    void getBinaryImage(Mat &imgGray, Mat &imgBIn, int method);
    void filterImage(Mat &imgSrc, Mat &imgDst, int method);
    void enhanceContrast(Mat &imgSrc, Mat &imgDst);
    int  markCellsInCluster(Mat &cluster, Mat &img, Point2f ltPoint, int radius);
    int  markCells(Mat &img, Mat &imgMarked);
    struct experiDataRes *getImageProcessResult();

private:
    struct experiDataRes m_result;
//    int m_cellConc;
//    int m_liveCellConc;
//    int m_deadCellConc;
//    double m_viability;
//    int m_totalCellNum;
//    int m_liveCellNum;
//    int m_deadCellNum;
//    int m_avgDiameter;
//    double m_avgCompactness;
    //    double m_aggregateRate;
};

inline CellImageAlogrithm::experiDataRes *CellImageAlogrithm::getImageProcessResult()
{
    return &m_result;
}

#endif // CELLIMAGEALOGRITHM_H
