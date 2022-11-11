#include "CellImageAlogrithm.h"
#include <QDebug>
CellImageAlogrithm::CellImageAlogrithm(QObject *parent) : QObject(parent)
{

}

void CellImageAlogrithm::addBorder(Mat &img, int width, uchar value)
{
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < width; j++) {
            img.at<uchar>(i, j) = value;
            img.at<uchar>(i, img.cols-j) = value;
        }
    }

    for (int i = 0; i < img.cols; i++) {
        for (int j = 0; j < width; j++) {
            img.at<uchar>(j, i) = value;
            img.at<uchar>(img.rows-j-1, i) = value;
        }
    }
}

void CellImageAlogrithm::getBinaryImage(Mat &imgGray, Mat &imgBin, int method)
{
    switch (method) {
    case BINARY_OTSU:
    {
        int thresh = 128;
        thresh = threshold(imgGray, imgBin, thresh, 255, THRESH_BINARY | THRESH_OTSU);
        qDebug() << "--- Image Algorithm: OTSU threshold value: " << thresh;
        break;
    }
    case BINARY_ADAPTIVE:
    {
        adaptiveThreshold(imgGray, imgBin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 9, 1);
        qDebug() << "--- Image Algorithm: adaptive threshold method";
        break;
    }
    default:
        break;
    }
}

void CellImageAlogrithm::filterImage(Mat &imgSrc, Mat &imgDst, int method)
{
    switch (method) {
    case MEDIAN_FILTER:
    {
        medianBlur(imgSrc, imgDst, 3);
        break;
    }
    case GAUSSIAN_FILTER:
    {
        GaussianBlur(imgSrc, imgDst, Size(3, 3), 1);
        break;
    }
    case BILATERAL_FILTER:
    {
        bilateralFilter(imgSrc, imgDst, 3, 0.3, 0.4);
        break;
    }
    case MEAN_FILTER:
    {
        blur(imgSrc, imgDst, Size(3, 3));
        break;
    }
    default:
        break;
    }
}

void CellImageAlogrithm::enhanceContrast(Mat &imgSrc, Mat &imgDst)
{

}

int CellImageAlogrithm::markCellsInCluster(Mat &cluster, Mat &img, Point2f ltPoint, int radiu)
{
    int minRadiu = 2;
    int radiuLimit = 10;
    threshold(cluster, cluster, 128, 255, THRESH_BINARY | THRESH_TRIANGLE);
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(cluster, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
    std::vector<Point2f> centers;
    std::vector<float> radius;
    int cellNum = 0;
    double avgRadiu = 0;
    unsigned long contourNum = contours.size();
    for (unsigned long i = 0; i < contourNum; i++) {
        Point2f center;
        float radiu;
        minEnclosingCircle(contours[i], center, radiu);
        if (radiu > minRadiu && radiu < radiuLimit) {
            centers.push_back(center);
            radius.push_back(radiu);
            avgRadiu += radiu;
        }
    }
    avgRadiu /= radius.size();
    for (unsigned long i = 0; i < radius.size(); i++) {
        circle(img, centers[i]+ ltPoint, radius[i]+1, Scalar(0, 255, 255), 2, LINE_8, 0);
        cellNum++;
    }
    return cellNum;
}

int CellImageAlogrithm::markCells(Mat &img, Mat &imgMarked)
{
    Mat imgGray;
    Mat imgBin;
    unsigned long cellNum = 0;
    int cluseterCellNum = 0;
    int minRadiu = 10;
    int maxRadiu = 30;

    cvtColor(img, imgGray, COLOR_BGR2GRAY);

//    filterImage(imgGray, imgGray, MEDIAN_FILTER);
//    getBinaryImage(imgGray, imgBin, BINARY_OTSU);

//    addBorder(imgBin, 3, 255);

//    // get cell contours
//    std::vector<std::vector<Point>> contours;
//    std::vector<Vec4i> hierarchy;
//    findContours(imgBin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);


//    // fitting the contours with circles
//    std::vector<Point2f> centers;
//    std::vector<float> radius;
//    double avgRadiu = 0;
//    unsigned long contourNum = contours.size();
//    for (unsigned long i = 0; i < contourNum; i++) {
//        Point2f center;
//        float radiu;
//        minEnclosingCircle(contours[i], center, radiu);
//        if (hierarchy[i][3] == 0 && radiu > minRadiu && radiu < maxRadiu) {
//            centers.push_back(center);
//            radius.push_back(radiu);
//            avgRadiu += radiu;
//            cellNum++;
//            circle(imgMarked, center, radiu, Scalar(0, 0, 255), 2, LINE_8, 0);
//        }
//    }
//    avgRadiu /= radius.size();
//    qDebug() << "cell numbers: " << cellNum << ", average radiu: " << avgRadiu << endl;
//    filterImage(imgMarked, imgMarked, GAUSSIAN_FILTER);
    emit markCellsFinished();
    return cellNum;
}









