#include "inExperimentUi.h"
#include "ui_inExperimentUi.h"
#include <QFile>

inExperimentUi::inExperimentUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::inExperimentUi)
{
    ui->setupUi(this);
    loadStyleSheet(":/styles/inExperimentStyle.qss");
    m_waitingGif = new QMovie(":/images/waiting.gif", QByteArray(), this);
    m_waitingGif->setCacheMode(QMovie::CacheAll);
    ui->lblWaittingGif->setMovie(m_waitingGif);
    ui->lblWaittingGif->setScaledContents(true);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->lblNoticeText_3rd->setText("start experiment...");
    m_waitingGif->start();
}

inExperimentUi::~inExperimentUi()
{
    delete ui;
}

void inExperimentUi::updateNoticeText(QString notice)
{
    ui->lblNoticeText_1st->setText(ui->lblNoticeText_1st->text());
    ui->lblNoticeText_2nd->setText(ui->lblNoticeText_3rd->text());
    ui->lblNoticeText_3rd->setText(notice);
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
