#include "ExperiData.h"
#include <QDateTime>
#include <QVariant>
#include <QDebug>
#include <QSqlError>


ExperiData::ExperiData(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);
}
//struct experiDataRes{
//    int cellConc;
//    int liveCellConc;
//    int deadCellConc;
//    double viability;
//    int totalCellNum;
//    int liveCellNum;
//    int deadCellNum;
//    int avgDiameter;
//    double avgCompactness;
//    double aggregateRate;
//};

void ExperiData::insertExperimentData(int settingId)
{
//    query->exec("SELECT * from experiData WHERE experiID = '-1'");
//    QSqlRecord mRec = query->record();
    QString experiID = QDateTime::currentDateTime().toString("yyMMddHHmmss");
    CellImageAlogrithm::experiDataRes *res = algorithm->getImageProcessResult();
    query->prepare("INSERT INTO experiData(experiID, experiName, userID, sampleID, experiTypeID,"
                   "dilutionRatio, cellTypeID, champerSet, cellConc, liveCellConc, deadCellConc, "
                   "viability, totalCellNum, liveCellNum, deadCellNum, avgDiameter, "
                   "avgCompactness, aggregateRate, startTime) "
                   " Values(:experiID, :experiName, :userID, :sampleID, :experiTypeID,"
                   ":dilutionRatio, :cellTypeID, :champerSet, :cellConc, :liveCellConc, :deadCellConc, "
                   ":viability, :totalCellNum, :liveCellNum, :deadCellNum, :avgDiameter"
                   ":avgCompactness, :aggregateRate, :startTime)");

    query->bindValue(":experiID", experiID);
    query->bindValue(":experiName", setting->experiName());
    query->bindValue(":userID", setting->userID());
    query->bindValue(":sampleID", setting->sampleId());
    query->bindValue(":experiTypeID", setting->experiTypeID());
    query->bindValue(":dilutionRatio", setting->dilutionRatio());
    query->bindValue(":cellTypeID", setting->cellTypeID());
    query->bindValue(":chamberSet", setting->chamberSet());
    query->bindValue(":cellConc", res->cellConc);
    query->bindValue(":liveCellConc", res->liveCellConc);
    query->bindValue(":deadCellConc", res->deadCellConc);
    query->bindValue(":viability", res->viability);
    query->bindValue(":totalCellNum", res->totalCellNum);
    query->bindValue(":livCellNum", res->liveCellNum);
    query->bindValue(":deadCellNum", res->deadCellNum);
    query->bindValue(":avgDiameter", res->avgDiameter);
    query->bindValue(":avgCompactness", res->avgCompactness);
    query->bindValue(":aggregateRate", res->aggregateRate);
    query->bindValue(":startTime", QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss"));

    if (!query->exec()) {
        qDebug() << "--- Database Error: insert record failed, " << query->lastError().text();
    }
}

void ExperiData::deleteExperimentData(int experiID)
{
    query->prepare("DELETE FROM experiData WHERE experiID = :ID");
    query->bindValue(":ID", experiID);
    if (!query->exec()) {
        qDebug() << "--- Database Error: delete record failed, " << query->lastError().text();
    }
}


