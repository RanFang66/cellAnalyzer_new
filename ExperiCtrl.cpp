#include "ExperiCtrl.h"


enum EXPERI_GENERA_STATE {
    EXPERI_IDLE = 0,
    EXPERI_INIT,
    EXPERI_CHAMBER1,
    EXPERI_CHAMBER2,
    EXPERI_CHAMBER3,
    EXPERI_CHAMBER4,
    EXPERI_CHAMBER5,
    EXPERI_CHAMBER6,
    EXPERI_FINISH,
};

enum EXPERI_CHAMBER_STATE {
    EXPERI_CHAMBER_IDLE = 0,
    EXPERI_CHAMBER_INIT,
    EXPERI_CHAMBER_POS1,
    EXPERI_CHAMBER_POS2,
    EXPERI_CHAMBER_POS3,
    EXPERI_CHAMBER_FINISH,
};

enum EPXERI_POS_STATE {
    EXPERI_POS_IDLE = 0,
    EXPERI_POS_INIT,
    EXPERI_POS_BRIGHT,
    EXPERI_POS_BLUE,
    EXPERI_POS_GREEN,
    EXPERI_POS_FINISH,
};

enum POS_ID {
    POS_1 = 1,
    POS_2,
    POS_3,
};


enum EXPERI_CAP_STATE {
    CAP_IDLE = 0,
    CAP_INIT,
    CAP_SNAP,
    CAP_PROCESS,
    CAP_FINISH,
};

#define FILTER_BRIGHT_POS   0
#define FILTER_BLUE_POS     133
#define FILTER_GREEN_POS    266

ExperiCtrl::ExperiCtrl(QObject *parent) : QObject(parent)
{
    m_experiState = EXPERI_IDLE;
    m_experiChamberState = EXPERI_CHAMBER_IDLE;
    m_experiPosState = EXPERI_POS_IDLE;
    m_experiCapState = CAP_IDLE;

    connect(devCtrl, SIGNAL(chipYMotorStateUpdated()), this, SLOT(experiChamberStateTransfer()));
    connect(devCtrl, SIGNAL(chipXMotorStateUpdated()), this, SLOT(experiPosStateTransfer()));
    connect(devCtrl, SIGNAL(filterMotorStateUpdated()), this, SLOT(experiCapStateTransfer()));
    connect(devCtrl, SIGNAL(imageUpdated()), this, SLOT(experiCapStateTransfer()));

    connect(this, SIGNAL(experimentInitOk()), this, SLOT(experimentStateMachine()));
    connect(this, SIGNAL(experiCapFinished()), this, SLOT(experiPosStateTransfer()));
    connect(this, SIGNAL(experiOnePosFinished()), this, SLOT(experiChamberStateTransfer()));
    connect(this, SIGNAL(experiOneChamberFinished()), this, SLOT(experimentStateTransfer()));

}

ExperiCtrl::ExperiCtrl(DevCtrl *dev, ExperiSetting *setting, ExperiData *data, CellImageAlogrithm *algo, QObject *parent)
    : devCtrl(dev), m_data(data), m_setting(setting), m_algorithm(algo), QObject(parent)
{
    m_experiState = EXPERI_IDLE;
    m_experiChamberState = EXPERI_CHAMBER_IDLE;
    m_experiPosState = EXPERI_POS_IDLE;
    m_experiCapState = CAP_IDLE;

    connect(devCtrl, SIGNAL(chipYMotorStateUpdated()), this, SLOT(experiChamberStateTransfer()));
    connect(devCtrl, SIGNAL(chipXMotorStateUpdated()), this, SLOT(experiPosStateTransfer()));
    connect(devCtrl, SIGNAL(filterMotorStateUpdated()), this, SLOT(experiCapStateTransfer()));
    connect(devCtrl, SIGNAL(imageUpdated()), this, SLOT(experiCapStateTransfer()));

    connect(this, SIGNAL(experimentInitOk()), this, SLOT(experimentStateMachine()));
    connect(this, SIGNAL(experiCapFinished()), this, SLOT(experiPosStateTransfer()));
    connect(this, SIGNAL(experiOnePosFinished()), this, SLOT(experiChamberStateTransfer()));
    connect(this, SIGNAL(experiOneChamberFinished()), this, SLOT(experimentStateTransfer()));
}

