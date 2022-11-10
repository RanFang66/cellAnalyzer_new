#include "ExperiCtrl.h"

using namespace DevCtrl;

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
    EXPERI_CHAMBER_INIT,
    EXPERI_POS1_BRIGHT,
    EXPERI_POS1_BLUE,
    EXPERI_POS1_GREEN,
    EXPERI_POS2_BRIGHT,
    EXPERI_POS2_BLUE,
    EXPERI_POS2_GREEN,
    EXPERI_POS3_BRIGHT,
    EXPERI_POS3_BLUE,
    EXPERI_POS3_GREEN,
    EXPERI_CHAMBER_FINISH,
};

ExperiCtrl::ExperiCtrl(QObject *parent) : QObject(parent)
{
//    if (setting->chamberIsEnable(CHAMBER_1)) {
//        devCtrl->motorRun(DevCtrl::CHIP_MOTOR_Y, DevCtrl::MOTOR_RUN_POS, m_chamberPos[CHAMBER_1]);
    //    } else if (setting->)
}

void ExperiCtrl::startExperiment()
{
    m_experiState = EXPERI_INIT;
    devCtrl->motorRun(DevCtrl::CHIP_MOTOR_X, DevCtrl::MOTOR_RUN_POS, m_capPos[0]);
    devCtrl->cameraAutoExplosure(true);
    devCtrl->cameraWhiteBalance();
}

void ExperiCtrl::initExperiState()
{
    m_experiState = getNextState(EXPERI_INIT);
    m_experiChamberState = EXPERI_CHAMBER_INIT;
}

void ExperiCtrl::initChamber(int id)
{
    devCtrl->motorRun(DevCtrl::CHIP_MOTOR_Y, DevCtrl::MOTOR_RUN_POS, m_capPos[id-1]); // Y channel run to chamber pos
}

int  ExperiCtrl::getNextState(int currentChamber)
{
    int chamberIdStart = currentChamber - 1;
    for (int i = chamberIdStart; i <= CHAMBER_6; i++) {
        if (setting->chamberIsEnable(i)) {
            return i + 2;
        }
    }
    return EXPERI_FINISH;
}

void ExperiCtrl::experimentStateMachine()
{
    switch (m_experiState) {
    case EXPERI_IDLE:
        break;
    case EXPERI_INIT:
        break;
    case EXPERI_CHAMBER1:
        break;
    case EXPERI_CHAMBER2:
        break;
    case EXPERI_CHAMBER3:
        break;
    case EXPERI_CHAMBER4:
        break;
    case EXPERI_CHAMBER5:
        break;
    case EXPERI_CHAMBER6:
        break;
    case EXPERI_FINISH:
        break;
    }
}

void ExperiCtrl::experiChamberStateMachine()
{
    switch(m_experiChamberState) {
    case EXPERI_CHAMBER_INIT:
        break;
    case EXPERI_POS1_BRIGHT:
        devCtrl->ledLigthOn(DevCtrl::LED_WHITE);
        devCtrl->motroRun(DevCtrl::FILTER_MOTOR, DevCtrl::MOTOR_RUN_POS, FILTER_BRIGHT_POS);
        break;
    case EXPERI_POS1_BLUE:
        break;
    case EXPERI_POS1_GREEN:
        break;
    case EXPERI_POS2_BRIGHT:

        break;
    case EXPERI_POS2_BLUE:
        break;
    case EXPERI_POS2_GREEN:
        break;
    case EXPERI_POS3_BRIGHT:
        break;
    case EXPERI_POS3_BLUE:
        break;
    case EXPERI_POS3_GREEN:
        break;
    case EXPERI_CHAMBER_FINISH:
        break;
    }
}
