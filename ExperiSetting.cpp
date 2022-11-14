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

    m_experiTypeID = 0;
    m_cellTypeID = 0;
    m_dilutionRatio = 0;
    m_sampleID = "Sample_" + QDateTime::currentDateTime().toString("MMddHHmm");
}

void ExperiSetting::initSetting(int userId, int experiTypeId)
{
    m_experiId = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    m_userID = userId;
    m_experiTypeID = experiTypeId;
    m_cellTypeID = 0;
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

}

const QString &ExperiSetting::getUserName() const
{
    return m_userName;
}


