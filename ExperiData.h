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
    explicit ExperiData(QObject *parent = nullptr);

public slots:
    void insertExperimentData(int settingId);
    void deleteExperimentData(int experiID);


signals:

private:
    QSqlDatabase db;
    QSqlQuery *query;

    ExperiSetting *setting;
    CellImageAlogrithm *algorithm;
};

#endif // EXPERIDATA_H
