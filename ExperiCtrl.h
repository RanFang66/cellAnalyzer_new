#ifndef EXPERICTRL_H
#define EXPERICTRL_H

#include <QObject>
#include <DevCtrl.h>
#include <CellImageAlogrithm.h>
#include <ExperiSetting.h>
#include <ExperiData.h>



class ExperiCtrl : public QObject
{
    Q_OBJECT
public:
    explicit ExperiCtrl(QObject *parent = nullptr);

public slots:
    void startExperiment();

signals:

private:
    DevCtrl *devCtrl;
    ExperiSetting *setting;
    ExperiData  *data;


};

#endif // EXPERICTRL_H
