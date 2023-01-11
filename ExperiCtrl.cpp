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
    CAP_INIT_FILTER,
    CAP_INIT_CAMERA,
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
    connect(devCtrl, SIGNAL(camInitOk()), this, SLOT(experimentStateTransfer()));
    connect(this, SIGNAL(experimentInitOk()), this, SLOT(experimentStateMachine()));
    connect(this, SIGNAL(experiCapFinished()), this, SLOT(experiPosStateTransfer()));
    connect(this, SIGNAL(experiOnePosFinished()), this, SLOT(experiChamberStateTransfer()));
    connect(this, SIGNAL(experiOneChamberFinished()), this, SLOT(experimentStateTransfer()));

}

ExperiCtrl::ExperiCtrl(DevCtrl *dev, ExperiSetting *setting, ExperiData *data, QObject *parent)
    : devCtrl(dev), m_data(data), m_setting(setting), QObject(parent)
{
    m_experiState = EXPERI_IDLE;
    m_experiChamberState = EXPERI_CHAMBER_IDLE;
    m_experiPosState = EXPERI_POS_IDLE;
    m_experiCapState = CAP_IDLE;

    m_chamberId = 1;
    m_viewId = 1;
    m_imageTypeId = 0;
    m_algorithm = new CellImageAlogrithm();
    m_algoThread = new QThread();

    m_algorithm->moveToThread(m_algoThread);
    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Mat>("Mat&");
    connect(this, SIGNAL(analyzeImage(int, Mat&, Mat&)), m_algorithm, SLOT(markCells(int ,Mat&, Mat&)));
    connect(m_algoThread, &QThread::finished, m_algorithm, &QObject::deleteLater);
    connect(m_algoThread, &QThread::started, m_algorithm, &CellImageAlogrithm::initAlgorithm);
    m_algoThread->start();
    // for seekgene chip
//    m_chipPos_Y[0] = 270;
//    m_chipPos_Y[1] = 2241;
//    m_chipPos_Y[2] = 4080;
//    m_chipPos_Y[3] = 5907;
//    m_chipPos_Y[4] = 7802;
//    m_chipPos_Y[5] = 9767;


    // for CountStar chip
    m_chipPos_Y[0] = 550;
    m_chipPos_Y[1] = 2700;
    m_chipPos_Y[2] = 4800;
    m_chipPos_Y[3] = 7100;
    m_chipPos_Y[4] = 9400;
    m_chipPos_Y[5] = 9600;

    m_chipPos_X[0] = 900;
    m_chipPos_X[1] = 1500;
    m_chipPos_X[2] = 2100;



}

void ExperiCtrl::getCurrentState(int &chamber, int &view, int &imageType)
{
    chamber = m_chamberId;
    view = m_viewId;
    imageType = m_imageTypeId;

}

const QImage ExperiCtrl::getCurrImage() const
{
    return devCtrl->getQImage();
}

void ExperiCtrl::startExperiment(const QString &experiId)
{
//    imgFilePath = "/cellImages/" + experiId + "/";
//    QDir dir(imgFilePath);
//    if (!dir.exists()) {
//        dir.mkdir(imgFilePath);
//    }

//    int minR = m_setting->getCellMinRadiu(1) * 0.8 + 0.4;
//    int maxR = m_setting->getCellMaxRadiu(1) * 0.8 + 0.4;
//    m_algorithm->setCellParameters(minR, maxR);
//    m_algorithm->initAlgorithm();
//    for (int i = 0; i < VIEW_3; i++) {
//        m_cellNum[i] = 0;
//        m_clusterCellNum[i] = 0;
//        m_liveCellNum[i] = 0;
//        m_deadCellNum[i] = 0;
//        m_avgRadiu[i] = 0;
//        m_avgCompact[i] = 0;
//    }
    m_experiState = EXPERI_INIT;
    experimentStateMachine();
}

void ExperiCtrl::experimentStateTransfer()
{
    if (m_experiState == EXPERI_FINISH) {
        disconnect(devCtrl, SIGNAL(chipYMotorStateUpdated()), this, SLOT(experimentStateTransfer()));
        devCtrl->motorRun(DevCtrl::INSERT_MOTOR, 2);
        m_experiState = EXPERI_IDLE;
        m_experiChamberState = EXPERI_CHAMBER_IDLE;
        m_experiPosState = EXPERI_POS_IDLE;
        m_experiCapState = CAP_IDLE;
    } else {
        int nextState = getNextState(m_experiState);
        m_experiState = nextState;
        m_yPos = getNextChamberPos(nextState);
        m_experiChamberState = EXPERI_CHAMBER_INIT;
        experimentStateMachine();
    }
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
        m_experiChamberState = EXPERI_CHAMBER_IDLE;
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
            m_experiCapState = CAP_INIT_FILTER;
        }
        break;
    case EXPERI_POS_BRIGHT:
        saveImages(imgBR, imgMarked);
        m_experiPosState = EXPERI_POS_BLUE;
        m_imageTypeId = IMAGE_FL1;
        m_filterPos = FILTER_BLUE_POS;
        m_ledState = DevCtrl::LED_BLUE;
        m_experiCapState = CAP_INIT_FILTER;
        break;
    case EXPERI_POS_BLUE:
        saveImages(imgFL1, imgMarked);
        m_experiPosState = EXPERI_POS_GREEN;
        m_imageTypeId = IMAGE_FL2;
        m_filterPos = FILTER_GREEN_POS;
        m_ledState = DevCtrl::LED_GREEN;
        m_experiCapState = CAP_INIT_FILTER;
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
    case CAP_INIT_FILTER:
