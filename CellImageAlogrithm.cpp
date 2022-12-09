#include "CellImageAlogrithm.h"
#include <QDebug>
CellImageAlogrithm::CellImageAlogrithm(QObject *parent) : QObject(parent)
{
    cellNum = 0;
    liveCellNum = 0;
    deadCellNum = 0;
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

void CellImageAlogrithm::getBinaryImage(Mat &imgGray, Mat &imgBin, int method)
{
    switch (method) {
    case BINARY_OTSU:
    {
        int thresh = 128;
        thresh = threshold(imgGray, imgBin, thresh, 255, THRESH_BINARY_INV | THRESH_OTSU);
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
        medianBlur(imgSrc, imgDst, 5);
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


using namespace std;
int CellImageAlogrithm::markCellsInCluster(Mat &cluster, Mat &imgMarked, Point ltPoint, int r)
{
//        vector<Vec3f> circles;
//        HoughCircles(temp, circles, HOUGH_GRADIENT, 1, HoughMinDist, cannyThres2,  HoughThres, HoughMinRadiu, HoughMaxRadiu);

//        double radiuSum = 0;
//        for (unsigned long i = 0; i < circles.size(); i++) {
//            radiuSum += circles[i][2];
//        }
//        double radiuMean = radiuSum / circles.size();

//        for (unsigned long i = 0; i < circles.size(); i++) {

//            if (radiuMean*1.5 > circles[i][2])
//                circle(imgMarked, Point(circles[i][0]+x, circles[i][1]+y), circles[i][2], Scalar(0, 255, 255), 2);
//        }
//        QString name = QString("(%1, %2)").arg(x).arg(y);
//        imshow(name.toStdString(), temp);

    //    QString name = "/home/ran/Documents/clusterCells/"+QString("cluster_(%1, %2)").arg(x).arg(y)+".jpg";
    //    imwrite(name.toStdString(), temp);
    //    cvtColor(temp, temp, COLOR_BGR2GRAY);


    //    double a = 0;



    //    for (int i = 1; i < temp.rows-1; i++) {
    //        for (int j = 1; j < temp.cols-1; j++) {
    //            Point maxLoc = findKernelMax(temp, i, j, 3, 3);
    //            if (maxLoc.y > temp.at<uchar>(i, j) + 18) {
    //                tempBin.at<uchar>(i, j) = 255;
    //            } else {
    //                tempBin.at<uchar>(i, j) = 0;
    //            }
    //        }
    //    }

        Mat tempBin;
        int a = threshold(cluster, tempBin, 130, 255, THRESH_BINARY | THRESH_TRIANGLE);
 //       qDebug() << "(" << x <<", " << y << ") threshold :" << a << endl;


        Mat kernel = getStructuringElement(MORPH_CROSS, Size(3, 3));
        erode(tempBin, tempBin, kernel);

//        QString name = QString("(%1, %2)").arg(x).arg(y);
//        imshow(name.toStdString(), tempBin);

    //    //fillHole(temp, temp);
        vector<vector<Point>>  contou;
        vector<Vec4i> hier;


        findContours(tempBin, contou, hier, RETR_TREE, CHAIN_APPROX_NONE);
        vector<Point2f> center(contou.size());
        vector<float> radius(contou.size());
        for (unsigned long i = 0; i < contou.size(); i++) {
            minEnclosingCircle(contou[i], center[i], radius[i]);

     //       qDebug() << "(" << center[i].x << ", " << center[i].y << ")" << endl;
        }

        for (unsigned long j = 0; j < contou.size(); j++) {
            if (radius[j] < minRadiu-1 && contourArea(contou[j]) > 4)
                circle(imgMarked, center[j] + Point2f(ltPoint), radius[j]+1, Scalar(0, 255, 255), 1);
        }

//    Mat labels, stats, centroids;
//    auto num_cells = connectedComponentsWithStats(cluster, labels, stats, centroids);

//    for (int i = 2; i < num_cells; i++) {
//  //      qDebug() << "Obeject" << i << "with pos: (" << centroids.at<Point2d>(i).x << ", "<< centroids.at<Point2d>(i).y << ")with area" << stats.at<int>(i, CC_STAT_AREA) << endl;
//        if (stats.at<int>(i, CC_STAT_AREA) > 9 && stats.at<int>(i, CC_STAT_AREA) < 50) {
//            int width = stats.at<int>(i, CC_STAT_WIDTH);
//            int height = stats.at<int>(i, CC_STAT_HEIGHT);
//            int radiu = (width + height) / 2 + 1;
//            circle(imgMarked, centroids.at<Point2d>(i)+ltPoint, radiu, Scalar(0, 255, 255), 1);
//        }
//    }
}

void CellImageAlogrithm::analyzeCellsBright(Mat &img, Mat &imgMarked)
{
    Mat imgGray;
    Mat imgBin;
    int cluseterCellNum = 0;

    cellNum = 0;
    avgRadiu = 0;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);

    filterImage(imgGray, imgGray, MEDIAN_FILTER);
    getBinaryImage(imgGray, imgBin, BINARY_OTSU);

//    addBorder(imgBin, 3, 0);

    // get cell contours
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(imgBin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);

    vector<vector<Point>> contours_poly(contours.size());
    vector<Point2f> centers(contours.size());
    vector<float> radius(contours.size());
    Scalar colorRed(0, 0, 255);
    Scalar colorBlue(255, 0, 0);
    Scalar colorPurple(255, 0, 255);
    double radiuSum = 0;
    int contourSize = contours.size();
    if (contourSize > 2000) {
        contourSize = 2000;
    }
    for (unsigned long i = 0; i < contourSize; i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
//        qDebug() << "(" << centers[i].x << ", " << centers[i].y << ")" ;
    }

    for (unsigned long i = 0; i < contourSize; i++) {
        if (radius[i] > minRadiu && radius[i] < maxRadiu) {
            circle(imgMarked, centers[i], (int)radius[i], colorRed, 2, LINE_8, 0);
            cellNum++;
            radiuSum += radius[i];

        } else if (radius[i] > maxRadiu && radius[i] < 4*maxRadiu) {
            circle(imgMarked, centers[i], (int)radius[i], colorPurple, 2, LINE_8, 0);
            int Roi_x = centers[i].x - radius[i];
            int Roi_y = centers[i].y - radius[i];

            int height = radius[i] * 2 + 3;
            int width = radius[i] * 2 + 3;
            if (width + Roi_x >= imgGray.cols) {
                width = imgGray.cols-1-Roi_x;
            }
            if (height + Roi_y >= imgGray.rows) {
                height = imgGray.rows-1-Roi_y;
            }

            if (Roi_x < 0 ||  Roi_y < 0 || width <= 0 || height <= 0)
                continue;

            Mat cluster = Mat(height, width, CV_8UC1);
            Point2d plt(Roi_x, Roi_y);
            imgGray(Rect(Roi_x, Roi_y, width, height)).copyTo(cluster);
            markCellsInCluster(cluster, imgMarked, plt, radius[i]);
        }
    }
    avgRadiu = radiuSum / cellNum;

//    unsigned long contourNum = contours.size();
//    for (unsigned long i = 0; i < contourNum; i++) {
//        if (hierarchy[i][3] != 0)
//            continue;
//        Point2f center;
//        float radiu;
//        minEnclosingCircle(contours[i], center, radiu);
//        if (radiu > minRadiu && radiu < maxRadiu) {
//            radiuStat[radiu]++;
//            centers.push_back(center);
//            radius.push_back(radiu);
//            avgRadiu += radiu;
//            cellNum++;
//            circle(imgMarked, center, radiu, Scalar(0, 0, 255), 2, LINE_8, 0);
//        }
//    }
//    avgRadiu /= radius.size();
    qDebug() << "cell numbers: " << cellNum << ", average radiu: " << avgRadiu;
    filterImage(imgMarked, imgMarked, GAUSSIAN_FILTER);
//    cellNum = 1030;
//    avgRadiu = 30;
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
    findContours(imgBin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
    liveCellNum = contours.size();
    if (liveCellNum > 2000) {
        liveCellNum = 2000;
    }
    for (int i = 0; i < liveCellNum; i++) {
        drawContours(imgMarked, contours, i, Scalar(255, 255, 255));
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
    findContours(imgBin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
    deadCellNum = contours.size();
    if (deadCellNum > 2000) {
        deadCellNum = 2000;
    }
    for (int i = 0; i < deadCellNum; i++) {
        drawContours(imgMarked, contours, i, Scalar(255, 255, 255));
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
    avgRadiu = 0;
    avgCompactness = 0;
    aggregateRate = 0;
}









