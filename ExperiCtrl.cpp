#include "ExperiCtrl.h"
#include <QDir>

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
enum VIEW_ID {
    VIEW_1 = 1,
    VIEW_2,
    VIEW_3,
};

enum IMAGE_TYPE_ID {
    IMAGE_BRIGHT = 1,
    IMAGE_FL1,
    IMAGE_FL2,
};

enum EXPERI_CAP_STATE {
    CAP_IDLE = 0,
    CAP_INIT,
    CAP_AUTOFOCUS,
    CAP_SNAP,
    CAP_PROCESS,
    CAP_FINISH,
};

const QStringList ImageType= {
    "BR",
    "FL1",
    "FL2",
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

    m_chamberId = 1;
    m_viewId = 1;
    m_imageTypeId = 0;

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

    m_chamberId = 1;
    m_viewId = 1;
    m_imageTypeId = 0;

    m_chipPos_Y[0] = 270;
    m_chipPos_Y[1] = 2241;
    m_chipPos_Y[2] = 4080;
    m_chipPos_Y[3] = 5907;
    m_chipPos_Y[4] = 7802;
    m_chipPos_Y[5] = 9767;


    m_chipPos_X[0] = 1300;
    m_chipPos_X[1] = 1900;
    m_chipPos_X[2] = 2500;
    connect(devCtrl, SIGNAL(chipYMotorStateUpdated()), this, SLOT(experiChamberStateTransfer()));
    connect(devCtrl, SIGNAL(chipXMotorStateUpdated()), this, SLOT(experiPosStateTransfer()));
    connect(devCtrl, SIGNAL(filterMotorStateUpdated()), this, SLOT(experiCapStateTransfer()));
    connect(devCtrl, SIGNAL(imageUpdated()), this, SLOT(experiCapStateTransfer()));
    connect(devCtrl, SIGNAL(autoFocusComplete()), this, SLOT(experiCapStateTransfer()));
    connect(m_algorithm, SIGNAL(markCellsFinished()), this, SLOT(experiCapStateTransfer()));
    connect(this, SIGNAL(experimentInitOk()), this, SLOT(experimentStateMachine()));
    connect(this, SIGNAL(experiCapFinished()), this, SLOT(experiPosStateTransfer()));
    connect(this, SIGNAL(experiOnePosFinished()), this, SLOT(experiChamberStateTransfer()));
    connect(this, SIGNAL(experiOneChamberFinished()), this, SLOT(experimentStateTransfer()));


}

void ExperiCtrl::startExperiment(const QString &experiId)
{
    imgFilePath = "/cellImages/" + experiId + "/";
    QDir dir(imgFilePath);
    if (!dir.exists()) {
        dir.mkdir(imgFilePath);
    }
    m_algorithm->setCellParameters(m_setting->getCellMinRadiu(), m_setting->getCellMaxRadiu());
    for (int i = 0; i < VIEW_3; i++) {
        m_cellNum[i] = 0;
        m_liveCellNum[i] = 0;
        m_deadCellNum[i] = 0;
        m_avgRadiu[i] = 0;
        m_avgCompact[i] = 0;
    }
    m_experiState = EXPERI_INIT;
    experimentStateMachine();
}

void ExperiCtrl::experimentStateTransfer()
{
    int nextState = getNextState(m_experiState);
    m_experiState = nextState;
    m_yPos = getNextChamberPos(nextState);
    m_experiChamberState = EXPERI_CHAMBER_INIT;
    experimentStateMachine();
}

