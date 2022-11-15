#ifndef EXPERIRESULTUI_H
#define EXPERIRESULTUI_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>

namespace Ui {
class experiResultUi;
}

class experiResultUi : public QWidget
{
    Q_OBJECT

public:
    explicit experiResultUi(QWidget *parent = nullptr);
    ~experiResultUi();
    void showResult();

    void setExperiId(QString id);
    void changeView(int id);
    void changeChamber(int id);
    void changeImageType(int id);
    void initResultShow(QString experiId, int chamberSelc);

private slots:
    void onViewChanged();
    void onImageTypeChanged();


    void on_cBoxChamberSelect_currentIndexChanged(const QString &arg1);

private:
    Ui::experiResultUi *ui;
    QGraphicsScene          *m_scene;
    QGraphicsPixmapItem     *m_imageItem;

    QString m_experiId;
    int     currentViewId;
    int     currentChamberId;
    int     currentTypeId;

    QString imgPath;
    QImage imgView;
    QString getFileName(int chamberId, int viewId, int imgType);
};

#endif // EXPERIRESULTUI_H
