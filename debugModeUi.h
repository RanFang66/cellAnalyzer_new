#ifndef DEBUGMODEUI_H
#define DEBUGMODEUI_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>

namespace Ui {
class debugModeUi;
}

class debugModeUi : public QWidget
{
    Q_OBJECT

public:
    explicit debugModeUi(QWidget *parent = nullptr);
    ~debugModeUi();

public slots:
    void onCamImageUpdated(const unsigned char *data, int width, int height);

private:
    Ui::debugModeUi *ui;

    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_image_item;

    void initDubugModeUi();
};

#endif // DEBUGMODEUI_H
