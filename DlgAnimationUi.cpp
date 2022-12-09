#include "DlgAnimationUi.h"
#include "ui_DlgAnimationUi.h"

DlgAnimationUi::DlgAnimationUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAnimationUi)
{
    ui->setupUi(this);
    m_chipState = false;
    m_movie = new QMovie(":/images/insert_chip.gif", QByteArray(), this);
    m_movie->setCacheMode(QMovie::CacheAll);
    ui->lblMovie->setMovie(m_movie);
    ui->lblMovie->setScaledContents(true);
    this->setWindowFlags(Qt::FramelessWindowHint);

    m_movie->start();
}

DlgAnimationUi::~DlgAnimationUi()
{
    delete ui;
}

void DlgAnimationUi::on_btnOk_clicked()
{
    if (m_chipState == 1) {
        this->accept();
    }
//    this->accept();
}