void ExperiCtrl::experiChamberStateTransfer()
{
    switch(m_experiChamberState) {
    case EXPERI_CHAMBER_IDLE:
        break;
    case EXPERI_CHAMBER_INIT:
        if (devCtrl->chipPos_Y() == m_yPos) {
            m_experiChamberState = EXPERI_CHAMBER_POS1;
            m_viewId = VIEW_1;
            m_xPos = m_chipPos_X[0];
            m_experiPosState = EXPERI_POS_INIT;
        }
        break;
    case EXPERI_CHAMBER_POS1:
        m_experiChamberState = EXPERI_CHAMBER_POS2;
        m_viewId = VIEW_2;
        m_xPos = m_chipPos_X[1];
        m_experiPosState = EXPERI_POS_INIT;

        break;
    case EXPERI_CHAMBER_POS2:
        m_experiChamberState = EXPERI_CHAMBER_POS3;
        m_viewId = VIEW_3;
        m_xPos = m_chipPos_X[2];
        m_experiPosState = EXPERI_POS_INIT;
        break;
    case EXPERI_CHAMBER_POS3:
        m_experiChamberState = EXPERI_CHAMBER_FINISH;
        m_viewId = VIEW_1;
        m_xPos = m_chipPos_X[0];
        m_experiPosState = EXPERI_POS_IDLE;
        break;
    case EXPERI_CHAMBER_FINISH:
        break;
    }
    experimentStateMachine();
}

void ExperiCtrl::experiPosStateTransfer()
{
    switch(m_experiPosState) {
    case EXPERI_POS_IDLE:
        break;
    case EXPERI_POS_INIT:
        if (devCtrl->chipPos_X() == m_xPos) {
            m_experiPosState = EXPERI_POS_BRIGHT;
            m_imageTypeId = IMAGE_BRIGHT;
            m_filterPos = FILTER_BRIGHT_POS;
            m_ledState = DevCtrl::LED_WHITE;
            m_experiCapState = CAP_INIT;
        }
        break;
    case EXPERI_POS_BRIGHT:
        saveImages(imgBR, imgMarked);
        m_experiPosState = EXPERI_POS_BLUE;
        m_imageTypeId = IMAGE_FL1;
        m_filterPos = FILTER_BLUE_POS;
        m_ledState = DevCtrl::LED_BLUE;
        m_experiCapState = CAP_INIT;
        break;
    case EXPERI_POS_BLUE:
        saveImages(imgFL1, imgMarked);
        m_experiPosState = EXPERI_POS_GREEN;
        m_imageTypeId = IMAGE_FL2;
        m_filterPos = FILTER_GREEN_POS;
        m_ledState = DevCtrl::LED_GREEN;
        m_experiCapState = CAP_INIT;
        break;
    case EXPERI_POS_GREEN:
        saveImages(imgFL2, imgMarked);
        saveFLImage();
        m_experiPosState = EXPERI_POS_FINISH;
        m_imageTypeId = IMAGE_BRIGHT;
        m_filterPos = FILTER_BRIGHT_POS;
        m_ledState = DevCtrl::LED_OFF;
        m_experiCapState = CAP_IDLE;
        break;
    case EXPERI_POS_FINISH:
        m_experiPosState = EXPERI_POS_IDLE;
        break;
    }
    experimentStateMachine();
}

void ExperiCtrl::experiCapStateTransfer()
{
    switch (m_experiCapState) {
    case CAP_IDLE:
        break;
    case CAP_INIT:
//        if (devCtrl->filterPos() == m_filterPos) {
            m_experiCapState = CAP_AUTOFOCUS;
//        }
        break;
    case CAP_AUTOFOCUS:
        m_experiCapState = CAP_SNAP;
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
    experimentStateMachine();
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
        calcAnalyzeResult();
        emit experimentFinished();
        m_experiState = EXPERI_IDLE;
        m_experiChamberState = EXPERI_CHAMBER_IDLE;
        m_experiPosState = EXPERI_POS_IDLE;
        m_experiCapState = CAP_IDLE;
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
        getAnalyzeResult();
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
        devCtrl->ledLigthOn(m_ledState);
//        devCtrl->motorRun(DevCtrl::FILTER_MOTOR, DevCtrl::MOTOR_RUN_POS, m_filterPos);
        break;
    case CAP_AUTOFOCUS:
        if (m_imageTypeId == IMAGE_BRIGHT) {
            devCtrl->cameraWhiteBalance();
        }
        devCtrl->startAutoFocus(true);
        break;
    case CAP_SNAP:
        devCtrl->camSnap();
        break;
    case CAP_PROCESS:
    {
        switch (m_imageTypeId) {
        case IMAGE_BRIGHT:
            imgBR = devCtrl->getCVImage();
            imgMarked = imgBR.clone();
            m_algorithm->analyzeCellsBright(imgBR, imgMarked);
//            saveImages(imgBR, imgMarked);
            break;
        case IMAGE_FL1:
            imgFL1 = devCtrl->getCVImage();
            imgMarked = imgFL1.clone();
            m_algorithm->analyzeCellsFL1(imgFL1, imgMarked);
//            saveImages(imgFL1, imgMarked);
            break;
        case IMAGE_FL2:
            imgFL2 = devCtrl->getCVImage();
            imgMarked = imgFL2.clone();
            m_algorithm->analyzeCellsFL2(imgFL2, imgMarked);
//            saveImages(imgFL2, imgMarked);
//            saveFLImage();
            break;
        default:
            break;
        }
        break;
    }
    case CAP_FINISH:
        devCtrl->ledLigthOn(DevCtrl::LED_OFF);
        emit experiCapFinished();
        break;
    }
}