void ExperiCtrl::startExperiment()
{
    m_experiState = EXPERI_INIT;
    experimentStateMachine();
}

void ExperiCtrl::experimentStateTransfer()
{
    int nextState = getNextState(m_experiState);
    m_experiState = nextState;
    m_yPos = getNextChamberPos(nextState);
    m_experiChamberState = EXPERI_CHAMBER_INIT;
}

void ExperiCtrl::experiChamberStateTransfer()
{
    switch(m_experiChamberState) {
    case EXPERI_CHAMBER_IDLE:
        break;
    case EXPERI_CHAMBER_INIT:
        if (devCtrl->chipPos_Y() == m_yPos) {
            m_experiChamberState = EXPERI_CHAMBER_POS1;
            m_xPos = m_chipPos_X[0];
            m_experiPosState = EXPERI_POS_INIT;
        }
        break;
    case EXPERI_CHAMBER_POS1:
        m_experiChamberState = EXPERI_CHAMBER_POS2;
        m_xPos = m_chipPos_X[1];
        m_experiPosState = EXPERI_POS_INIT;

        break;
    case EXPERI_CHAMBER_POS2:
        m_experiChamberState = EXPERI_CHAMBER_POS3;
        m_xPos = m_chipPos_X[2];
        m_experiPosState = EXPERI_POS_INIT;
        break;
    case EXPERI_CHAMBER_POS3:
        m_experiChamberState = EXPERI_CHAMBER_FINISH;
        m_xPos = m_chipPos_X[0];
        m_experiPosState = EXPERI_POS_IDLE;
        break;
    case EXPERI_CHAMBER_FINISH:
        break;
    }
}

void ExperiCtrl::experiPosStateTransfer()
{
    switch(m_experiPosState) {
    case EXPERI_POS_IDLE:
        break;
    case EXPERI_POS_INIT:
        if (devCtrl->chipPos_X() == m_xPos) {
            m_experiPosState = EXPERI_POS_BRIGHT;
            m_filterPos = FILTER_BRIGHT_POS;
            m_ledState = DevCtrl::LED_WHITE;
            m_experiCapState = CAP_INIT;
        }
        break;
    case EXPERI_POS_BRIGHT:
        m_experiPosState = EXPERI_POS_BLUE;
        m_filterPos = FILTER_BLUE_POS;
        m_ledState = DevCtrl::LED_BLUE;
        m_experiCapState = CAP_INIT;
        break;
    case EXPERI_POS_BLUE:
        m_experiPosState = EXPERI_POS_GREEN;
        m_filterPos = FILTER_GREEN_POS;
        m_ledState = DevCtrl::LED_GREEN;
        m_experiCapState = CAP_INIT;
        break;
    case EXPERI_POS_GREEN:
        m_experiPosState = EXPERI_POS_FINISH;
        m_filterPos = FILTER_BRIGHT_POS;
        m_ledState = DevCtrl::LED_OFF;
        m_experiCapState = CAP_IDLE;
        break;
    case EXPERI_POS_FINISH:
        m_experiPosState = EXPERI_POS_IDLE;
        break;
    }
}

void ExperiCtrl::experiCapStateTransfer()
{
    switch (m_experiCapState) {
    case CAP_IDLE:
        break;
    case CAP_INIT:
        if (devCtrl->filterPos() == m_filterPos) {
            m_experiCapState = CAP_SNAP;
        }
        break;
    case CAP_SNAP:
        m_experiCapState = CAP_PROCESS;
        break;
    case CAP_PROCESS:
        m_experiCapState = CAP_FINISH;
        break;
    case CAP_FINISH:
        m_experiCapState = CAP_IDLE;
        break;
    }
}


void ExperiCtrl::initExperiment()
{
    m_experiState = getNextState(EXPERI_INIT);
    m_yPos = getNextChamberPos(m_experiState);
    m_experiChamberState = EXPERI_CHAMBER_INIT;
    emit experimentInitOk();
}

