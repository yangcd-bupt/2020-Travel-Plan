#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <qpainter.h>

namespace Ui {
class mapWidget;
}

class mapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mapWidget(QWidget *parent = 0);
    ~mapWidget();
private:
    Ui::mapWidget *ui;
};

#endif // MAPWIDGET_H
