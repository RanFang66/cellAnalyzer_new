#include "experiResultUi.h"
#include "ui_experiResultUi.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>

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
    loadStyleSheet(":/styles/experiResultStyle.qss");
    grabGesture(Qt::PinchGesture);
    m_scene = new QGraphicsScene(this);
    m_imageItem = m_scene->addPixmap(QPixmap(600, 1000));
    ui->gvCellImage->setScene(m_scene);
    ui->gvCellImage->scale(1/1.728, 1/1.728);
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

void experiResultUi::initResultShow(QString experiID, int chamber)
{
    QSqlRecord rec;
    m_experiId = experiID;
    QString sampleId;
    int index = 0;
    currentChamberId = 0;
    currentViewId = 1;
    currentTypeId = 0;

    query->prepare("SELECT * FROM experiData WHERE experiID = :ID");
    query->bindValue(":ID", experiID);

    ui->cBoxChamberSelect->clear();
    if (query->exec()) {
        while (query->next()) {
            int chamberId = query->value("chamberSet").toInt();
            ui->cBoxChamberSelect->addItem(QString("Chamber %1").arg(chamberId), chamberId);
            if (chamberId == chamber) {
                currentChamberId = chamberId;
                sampleId = query->value("sampleID").toString();
                rec = query->record();
                ui->cBoxChamberSelect->setCurrentIndex(index);
            }
            index++;
        }
    } else {
        QMessageBox::critical(this, "Error", "Query record failed!");
        return ;
    }

    if (chamber == 0) {
        query->first();
        currentChamberId = query->value("chamberSet").toInt();
        sampleId = query->value("sampleID").toString();
        rec = query->record();
    }



//    for (int i = 0; i < 6; i++) {
//        int p = (0x01 << i);
//        if (chamberSet & p) {
//            ui->cBoxChamberSelect->addItem(QString("Chamber %1").arg(i+1), i+1);
//            if (currentChamberId == 0) {
//                currentChamberId = i+1;
//            }
//        }
//    }

    ui->rBtnView1->setChecked(true);
    ui->rBtnBright->setChecked(true);

    double val = rec.value("viability").toDouble();
    ui->lblViability->setText(QString::number(val, 'f', 2) + "%");
    val = rec.value("cellConc").toDouble();
    ui->lblCellConc->setText(QString::number(val, 'e', 2));
    val = rec.value("liveCellConc").toDouble();
    ui->lblLiveCellConc->setText(QString::number(val, 'e', 2));
    val = rec.value("deadCellConc").toDouble();
    ui->lblDeadCellConc->setText(QString::number(val, 'e', 2));

    ui->lblCellNum->setText(rec.value("totalCellNum").toString());
    ui->lblLiveCellNum->setText(rec.value("liveCellNum").toString());
    ui->lblDeadCellNum->setText(rec.value("deadCellNum").toString());
    val = rec.value("avgDiameter").toDouble();
    ui->lblAvgDiameter->setText(QString::number(val, 'f', 2));
    val = rec.value("aggregateRate").toDouble();
    ui->lblAggregateRate->setText(QString::number(val, 'f', 2) + "%");
    val = rec.value("avgCompactness").toDouble();
    ui->lblAvgRoundness->setText(QString::number(val, 'f', 2));
    val = rec.value("nucleusRate").toDouble();
    ui->lblNucleusRate->setText(QString::number(val, 'f', 2) + "%");
    imgPath = "/cellImages/" + m_experiId + "_" + sampleId + "/";
    showCellImage();
}


QString experiResultUi::getFileName(int chamberId, int viewId, int imgType)
{
    QString fileName;
    fileName  = QString("chamber%1_%2_%3.jpg").arg(chamberId).arg(viewId).arg(imageType.at(imgType));
    return fileName;
}

void experiResultUi::loadStyleSheet(const QString &styleSheetFile)
{
    QFile file(styleSheetFile);
    file.open(QFile::ReadOnly);
    if (file.isOpen()) {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Login: Open Style Sheet File Failed!";
    }
}

bool experiResultUi::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }
    return QWidget::event(event);
}

bool experiResultUi::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }
    return true;
}

void experiResultUi::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();

    qreal m_scale = 1;
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        m_scale = gesture->totalScaleFactor();
//        if (m_scale > 1) {
//            m_scale = 1.2;
//        } else {
//            m_scale = 1/1.2;
//        }
    }
    if (gesture->state() == Qt::GestureFinished) {
        ui->gvCellImage->scale(m_scale, m_scale);
        m_scale = 1;
    }
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


void experiResultUi::on_btnReturn_clicked()
{
     emit returnToMainPage();
}

void experiResultUi::on_btnZoomOut_clicked()
{
    ui->gvCellImage->scale(1.2, 1.2);
}

void experiResultUi::on_btnZoomIn_clicked()
{
    ui->gvCellImage->scale(1/1.2, 1/1.2);
}

void experiResultUi::on_cBoxChamberSelect_activated(int index)
{
    int chamberId = ui->cBoxChamberSelect->currentData().toInt();
    QSqlRecord rec;
    QString sampleId;

    query->prepare("SELECT * FROM experiData WHERE experiID = :ID AND chamberSet = :chamber");
    query->bindValue(":ID", m_experiId);
    query->bindValue(":chamber", chamberId);
    if (query->exec() && query->next()) {
        sampleId = query->value("sampleID").toString();
        rec = query->record();
    }
    currentChamberId = chamberId;
    double val = rec.value("viability").toDouble();
    ui->lblViability->setText(QString::number(val, 'f', 2) + "%");
    val = rec.value("cellConc").toDouble();
    ui->lblCellConc->setText(QString::number(val, 'e', 2));
    val = rec.value("liveCellConc").toDouble();
    ui->lblLiveCellConc->setText(QString::number(val, 'e', 2));
    val = rec.value("deadCellConc").toDouble();
    ui->lblDeadCellConc->setText(QString::number(val, 'e', 2));

    ui->lblCellNum->setText(rec.value("totalCellNum").toString());
    ui->lblLiveCellNum->setText(rec.value("liveCellNum").toString());
    ui->lblDeadCellNum->setText(rec.value("deadCellNum").toString());
    val = rec.value("avgDiameter").toDouble();
    ui->lblAvgDiameter->setText(QString::number(val, 'f', 2));
    val = rec.value("aggregateRate").toDouble();
    ui->lblAggregateRate->setText(QString::number(val, 'f', 2) + "%");
    val = rec.value("avgCompactness").toDouble();
    ui->lblAvgRoundness->setText(QString::number(val, 'f', 2));
    val = rec.value("nucleusRate").toDouble();
    ui->lblNucleusRate->setText(QString::number(val, 'f', 2) + "%");
    imgPath = "/cellImages/" + m_experiId + "_" + sampleId + "/";
    showCellImage();
}
