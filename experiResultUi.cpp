#include "experiResultUi.h"
#include "ui_experiResultUi.h"
#include <QFile>
#include <QMessageBox>
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
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);
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



void experiResultUi::showCellImage()
{
    QString fileName;
    fileName  = QString("chamber%1_%2_%3.jpg").arg(currentChamberId).arg(currentViewId).arg(imageType.at(currentTypeId));
    imgPath = "/cellImages/" + m_experiId + "/";
    imgView = QImage(imgPath + fileName);
    m_imageItem->setPixmap(QPixmap::fromImage(imgView));

}

void experiResultUi::setExperiId(QString id)
{
    m_experiId = id;
}

void experiResultUi::changeView(int id)
{
    currentViewId = id;
    showCellImage();
}

void experiResultUi::changeChamber(int id)
{
    currentChamberId = id;
    showCellImage();
}

void experiResultUi::changeImageType(int id)
{
    currentTypeId = id;
    showCellImage();
}

void experiResultUi::initResultShow(QString experiID)
{
    QSqlRecord rec;
    query->prepare("SELECT * FROM experiData WHERE experiID = :ID");
    query->bindValue(":ID", experiID);
    if (query->exec() && query->next()) {
        rec = query->record();
    } else {
        QMessageBox::critical(this, "Error", "Query record failed!");
        return ;
    }


    m_experiId = experiID;
    currentChamberId = 0;
    int chamberSet = rec.value("chamberSet").toInt();
    ui->cBoxChamberSelect->clear();
    for (int i = 0; i < 6; i++) {
        int p = (0x01 << i);
        if (chamberSet & p) {
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
    ui->lblViability->setText(rec.value("viability").toString());
    ui->lblCellConc->setText(rec.value("cellConc").toString());
    ui->lblLiveCellConc->setText(rec.value("liveCellConc").toString());
    ui->lblDeadCellConc->setText(rec.value("deadCellConc").toString());
    ui->lblCellNum->setText(rec.value("totalCellNum").toString());
    ui->lblLiveCellNum->setText(rec.value("liveCellNum").toString());
    ui->lblDeadCellNum->setText(rec.value("deadCellNum").toString());
    ui->lblAvgDiameter->setText(rec.value("avgDiameter").toString());
    ui->lblAggregateRate->setText(rec.value("aggregateRate").toString());
    ui->lblAvgRoundness->setText(rec.value("avgCompactness").toString());
    showCellImage();
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
