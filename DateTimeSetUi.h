#ifndef DATETIMESETUI_H
#define DATETIMESETUI_H

#include <QWidget>

namespace Ui {
class DateTimeSetUi;
}

class DateTimeSetUi : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeSetUi(QWidget *parent = nullptr);
    ~DateTimeSetUi();

private:
    Ui::DateTimeSetUi *ui;
};

#endif // DATETIMESETUI_H
