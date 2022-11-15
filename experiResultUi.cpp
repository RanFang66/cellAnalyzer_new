#include "experiResultUi.h"
#include "ui_experiResultUi.h"
#include <QFile>

const QStringList imageType = {
    "BR",
    "FL1_FL2",
    "BR_Marked",
};

experiResultUi::experiResultUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::experiResultUi)
{
    ui->setupUi(this);

    m_scene = new QGraphicsScene(this);
    m_imageItem = m_scene->addPixmap(QPixmap(600, 1000));
    ui->gvCellImage->setScene(m_scene);
    imgPath = "/cellImages/";
    currentChamberId = 1;
    currentViewId = 1;
    currentTypeId = 0;
    connect(ui->rBtnView1, SIGNAL(clicked()), this, SLOT(onViewChanged()));
    connect(ui->rBtnView2, SIGNAL(clicked()), this, SLOT(onViewChanged()));
    connect(ui->rBtnView3, SIGNAL(clicked()), this, SLOT(onViewChanged()));
    connect(ui->rBtnBright, SIGNAL(clicked()), this, SLOT(onImageTypeChanged()));
    connect(ui->rBtnFluore, SIGNAL(clicked()), this, SLOT(onImageTypeChanged()));
    connect(ui->rBtnMarked, SIGNAL(clicked()), this, SLOT(onImageTypeChanged()));

}

experiResultUi::~experiResultUi()
{
    delete ui;
}



void experiResultUi::showResult()
{
    QString fileName;
    fileName  = QString("chamber%1_%2_%3.jpg").arg(currentChamberId).arg(currentViewId).arg(imageType.at(currentTypeId));
    imgPath = "/cellImages/" + m_experiId + "/";
    imgView = QImage(imgPath + fileName);
//    if (m_imageItem) {
        m_imageItem->setPixmap(QPixmap::fromImage(imgView));
//    } else {
//        m_imageItem = m_scene->addPixmap(QPixmap::fromImage(imgView));
//    }
}

void experiResultUi::setExperiId(QString id)
{
    m_experiId = id;
}

void experiResultUi::changeView(int id)
{
    currentViewId = id;
    showResult();
}

void experiResultUi::changeChamber(int id)
{
    currentChamberId = id;
    showResult();
}

void experiResultUi::changeImageType(int id)
{
    currentTypeId = id;
    showResult();
}

void experiResultUi::initResultShow(QString experiId, int chamberSelc)
{
    m_experiId = experiId;
    currentChamberId = 0;
    for (int i = 0; i < 6; i++) {
        int p = (0x01 << i);
        if (chamberSelc & p) {
            ui->cBoxChamberSelect->addItem(QString("Chamber %1").arg(i+1), i+1);
            if (currentChamberId == 0) {
                currentChamberId = i+1;
            }
        }
    }
    currentViewId = 1;
    currentTypeId = 0;
    ui->rBtnView1->setChecked(true);
    ui->rBtnBright->setChecked(true);
    showResult();
}


QString experiResultUi::getFileName(int chamberId, int viewId, int imgType)
{
    QString fileName;
    fileName  = QString("chamber%1_%2_%3.jpg").arg(chamberId).arg(viewId).arg(imageType.at(imgType));
    return fileName;
}

void experiResultUi::onViewChanged()
{
    if (ui->rBtnView1->isChecked()) {
        changeView(1);
    } else if (ui->rBtnView2->isChecked()) {
        changeView(2);
    } else if (ui->rBtnView3->isChecked()) {
        changeView(3);
    } else {
        changeView(1);
    }
}

void experiResultUi::onImageTypeChanged()
{
    if (ui->rBtnBright->isChecked()) {
        changeImageType(0);
    } else if (ui->rBtnFluore->isChecked()) {
        changeImageType(1);
    } else if (ui->rBtnMarked->isChecked()) {
        changeImageType(2);
    } else {
        changeImageType(0);
    }
}

void experiResultUi::on_cBoxChamberSelect_currentIndexChanged(const QString &arg1)
{
     changeChamber(ui->cBoxChamberSelect->currentData().toInt());
}
