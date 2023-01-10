#include "ExperiSetting.h"
#include <QDateTime>
#include <QVariant>
ExperiSetting::ExperiSetting(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);

    initSetting(1, 1);

}

void ExperiSetting::initSetting(int userId, int experiTypeId)
{
    m_experiId = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    m_userID = userId;
    m_experiTypeID = experiTypeId;

    for (int i = 0; i < CHAMBER_NUM; i++) {
        m_experiName[i] = "Cell_" + m_experiId;
        m_sampleID[i] = QString("Chamber%1").arg(i+1);
        m_dilutionRatio[i] = 1;
        m_chamberSelc[i] = 0;
        m_cellTypeID[i] = 1;
    }
    m_chamberSet = 0;


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

    queryStr = QString("SELECT * FROM cellType WHERE cellTypeID = 1");
    if (query->exec(queryStr) && query->next()) {
        for (int i = 0; i < CHAMBER_NUM; i++) {
            m_cellType[i] = query->value(1).toString();
            m_minRadiu[i] = query->value(2).toInt();
            m_maxRadiu[i] = query->value(3).toInt();
        }
    } else {
        for (int i = 0; i < CHAMBER_NUM; i++) {
            m_cellType[i] = "cell_2_to_15um";
            m_minRadiu[i] = 15;
            m_maxRadiu[i] = 30;
        }
    }
}

void ExperiSetting::setCellTypeID(int chamberid, int newCellTypeID)
{
    int index = chamberid-1;
    m_cellTypeID[index] = newCellTypeID;
    QString queryStr = QString("SELECT * FROM cellType WHERE cellTypeID = %1").arg(m_cellTypeID[index]);
    if (query->exec(queryStr) && query->next()) {
        m_cellType[index] = query->value(1).toString();
        m_minRadiu[index] = query->value(2).toInt();
        m_maxRadiu[index] = query->value(3).toInt();
    } else {
        m_cellType[index] = "cell_2_to_15um";
        m_minRadiu[index] = 15;
        m_maxRadiu[index] = 30;
    }
}


const QString &ExperiSetting::getUserName() const
{
    return m_userName;
}


