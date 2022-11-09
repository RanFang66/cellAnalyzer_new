#ifndef EXPERISETTING_H
#define EXPERISETTING_H

#include <QObject>

#define CHAMBER_NUM     (5)
class ExperiSetting : public QObject
{
    Q_OBJECT
public:
    explicit ExperiSetting(QObject *parent = nullptr);

    const QString &experiName() const;
    void setExperiName(const QString &newExperiName);
    const QString &sampleId() const;
    void setSampleId(const QString &newSampleId);
    int  userID() const;
    void setUserID(int newUserId);
    int  experiTypeID() const;
    void setExperiTypeID(int newExperiTypeID);
    int  cellTypeID() const;
    void setCellTypeID(int newCellTypeID);
    int dilutionRatio() const;
    void setDilutionRatio(int newDilutionRatio);

    bool chamberIsEnable(int id);
    int  chamberSet(void) const;
    void setChamberEn(int id, bool checked);



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

    QString m_experiName;
    QString m_sampleID;
    int     m_userID;
    int     m_experiTypeID;
    int     m_cellTypeID;
    int     m_dilutionRatio;
    bool    m_chamberSelc[CHAMBER_NUM];
    int     m_chamberSet;
};

inline const QString &ExperiSetting::experiName() const
{
    return m_experiName;
}

inline void ExperiSetting::setExperiName(const QString &newExperiName)
{
    m_experiName = newExperiName;
}

inline const QString &ExperiSetting::sampleId() const
{
    return m_sampleID;
}

inline void ExperiSetting::setSampleId(const QString &newSampleId)
{
    m_sampleID = newSampleId;
}

inline int ExperiSetting::userID() const
{
    return m_userID;
}

inline void ExperiSetting::setUserID(int newUserId)
{
    m_userID = newUserId;
}

inline int ExperiSetting::experiTypeID() const
{
    return m_experiTypeID;
}

inline void ExperiSetting::setExperiTypeID(int newExperiTypeID)
{
    m_experiTypeID = newExperiTypeID;
}

inline int ExperiSetting::cellTypeID() const
{
    return m_cellTypeID;
}

inline void ExperiSetting::setCellTypeID(int newCellTypeID)
{
    m_cellTypeID = newCellTypeID;
}

inline int ExperiSetting::dilutionRatio() const
{
    return m_dilutionRatio;
}

inline void ExperiSetting::setDilutionRatio(int newDilutionRatio)
{
    m_dilutionRatio = newDilutionRatio;
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
    m_chamberSelc[id] = checked;
    if (checked)
        m_chamberSet = m_chamberSet | (1 << id);
    else
        m_chamberSet = m_chamberSet & (~(1 << id));

}

#endif // EXPERISETTING_H
