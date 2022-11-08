#ifndef EXPERICTRL_H
#define EXPERICTRL_H

#include <QObject>
#include <DevCtrl.h>

class ExperiCtrl : public QObject
{
    Q_OBJECT
public:
    explicit ExperiCtrl(QObject *parent = nullptr);

signals:

private:
    DevCtrl *devCtrl;

};

#endif // EXPERICTRL_H
