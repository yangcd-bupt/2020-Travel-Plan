#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QLabel>
#include <QProcess>
#include <QStack>

#include "map.h"
#include "way.h"
#include "mysql.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;


    QProcess process;
    MySql mysql;
    //时间参数
    int day,hour,minute;
    QString sDay,sHour,sMinute;
    QString txtContext=tr("第");

    //输入的数据
    int strategy;
    int from,to;
    int timeLimite;
    int startTime[3];
    bool pushBotton;

    //定时器
    QTimer *fTimer;
    //子窗口
    map *mapWidget;

    //画图所需参数
    int type;
    int currentFrom,currentTo;
    //交通工具花费时间
    int plane_cost=1;
    int train_cost=2;
    int car_cost=3;

    //输出的数据
    Way plan[10];//记录每个城市的移动路径
    int arriveTime[10][3];//小时：分钟：天
    bool success;

    //算法参数
    Way tempPlan[10];
    QStack<int> path;
    QStack<int> mapPath;
    double minCost;
    int costTime[2];
    double cost;
    double value[10];
    int currentTime[3];
    bool throughCity[10];
    double distance[10];

    //算法
    void Makeplan(int city);
    void findWay();
    void Djkstra();
    void updateCity(int currentCity);
    void DFS(int city,double totalCost);
    double calculateTotalCost(bool flag,int from,int start);
    void calculateCost(bool flag,int from,int start,int strategy);

    void updateCurrentTime();//时间向前6分钟
private slots:
    void fTimer_timeout();//定时器溢出
    void on_submit_clicked();//按下提交按钮
    void on_pushButton_clicked();

signals:
    sendData(int type,int currentFrom,int currentTo);
    showPassenger(int currentFrom);
};

#endif // WIDGET_H