int  ExperiCtrl::getNextState(int currentState)
{
    int chamberIdStart = currentState - 1;
    for (int i = chamberIdStart; i < CHAMBER_6; i++) {
        if (m_setting->chamberIsEnable(i)) {
            m_chamberId = i+1;
            return i + 2;
        }
    }
    return EXPERI_FINISH;
}


int ExperiCtrl::getNextChamberPos(int state)
{
    return m_chipPos_Y[state-2];
}


void ExperiCtrl::saveImages(Mat &img, Mat &imgMarked)
{
    QString imgName = imgFilePath + QString("chamber%1_%2_%3.jpg")
            .arg(m_chamberId)
            .arg(m_viewId)
            .arg(ImageType.at(m_imageTypeId-1));
    QString imgMarkedName = imgFilePath + QString("chamber%1_%2_%3_Marked.jpg")
            .arg(m_chamberId)
            .arg(m_viewId)
            .arg(ImageType.at(m_imageTypeId-1));
    imwrite(imgName.toStdString(), img);
    imwrite(imgMarkedName.toStdString(), imgMarked);
}

void ExperiCtrl::saveFLImage()
{
    QString name = imgFilePath +QString("chamber%1_%2_FL1_FL2.jpg")
            .arg(m_chamberId)
            .arg(m_viewId);
    Mat   img = imgFL1 + imgFL2;
    imwrite(name.toStdString(), img);
}

void ExperiCtrl::getAnalyzeResult()
{
    m_cellNum[m_viewId-1] = m_algorithm->getCellNum();
    m_liveCellNum[m_viewId-1] = m_algorithm->getLiveCellNum();
    m_deadCellNum[m_viewId-1] = m_algorithm->getDeadCellNum();
    m_avgRadiu[m_viewId-1] = m_algorithm->getAvgRadiu();
    m_avgCompact[m_viewId-1] = m_algorithm->getCompactness();
}

void ExperiCtrl::calcAnalyzeResult()
{
    int cellNum = 0;
    int liveCellNum = 0;
    int deadCellNum = 0;

    double avgRadiu = 0;
    double avgCompact = 0;
    for (int i = 0; i < VIEW_3; i++) {
        cellNum += m_cellNum[i];
        liveCellNum += m_liveCellNum[i];
        deadCellNum += m_deadCellNum[i];
        avgRadiu += m_liveCellNum[i] * m_avgRadiu[i];
        avgCompact += m_liveCellNum[i] * m_avgCompact[i];
    }
    avgRadiu /= cellNum;
    avgCompact /= cellNum;
    m_data->updateData(cellNum, liveCellNum, deadCellNum, avgRadiu, avgCompact);
}


void ExperiCtrl::pauseExperiment()
{
    m_experiState = EXPERI_IDLE;
    m_experiChamberState = EXPERI_CHAMBER_IDLE;
    m_experiPosState = EXPERI_POS_IDLE;
    m_experiCapState = CAP_IDLE;
}
