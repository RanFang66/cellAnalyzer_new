#include "ExperiSetting.h"
#include <QDateTime>
#include <QVariant>
ExperiSetting::ExperiSetting(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);
    for (int i = 0; i < CHAMBER_NUM; i++) {
        m_chamberSelc[i] = 0;
    }
    m_chamberSet = 0;

    m_experiTypeID = 1;
    m_cellTypeID = 1;
    m_dilutionRatio = 0;
    m_sampleID = "Sample_" + QDateTime::currentDateTime().toString("MMddHHmm");
}

void ExperiSetting::initSetting(int userId, int experiTypeId)
{
    m_experiId = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    m_userID = userId;
    m_experiTypeID = experiTypeId;
    m_cellTypeID = 1;
    m_experiName = "Cell_" + m_experiId;
    m_sampleID = "Sample_" + m_experiId;
    m_dilutionRatio = 0;
    m_chamberSet = 0;
    for (int i = 0; i < CHAMBER_NUM; i++) {
        m_chamberSelc[i] = 0;
    }

    QString queryStr = QString("SELECT name FROM userInfo WHERE userId = %1").arg(m_userID);
    if (query->exec(queryStr) && query->next()) {
        m_userName = query->value(0).toString();
    } else {
        m_userName = "admin";
    }

    queryStr = QString("SELECT experiTypeName FROM experiType WHERE experiTypeID = %1").arg(m_experiTypeID);
    if (query->exec(queryStr) && query->next()) {
        m_experiType = query->value(0).toString();
    } else {
        m_experiType = "AOPI VIABILITY";
    }

    queryStr = QString("SELECT * FROM cellType WHERE cellTypeID = %1").arg(m_cellTypeID);
    if (query->exec(queryStr) && query->next()) {
        m_cellType = query->value(1).toString();
        m_minRadiu = query->value(2).toInt();
        m_maxRadiu = query->value(3).toInt();
    } else {
        m_cellType = "cell_2_to_15um";
        m_minRadiu = 15;
        m_maxRadiu = 30;
    }
}

void ExperiSetting::setCellTypeID(int newCellTypeID)
{
    m_cellTypeID = newCellTypeID + 1;
    QString queryStr = QString("SELECT * FROM cellType WHERE cellTypeID = %1").arg(m_cellTypeID);
    if (query->exec(queryStr) && query->next()) {
        m_cellType = query->value(1).toString();
        m_minRadiu = query->value(2).toInt();
        m_maxRadiu = query->value(3).toInt();
    } else {
        m_cellType = "cell_2_to_15um";
        m_minRadiu = 15;
        m_maxRadiu = 30;
    }
}


const QString &ExperiSetting::getUserName() const
{
    return m_userName;
}