//        if (devCtrl->filterPos() == m_filterPos) {
            m_experiCapState = CAP_INIT_CAMERA;
//        }
        break;
    case CAP_INIT_CAMERA:
        m_experiCapState = CAP_AUTOFOCUS;
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
    connect(devCtrl, SIGNAL(chipYMotorStateUpdated()), this, SLOT(experiChamberStateTransfer()));
    connect(devCtrl, SIGNAL(chipXMotorStateUpdated()), this, SLOT(experiPosStateTransfer()));
    connect(devCtrl, SIGNAL(filterMotorStateUpdated()), this, SLOT(experiCapStateTransfer()));
    connect(devCtrl, SIGNAL(imageUpdated()), this, SLOT(experiCapStateTransfer()));
    connect(devCtrl, SIGNAL(autoFocusComplete()), this, SLOT(experiCapStateTransfer()));
    connect(m_algorithm, SIGNAL(markCellsFinished()), this, SLOT(experiCapStateTransfer()));
    connect(devCtrl->m_camCtrl, SIGNAL(cameraParasSetRet()), this, SLOT(experiCapStateTransfer()));
    connect(this, SIGNAL(experimentInitOk()), this, SLOT(experimentStateTransfer()));
    connect(this, SIGNAL(experiCapFinished()), this, SLOT(experiPosStateTransfer()));
    connect(this, SIGNAL(experiOnePosFinished()), this, SLOT(experiChamberStateTransfer()));
    connect(this, SIGNAL(experiOneChamberFinished()), this, SLOT(experimentStateTransfer()));
    m_autoFocusFlag = false;
    m_pauseFlag = false;
    emit experimentInitOk();
}

void ExperiCtrl::endExperiment()
{
    disconnect(devCtrl, SIGNAL(chipYMotorStateUpdated()), this, SLOT(experiChamberStateTransfer()));
    disconnect(devCtrl, SIGNAL(chipXMotorStateUpdated()), this, SLOT(experiPosStateTransfer()));
    disconnect(devCtrl, SIGNAL(filterMotorStateUpdated()), this, SLOT(experiCapStateTransfer()));
    disconnect(devCtrl, SIGNAL(imageUpdated()), this, SLOT(experiCapStateTransfer()));
    disconnect(devCtrl, SIGNAL(autoFocusComplete()), this, SLOT(experiCapStateTransfer()));
    disconnect(m_algorithm, SIGNAL(markCellsFinished()), this, SLOT(experiCapStateTransfer()));
    disconnect(devCtrl->m_camCtrl, SIGNAL(cameraParasSetRet()), this, SLOT(experiCapStateTransfer()));
    disconnect(this, SIGNAL(experimentInitOk()), this, SLOT(experimentStateTransfer()));
    disconnect(this, SIGNAL(experiCapFinished()), this, SLOT(experiPosStateTransfer()));
    disconnect(this, SIGNAL(experiOnePosFinished()), this, SLOT(experiChamberStateTransfer()));
    disconnect(this, SIGNAL(experiOneChamberFinished()), this, SLOT(experimentStateTransfer()));
    devCtrl->resetPos();
    connect(devCtrl, SIGNAL(chipYMotorStateUpdated()), this, SLOT(experimentStateTransfer()));
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

//    case EXPERI_X_RESET:
//        devCtrl->motorRun(DevCtrl::CHIP_MOTOR_X, DevCtrl::MOTOR_RUN_POS, 1750);
//        break;

//    case EXPERI_Y_RESET:
//        devCtrl->motorReset(DevCtrl::CHIP_MOTOR_Y);
//        break;
    case EXPERI_FINISH:
        if (!m_pauseFlag){
//            calcAnalyzeResult();
            emit experimentFinished();
        }
        endExperiment();
//        m_experiState = EXPERI_IDLE;
//        m_experiChamberState = EXPERI_CHAMBER_IDLE;
//        m_experiPosState = EXPERI_POS_IDLE;
//        m_experiCapState = CAP_IDLE;
        break;
    }
}


