#include "debugModeUi.h"
#include "ui_debugModeUi.h"

debugModeUi::debugModeUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::debugModeUi)
{
    ui->setupUi(this);
}

debugModeUi::~debugModeUi()
{
    delete ui;
}

void debugModeUi::onCamImageUpdated(const unsigned char *data, int width, int height)
{
    QImage imgShow(data, width, height, QImage::Format_BGR888);
    if(m_image_item)
    {
        m_image_item->setPixmap(QPixmap::fromImage(imgShow));
    }
    else
    {
        m_image_item = m_scene->addPixmap(QPixmap::fromImage(imgShow));
    }

    m_scene->setSceneRect(0, 0, imgShow.width(), imgShow.height());
}

void debugModeUi::initDubugModeUi()
{
    m_scene = new QGraphicsScene(this);
    ui->gvMain->setScene(m_scene);
}
