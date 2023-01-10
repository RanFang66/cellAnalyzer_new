#include "CellImageAlogrithm.h"
#include <QDebug>
CellImageAlogrithm::CellImageAlogrithm(QObject *parent) : QObject(parent)
{
    cellNum = 0;
    liveCellNum = 0;
    deadCellNum = 0;
    clusterCellNum = 0;
    avgRadiu = 0;
    avgCompactness = 0;
    aggregateRate = 0;

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

void CellImageAlogrithm::fillHoles(Mat &imgSrc, Mat &imgDst)
{
    Mat mask = Mat::zeros(imgSrc.rows+2, imgSrc.cols+2, CV_8UC1);
    Mat imgFlood = imgSrc.clone();
    floodFill(imgFlood, mask, cv::Point(0, 0), Scalar(255));
    bitwise_not(imgFlood, imgFlood);
    imgDst = (imgSrc | imgFlood);
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
    case BINARY_TRIANGLE:
    {
        int thresh = 128;
        thresh = threshold(imgGray, imgBin, thresh, 255, THRESH_BINARY | THRESH_TRIANGLE);
        qDebug() << "--- Image Algorithm: Triangle threshold value: " << thresh;
    }
    default:
        break;
    }
}

void CellImageAlogrithm::generateBinary(Mat &imgSrc, Mat &imgBin)
{
    Mat imgGray;

    if (imgSrc.channels() > 1)
        cvtColor(imgSrc, imgGray, COLOR_RGB2GRAY);
    else
        imgGray = imgSrc;

    filterImage(imgGray, imgGray, BLACK_HAT);
    getBinaryImage(imgGray, imgBin, BINARY_TRIANGLE);

//    namedWindow("BinSrc", WINDOW_KEEPRATIO);
//    imshow("BinSrc", imgBin);

    addBorder(imgBin, 3, 0);
    filterImage(imgBin, imgBin, MEDIAN_FILTER);
    fillHoles(imgBin, imgBin);
}