void ExperiCtrl::experiChamberStateMachine()    // ctrl the experiment in one chamber: ctrl the X pos motion
{

    switch(m_experiChamberState) {
    case EXPERI_CHAMBER_IDLE:
        break;
    case EXPERI_CHAMBER_INIT:
    {
        QString experiId = m_setting->getExperiId();
        QString sampleId = m_setting->sampleId(m_chamberId);
        imgFilePath = "/cellImages/" + experiId + "_" + sampleId +"/";
        QDir dir(imgFilePath);
        if (!dir.exists()) {
            dir.mkdir(imgFilePath);
        }

        int minR = m_setting->getCellMinRadiu(m_chamberId) * 0.8 + 0.4;
        int maxR = m_setting->getCellMaxRadiu(m_chamberId) * 0.8 + 0.4;
        m_algorithm->setCellParameters(minR, maxR);
        m_algorithm->initAlgorithm();
        for (int i = 0; i < VIEW_3; i++) {
            m_cellNum[i] = 0;
            m_clusterCellNum[i] = 0;
            m_liveCellNum[i] = 0;
            m_deadCellNum[i] = 0;
            m_avgRadiu[i] = 0;
            m_avgCompact[i] = 0;
        }
        devCtrl->motorRun(DevCtrl::CHIP_MOTOR_Y, DevCtrl::MOTOR_RUN_POS, m_yPos);
        break;
    }
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
        calcAnalyzeResult();
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
    case CAP_INIT_FILTER:
        devCtrl->ledLigthOn(m_ledState);
//        devCtrl->motorRun(DevCtrl::FILTER_MOTOR, DevCtrl::MOTOR_RUN_POS, m_filterPos);
        break;
    case CAP_INIT_CAMERA:
        devCtrl->initCameraParas(m_imageTypeId);
        break;
    case CAP_AUTOFOCUS:
        if (!m_autoFocusFlag) {
            devCtrl->startAutoFocus(true);
            m_autoFocusFlag = true;
        } else {
            devCtrl->startAutoFocus(false);
        }
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
            emit analyzeImage(m_imageTypeId, imgBR, imgMarked);
            break;
        case IMAGE_FL1:
            imgFL1 = devCtrl->getCVImage();
            imgTemp = imgFL1.clone();
            imgMarked = imgFL1.clone();
            emit analyzeImage(m_imageTypeId, imgFL1, imgMarked);
            break;
        case IMAGE_FL2:
            imgFL2 = devCtrl->getCVImage();
            imgMarked = imgFL2.clone();
            emit analyzeImage(m_imageTypeId, imgFL2, imgMarked);
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
    Mat imgFL1_FL2;


    cv::add(imgTemp, imgFL2, imgFL1_FL2, noArray(), CV_8UC3);
 //   Mat   img = imgFL1 + imgFL2;
    imwrite(name.toStdString(), imgFL1_FL2);
}

void ExperiCtrl::getAnalyzeResult()
{
    m_cellNum[m_viewId-1] = m_algorithm->getCellNum();
    m_clusterCellNum[m_viewId-1] = m_algorithm->getClusterCellNum();
    m_liveCellNum[m_viewId-1] = m_algorithm->getLiveCellNum();
    m_deadCellNum[m_viewId-1] = m_algorithm->getDeadCellNum();
    m_avgRadiu[m_viewId-1] = m_algorithm->getAvgRadiu();
    m_avgCompact[m_viewId-1] = m_algorithm->getCompactness();
}

void ExperiCtrl::calcAnalyzeResult()
{
    int cellNum = 0;
    int clusterCellNum = 0;
    int liveCellNum = 0;
    int deadCellNum = 0;

    double avgRadiu = 0;
    double avgCompact = 0;
    double aggregateRate = 0;
    for (int i = 0; i < VIEW_3; i++) {
        cellNum += m_cellNum[i];
        clusterCellNum += m_clusterCellNum[i];
        liveCellNum += m_liveCellNum[i];
        deadCellNum += m_deadCellNum[i];
        avgRadiu += m_cellNum[i] * m_avgRadiu[i];
        avgCompact += m_cellNum[i] * m_avgCompact[i];
    }
    avgRadiu /= cellNum;
    avgCompact /= cellNum;
    aggregateRate = (double)clusterCellNum / cellNum * 100.0;

    m_data->updateData(m_chamberId, cellNum, liveCellNum, deadCellNum, aggregateRate, avgRadiu*2, avgCompact);
}


void ExperiCtrl::pauseExperiment()
{
    devCtrl->stopAllMotor();
    m_pauseFlag = true;
    m_experiState = EXPERI_FINISH;
    experimentStateMachine();

//    m_experiChamberState = EXPERI_CHAMBER_IDLE;
//    m_experiPosState = EXPERI_POS_IDLE;
//    m_experiCapState = CAP_IDLE;
}
