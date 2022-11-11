#ifndef EXPERIRESULTUI_H
#define EXPERIRESULTUI_H

#include <QWidget>

namespace Ui {
class experiResultUi;
}

class experiResultUi : public QWidget
{
    Q_OBJECT

public:
    explicit experiResultUi(QWidget *parent = nullptr);
    ~experiResultUi();

private:
    Ui::experiResultUi *ui;
};

#endif // EXPERIRESULTUI_H