void CellImageAlogrithm::filterImage(Mat &imgSrc, Mat &imgDst, int method)
{
    switch (method) {
    case MEDIAN_FILTER:
    {
        medianBlur(imgSrc, imgDst, kernelSize);
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
    case TOP_HAT:
    {
        morphologyEx(imgSrc, imgDst, cv::MORPH_TOPHAT, m_kernel, Point(-1, -1), 1);
        break;
    }

    case BLACK_HAT:
    {
        morphologyEx(imgSrc, imgDst, cv::MORPH_BLACKHAT, m_kernel, Point(-1, -1), 1);
        break;
    }
    default:
        break;
    }
}

void CellImageAlogrithm::enhanceContrast(Mat &imgSrc, Mat &imgDst)
{

}


using namespace std;
int CellImageAlogrithm::markCellsInCluster(Mat &cluster, Mat &imgMarked, Point ltPoint, int r)
{
    Mat tempBin;
    int clusterCellNum = 0;
    int a = threshold(cluster, tempBin, 130, 255, THRESH_BINARY | THRESH_TRIANGLE);
//       qDebug() << "(" << x <<", " << y << ") threshold :" << a << endl;

    Mat kernel = getStructuringElement(MORPH_CROSS, Size(3, 3));
    erode(tempBin, tempBin, kernel);

//    //fillHole(temp, temp);
    vector<vector<Point>>  contou;
    vector<Vec4i> hier;


    findContours(tempBin, contou, hier, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    vector<Point2f> center(contou.size());
    vector<float> radius(contou.size());
    for (unsigned long i = 0; i < contou.size(); i++) {
        minEnclosingCircle(contou[i], center[i], radius[i]);
        if (radius[i] < maxRadiu -1 && radius[i] > minRadiu - 1) {
            clusterCellNum++;
        }
 //       qDebug() << "(" << center[i].x << ", " << center[i].y << ")" << endl;
    }

    if (clusterCellNum < 2)
        return clusterCellNum;

    for (unsigned long j = 0; j < contou.size(); j++) {
        if (radius[j] < maxRadiu - 1 && radius[j] > minRadiu - 1) {
            circle(imgMarked, center[j] + Point2f(ltPoint), radius[j]+1, Scalar(0, 255, 255), 2);
            clusterCellNum++;
        }
    }
    return clusterCellNum;
}

void CellImageAlogrithm::analyzeCellsBright(Mat &img, Mat &imgMarked)
{
    Mat imgGray;
    Mat imgBin;
    cellNum = 0;
    avgRadiu = 0;
    clusterCellNum = 0;

    // get gray image
    cvtColor(img, imgGray, COLOR_RGB2GRAY);

    // get binary image
    generateBinary(imgGray, imgBin);

//    namedWindow("Bin", WINDOW_KEEPRATIO);
//    imshow("Bin", imgBin);
    //addBorder(imgBin, 3, 0);

    // get cell contours
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    //findContours(imgBin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
    findContours(imgBin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);


    double radiuSum = 0;
    unsigned long contourSize = contours.size();
    Point2f center;
    float radiu;
    qDebug() << "Total contour number: " << contourSize;

    if (!contours.empty()) {
        for (unsigned long i = 0; i < contourSize; i++) {
            double area = contourArea(contours[i]);
            qDebug()<< "area[" << i << "] = " << area;
            if (area <= maxRadiu * maxRadiu * 2 && area > minRadiu * minRadiu * 2) {
                //drawContours(imgMarked, contours, i, Scalar(0, 0, 255));

                minEnclosingCircle(contours[i], center, radiu);
                circle(imgMarked, center, radiu, Scalar(0, 0, 255), 2);
                cellNum++;
                radiuSum += radiu;
                radiuStat.push_back((int)radiu);
            } else if (area > minRadiu * minRadiu){
                RotatedRect minRect = minAreaRect(contours[i]);
                Mat cluster;
                Rect rec = minRect.boundingRect();
                if (rec.br().x >= imgGray.cols || rec.br().y >= imgGray.rows || rec.tl().x < 0 || rec.tl().y < 0) {
                    continue;
                }
                //drawContours(imgMarked, contours, i, Scalar(255, 0, 255));
                cluster = imgGray(rec).clone();
                int ret = markCellsInCluster(cluster, imgMarked, rec.tl(), rec.height);
                if (ret > 1) {
                    minEnclosingCircle(contours[i], center, radiu);
                    circle(imgMarked, center, radiu, Scalar(255, 0, 255), 2);
                    clusterCellNum+=ret;
                } else if (ret == 1){
                    cellNum++;
                    //drawContours(imgMarked, contours, i, Scalar(0, 0, 255));
                    minEnclosingCircle(contours[i], center, radiu);
                    circle(imgMarked, center, radiu, Scalar(0, 0, 255), 2);
                    radiuSum += radiu;
                    radiuStat.push_back((int)radiu);
                }
            }
        }
    }
    avgRadiu = radiuSum / cellNum;
    cellNum += clusterCellNum;

    qDebug() << "cell numbers: " << cellNum << ", average radiu: " << avgRadiu;
    filterImage(imgMarked, imgMarked, GAUSSIAN_FILTER);
    emit markCellsFinished();
}

void CellImageAlogrithm::analyzeCellsFL1(Mat &img, Mat &imgMarked)
{
    Mat imgGray;
    Mat imgBin;

    liveCellNum = 0;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);

    filterImage(imgGray, imgGray, MEDIAN_FILTER);
    getBinaryImage(imgGray, imgBin, BINARY_OTSU);
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(imgBin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    unsigned contourSize = contours.size();

    for (unsigned i = 0; i < contourSize; i++) {
        double area = contourArea(contours[i]);
        if (area > minRadiu * minRadiu) {
            drawContours(imgMarked, contours, i, Scalar(255, 255, 255));
            liveCellNum++;
        }
    }
    emit markCellsFinished();

}

void CellImageAlogrithm::analyzeCellsFL2(Mat &img, Mat &imgMarked)
{
    Mat imgGray;
    Mat imgBin;

    deadCellNum = 0;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);

    filterImage(imgGray, imgGray, MEDIAN_FILTER);
    getBinaryImage(imgGray, imgBin, BINARY_OTSU);
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(imgBin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    unsigned contourSize = contours.size();

    for (unsigned i = 0; i < contourSize; i++) {
        double area = contourArea(contours[i]);
        if (area > minRadiu * minRadiu) {
            drawContours(imgMarked, contours, i, Scalar(255, 255, 255));
            deadCellNum++;
        }
    }
    emit markCellsFinished();
}

void CellImageAlogrithm::markCells(int imgType, Mat &img, Mat &imgMarked)
{
    switch (imgType) {
    case 1:
        analyzeCellsBright(img, imgMarked);
        break;
    case 2:
        analyzeCellsFL1(img, imgMarked);
        break;
    case 3:
        analyzeCellsFL2(img, imgMarked);
        break;
    default:
        break;
    }
}

void CellImageAlogrithm::initAlgorithm()
{
    cellNum = 0;
    liveCellNum = 0;
    deadCellNum = 0;
    clusterCellNum = 0;
    avgRadiu = 0;
    avgCompactness = 0;
    aggregateRate = 0;
}









