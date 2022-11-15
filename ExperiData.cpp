#include "ExperiData.h"
#include <QDateTime>
#include <QVariant>
#include <QDebug>
#include <QSqlError>


ExperiData::ExperiData(ExperiSetting *setting, QObject *parent) : QObject(parent), m_setting(setting)
{
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);
    m_sampleVolume = 0.02;
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

void ExperiData::insertExperimentData()
{
//    query->exec("SELECT * from experiData WHERE experiID = '-1'");
//    QSqlRecord mRec = query->record();
//    QString experiID = QDateTime::currentDateTime().toString("yyMMddHHmmss");
//    CellImageAlogrithm::experiDataRes *res = algorithm->getImageProcessResult();
    query->prepare("INSERT INTO experiData(experiID, experiName, userName, experiType, cellType, "
                   "chamberSet, sampleID, dilutionRatio, cellConc, liveCellConc, deadCellConc, "
                   "viability, totalCellNum, liveCellNum, deadCellNum, avgDiameter, "
                   "avgCompactness, aggregateRate, endTime) "
                   " VALUES(:experiID, :experiName, :userName, :experiType, :cellType, :chamberSet, "
                   ":sampleID, :dilutionRatio, :cellConc, :liveCellConc, :deadCellConc, "
                   ":viability, :totalCellNum, :liveCellNum, :deadCellNum, :avgDiameter"
                   ":avgCompactness, :aggregateRate, :endTime)");

    query->bindValue(":experiID", m_setting->getExperiId());
    query->bindValue(":experiName", m_setting->experiName());
    query->bindValue(":userName", m_setting->getUserName());
    query->bindValue(":experiType", m_setting->getExperiType());
    query->bindValue(":cellType", m_setting->getCellType());
    query->bindValue(":chamberSet", m_setting->chamberSet());
    query->bindValue(":sampleID", m_setting->sampleId());
    query->bindValue(":dilutionRatio", m_setting->dilutionRatio());
    query->bindValue(":cellConc", m_cellConc);
    query->bindValue(":liveCellConc", m_liveCellConc);
    query->bindValue(":deadCellConc", m_deadCellConc);
    query->bindValue(":viability", m_viability);
    query->bindValue(":totalCellNum", m_cellNum);
    query->bindValue(":livCellNum", m_liveCellNum);
    query->bindValue(":deadCellNum", m_deadCellNum);
    query->bindValue(":avgDiameter", m_avgDiameter);
    query->bindValue(":avgCompactness", m_avgCompactness);
    query->bindValue(":aggregateRate", m_aggregateRate);
    query->bindValue(":endTime", QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss"));


    if (!query->exec()) {
        qDebug() << "--- Database Error: insert record failed, " << query->lastError().text();
        qDebug() << query->lastQuery();
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

void ExperiData::updateData(int cellNum, int liveCellNum, int deadCellNum, double avgDiameter, double avgCompact)
{
    m_cellNum = cellNum;
    m_liveCellNum = liveCellNum;
    m_deadCellNum = deadCellNum;
    m_avgDiameter = avgDiameter;
    m_avgCompactness = avgCompact;
    m_viability = (float)m_liveCellConc / m_cellNum;
    m_liveCellConc = m_liveCellNum / m_sampleVolume;
    m_deadCellConc = m_deadCellNum / m_sampleVolume;
    m_cellConc = m_cellNum / m_sampleVolume;
    m_aggregateRate = 5;
    insertExperimentData();
}


