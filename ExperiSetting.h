#ifndef EXPERISETTING_H
#define EXPERISETTING_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#define CHAMBER_NUM     (6)

enum EXPERI_TYPE{
    AOPI_VIABILITY = 0,
    TAIPAN_BLUE,
    OTHER_TYPE,
};





enum CELL_TYPE {
    CELL_5_TO_20,
    CELL_15_TO_30,
};

class ExperiSetting : public QObject
{
    Q_OBJECT
public:
    explicit ExperiSetting(QObject *parent = nullptr);
    void initSetting(int userId, int experiTypeId);
    const QString &experiName(int chamberid) const;
    void setExperiName(int chamberid, const QString &newExperiName);
    const QString &sampleId(int chamberid) const;
    void setSampleId(int chamberid, const QString &newSampleId);
    int  userID() const;
    void setUserID(int newUserId);
    int  experiTypeID() const;
    void setExperiTypeID(int newExperiTypeID);
    int  cellTypeID(int chamberid) const;
    void setCellTypeID(int chamberid, int newCellTypeID);
    int  dilutionRatio(int id) const;
    void setDilutionRatio(int id, int newDilutionRatio);
    const QString &getExperiId(void) const;

    bool chamberIsEnable(int id);
    int  chamberSet(void) const;
    void setChamberEn(int id, bool checked);
    void setUserName(const QString &userName);
    const QString &getUserName() const;
    const QString &getExperiType() const;
    const QString &getCellType(int chamberid) const;
    int  getCellMinRadiu(int chamberid) const;
    int  getCellMaxRadiu(int chamberid) const;

//    struct ExperiSettingDataType {
//        QString experiName;
//        QString sampleID;
//        int     userId;
//        int     experiType;
//        int     cellType;
//        int     dilutionRatio;
//        bool    chamberSelc[CHAMBER_NUM];
//    };

signals:

private:
//    struct ExperiSettingDataType settingData;
    QString m_experiId;
    QSqlDatabase db;
    QSqlQuery *query;
    QString m_experiType;
    QString m_userName;


    QString m_experiName[CHAMBER_NUM];
    QString m_sampleID[CHAMBER_NUM];
    QString m_cellType[CHAMBER_NUM];
    int     m_dilutionRatio[CHAMBER_NUM];
    int     m_cellTypeID[CHAMBER_NUM];
    int     m_minRadiu[CHAMBER_NUM];
    int     m_maxRadiu[CHAMBER_NUM];

    int     m_userID;
    int     m_experiTypeID;

    bool    m_chamberSelc[CHAMBER_NUM];
    int     m_chamberSet;

};

inline const QString &ExperiSetting::experiName(int chamberid) const
{
    return m_experiName[chamberid-1];
}

inline void ExperiSetting::setExperiName(int chamberid, const QString &newExperiName)
{
    m_experiName[chamberid-1] = newExperiName;
}

inline const QString &ExperiSetting::sampleId(int chamberid) const
{
    return m_sampleID[chamberid-1];
}

inline void ExperiSetting::setSampleId(int chamberid, const QString &newSampleId)
{
    m_sampleID[chamberid-1] = newSampleId;
}

inline int ExperiSetting::userID() const
{
    return m_userID;
}

inline void ExperiSetting::setUserID(int newUserId)
{
    m_userID = newUserId;
    QString queryStr = QString("SELECT name FROM userInfo WHERE userId = %1").arg(m_userID);
    if (query->exec(queryStr) && query->next()) {
        m_userName = query->value(0).toString();
    } else {
        m_userName = "admin";
    }
}

inline int ExperiSetting::experiTypeID() const
{
    return m_experiTypeID;
}

inline void ExperiSetting::setExperiTypeID(int newExperiTypeID)
{
    m_experiTypeID = newExperiTypeID;
}

inline int ExperiSetting::cellTypeID(int chamberid) const
{
    return m_cellTypeID[chamberid];
}


inline int ExperiSetting::dilutionRatio(int chamberid) const
{
    return m_dilutionRatio[chamberid-1];
}

inline void ExperiSetting::setDilutionRatio(int id, int newDilutionRatio)
{
    m_dilutionRatio[id-1] = newDilutionRatio;
}

inline const QString &ExperiSetting::getExperiId() const
{
    return m_experiId;
}

inline bool ExperiSetting::chamberIsEnable(int id)
{
    return m_chamberSelc[id];
}

inline int ExperiSetting::chamberSet() const
{
    return m_chamberSet;
}

inline void ExperiSetting::setChamberEn(int id, bool checked)
{
    id -= 1;
    m_chamberSelc[id] = checked;
    if (checked)
        m_chamberSet = m_chamberSet | (1 << id);
    else
        m_chamberSet = m_chamberSet & (~(1 << id));

}

inline void ExperiSetting::setUserName(const QString &userName)
{
    m_userName = userName;
}

inline const QString &ExperiSetting::getExperiType() const
{
    return m_experiType;
}

inline const QString &ExperiSetting::getCellType(int chamberid) const
{
    return m_cellType[chamberid-1];
}

inline int ExperiSetting::getCellMinRadiu(int chamberid) const
{
    return m_minRadiu[chamberid-1];
}

inline int ExperiSetting::getCellMaxRadiu(int chamberid) const
{
    return m_maxRadiu[chamberid-1];
}


#endif // EXPERISETTING_H
