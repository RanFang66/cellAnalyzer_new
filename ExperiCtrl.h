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
    enum CHAMBER_ID {
        CHAMBER_1 = 1,
        CHAMBER_2,
        CHAMBER_3,
        CHAMBER_4,
        CHAMBER_5,
        CHAMBER_6,
        CHAMBER_FINISH = 99,
    };
public slots:
    void startExperiment();

signals:

private:
    DevCtrl *devCtrl;
    ExperiSetting *setting;
    ExperiData  *data;
    CellImageAlogrithm *alogrithm;

    int m_experiState;
    int m_experiChamberState;

    int m_chamberPos[CHAMBER_NUM];
    int m_capPos[3];

    void initExperiState(void);
    void getCellImages();
    void runToNewPos();
    void initChamber(int id);
    int  getNextState(int currentChamber);
    void experimentStateMachine();
    void experiChamberStateMachine();


};

#endif // EXPERICTRL_H
