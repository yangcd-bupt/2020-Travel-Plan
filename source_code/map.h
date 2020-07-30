#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QLabel>

namespace Ui {
class map;
}

class map : public QWidget
{
    Q_OBJECT

public:
    explicit map(QWidget *parent = 0);
    ~map();

private:
    Ui::map *ui;

    //1s定时器
    QTimer *mTimer;
    int count;
    bool flag;

    //交通工具的选择和移动
    int type;
    int from,to;
    int currentPosition[2];
    int destination[2];
    int city[10][2];
    int xMove,yMove;

    //移动方式
    void plane_move();
    void train_move();
    void car_move();
    void end();
private slots:
    //map模块计时器超时
    void mTimer_timeout();
    //从主窗口获取数据
    void getData(int type,int currentFrom,int currentTo);
    //显示旅客
    void showP(int currentFrom);
};

#endif // MAP_H