void ExperiCtrl::experimentStateMachine()       // ctrl the whole experiment: ctrl the Y pos motion
{
    switch (m_experiState) {
    case EXPERI_IDLE:
        break;
    case EXPERI_INIT:
        initExperiment();
        break;
    case EXPERI_CHAMBER1:
        experiChamberStateMachine();
        break;
    case EXPERI_CHAMBER2:
        experiChamberStateMachine();
        break;
    case EXPERI_CHAMBER3:
        experiChamberStateMachine();
        break;
    case EXPERI_CHAMBER4:
        experiChamberStateMachine();
        break;
    case EXPERI_CHAMBER5:
        experiChamberStateMachine();
        break;
    case EXPERI_CHAMBER6:
        experiChamberStateMachine();
        break;
    case EXPERI_FINISH:
        emit experimentFinished();
        break;
    }
}


void ExperiCtrl::experiChamberStateMachine()    // ctrl the experiment in one chamber: ctrl the X pos motion
{

    switch(m_experiChamberState) {
    case EXPERI_CHAMBER_IDLE:
        break;
    case EXPERI_CHAMBER_INIT:
        devCtrl->motorRun(DevCtrl::CHIP_MOTOR_Y, DevCtrl::MOTOR_RUN_POS, m_yPos);
        break;
    case EXPERI_CHAMBER_POS1:
        experiPosStateMachine();
        break;
    case EXPERI_CHAMBER_POS2:
        experiPosStateMachine();
        break;
    case EXPERI_CHAMBER_POS3:
        experiPosStateMachine();
        break;
    case EXPERI_CHAMBER_FINISH:
        emit experiOneChamberFinished();
        break;
    }
}

void ExperiCtrl::experiPosStateMachine()        // ctrl the experiment in one pos: ctrl the filter motor pos
{
    switch(m_experiPosState) {
    case EXPERI_POS_IDLE:
        break;
    case EXPERI_POS_INIT:
        devCtrl->motorRun(DevCtrl::CHIP_MOTOR_X, DevCtrl::MOTOR_RUN_POS, m_xPos);
        break;
    case EXPERI_POS_BRIGHT:
        experiCapImageStateMachine();
        break;
    case EXPERI_POS_BLUE:
        experiCapImageStateMachine();
        break;
    case EXPERI_POS_GREEN:
        experiCapImageStateMachine();
        break;
    case EXPERI_POS_FINISH:
        emit experiOnePosFinished();
        break;
    }
}



void ExperiCtrl::experiCapImageStateMachine()
{
    switch (m_experiCapState) {
    case CAP_IDLE:
        break;
    case CAP_INIT:
        devCtrl->motorRun(DevCtrl::FILTER_MOTOR, DevCtrl::MOTOR_RUN_POS, m_filterPos);
        devCtrl->ledLigthOn(DevCtrl::LED_WHITE);
        break;
    case CAP_SNAP:
        devCtrl->cameraAutoExplosure(true);
        devCtrl->cameraWhiteBalance();
        emit devCtrl->capImage();
        break;
    case CAP_PROCESS:
    {
        Mat img = devCtrl->getCVImage();
        Mat imgMarked;
        m_algorithm->markCells(img, imgMarked);
        break;
    }
    case CAP_FINISH:
        emit experiCapFinished();
        break;
    }
}

int  ExperiCtrl::getNextState(int currentChamber)
{
    int chamberIdStart = currentChamber - 1;
    for (int i = chamberIdStart; i <= CHAMBER_6; i++) {
        if (m_setting->chamberIsEnable(i)) {
            return i + 2;
        }
    }
    return EXPERI_FINISH;
}


int ExperiCtrl::getNextChamberPos(int state)
{
    return m_chipPos_Y[state-2];
}


void ExperiCtrl::pauseExperiment()
{
    m_experiState = EXPERI_IDLE;
    m_experiChamberState = EXPERI_CHAMBER_IDLE;
    m_experiPosState = EXPERI_POS_IDLE;
}
