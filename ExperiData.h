#ifndef EXPERIDATA_H
#define EXPERIDATA_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <ExperiSetting.h>
#include <CellImageAlogrithm.h>


class ExperiData : public QObject
{
    Q_OBJECT
public:
    explicit ExperiData(ExperiSetting *setting, QObject *parent = nullptr);

public slots:
    void insertExperimentData();
    void deleteExperimentData(int experiID);
    void updateData(int cellNum, int liveCellNum,
                    int deadCellNum, double avgDiameter, double avgCompact);

signals:

private:
    QSqlDatabase db;
    QSqlQuery *query;

    ExperiSetting *m_setting;
    CellImageAlogrithm *algorithm;

    int m_cellNum;
    int m_liveCellNum;
    int m_deadCellNum;
    double m_cellConc;
    double m_liveCellConc;
    double m_deadCellConc;
    double m_avgDiameter;
    double m_avgCompactness;
    double m_aggregateRate;
    double m_viability;
    double m_sampleVolume;
};

#endif // EXPERIDATA_H
