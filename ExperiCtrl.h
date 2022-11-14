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
    ExperiCtrl(DevCtrl *dev, ExperiSetting *setting, ExperiData *data, CellImageAlogrithm *algo, QObject *parent = nullptr);
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
    void startExperiment(const QString &experiId);
    void experimentStateTransfer();
    void experiChamberStateTransfer();
    void experiPosStateTransfer();
    void experiCapStateTransfer();

    void experimentStateMachine();
    void experiChamberStateMachine();
    void experiPosStateMachine();
    void experiCapImageStateMachine();
    void pauseExperiment();

signals:
    void experimentFinished();
    void experiOneChamberFinished();
    void experiOnePosFinished();
    void experiCapFinished();
    void experimentInitOk();

private:
    DevCtrl *devCtrl;
    ExperiSetting *m_setting;
    ExperiData  *m_data;
    CellImageAlogrithm *m_algorithm;

    int m_experiState;
    int m_experiChamberState;
    int m_experiPosState;
    int m_experiCapState;

    int m_xPos;
    int m_yPos;
    int m_filterPos;
    int m_ledState;

    int m_chipPos_Y[CHAMBER_NUM];
    int m_chipPos_X[3];
    QString imgFilePath = "/cellImages/";
    QString imgName;
    QString imgMarkedName;
    Mat     imgFL1;
    void initExperiment(void);
    void getCellImages();
    int  getNextState(int currentState);
    int  getNextChamberPos(int state);
    void updateImageName();
};

#endif // EXPERICTRL_H
