#include "inExperimentUi.h"
#include "ui_inExperimentUi.h"
#include <QFile>
#include <QDebug>
inExperimentUi::inExperimentUi(ExperiCtrl *experiCtrl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::inExperimentUi),
    experi(experiCtrl)
{
    ui->setupUi(this);
    loadStyleSheet(":/styles/inExperimentStyle.qss");
    m_waitingGif = new QMovie(":/images/waiting.gif", QByteArray(), this);
    m_waitingGif->setCacheMode(QMovie::CacheAll);
    ui->lblWaittingGif->setMovie(m_waitingGif);
    ui->lblWaittingGif->setScaledContents(true);
    this->setWindowFlags(Qt::FramelessWindowHint);

//    m_scene = new QGraphicsScene(this);
//    ui->gvImage->setScene(m_scene);

    ui->lblNoticeText_3rd->setText("start experiment...");
    m_waitingGif->start();
}

inExperimentUi::~inExperimentUi()
{
    delete ui;
}

void inExperimentUi::updateNoticeText(QString notice)
{
    ui->lblNoticeText_1st->setText(ui->lblNoticeText_2nd->text() + " finished");
    ui->lblNoticeText_2nd->setText(ui->lblNoticeText_3rd->text() + " finished");
    ui->lblNoticeText_3rd->setText(notice);
}

const QStringList IMAGE_TYPE = {
    "bright",
    "FL 1",
    "FL 2",
};

void inExperimentUi::onUpdateImage()
{
    QImage imgShow = experi->getCurrImage();
    int w = imgShow.width();
    int h = imgShow.height();
    imgShow.scaled(w/2, h/2,  Qt::KeepAspectRatio);
    QPixmap pic = QPixmap::fromImage(imgShow);
    ui->lblImage->setPixmap(pic);
//    if (m_image_item) {
//        m_image_item->setPixmap(QPixmap::fromImage(imgShow));
//    } else {
//        m_image_item = m_scene->addPixmap(QPixmap::fromImage(imgShow));
//    }
//    m_scene->setSceneRect(0, 0, imgShow.width()/2, imgShow.height()/2);
    int chamber, view, imageType;
    experi->getCurrentState(chamber, view, imageType);
    QString notice = QString("cap the %1 image of view %2, chamber %3").arg(IMAGE_TYPE.at(imageType-1)).arg(view).arg(chamber);
    updateNoticeText(notice);
}



void inExperimentUi::on_btnPauseExperi_clicked()
{
    emit pauseExperiment();
}


void inExperimentUi::loadStyleSheet(const QString &styleSheetFile)
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
