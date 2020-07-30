#include "map.h"
#include "ui_map.h"

map::map(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::map)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    hide();

    ui->transposition->hide();
    ui->transposition->clear();
    ui->transposition->setScaledContents(true);
    ui->passenger->hide();

    //设置城市位置
    city[0][0]=590;city[0][1]=450;//北京
    city[1][0]=590;city[1][1]=270;//上海
    city[2][0]=760;city[2][1]=560;//广州
    city[3][0]=430;city[3][1]=560;//南京
    city[4][0]=250;city[4][1]=680;//厦门
    city[5][0]=900;city[5][1]=680;//深圳
    city[6][0]=590;city[6][1]=150;//合肥
    city[7][0]=120;city[7][1]=770;//济南
    city[8][0]=590;city[8][1]=10;//郑州
    city[9][0]=1000;city[9][1]=770;//重庆

}

map::~map()
{
    delete ui;
}

void map::plane_move()
{
    if(count==9){
        ui->transposition->move(destination[0],destination[1]);
        flag=0;
    }
    else
        ui->transposition->move(currentPosition[0],currentPosition[1]);

}

void map::train_move()
{
    if(count==19){
        ui->transposition->move(destination[0],destination[1]);
        flag=0;
    }
    else
        ui->transposition->move(currentPosition[0],currentPosition[1]);
}

void map::car_move()
{
    if(count==29){
        ui->transposition->move(destination[0],destination[1]);
        flag=0;
    }
    else
        ui->transposition->move(currentPosition[0],currentPosition[1]);

}

void map::mTimer_timeout()
{
    if(flag)
    {
        count++;
        currentPosition[0]+=xMove;
        currentPosition[1]+=yMove;
        switch (type) {
        case 1:
            plane_move();
            break;
        case 2:
            train_move();
            break;
        case 3:
            car_move();
            break;
        }
    }
    else
        end();

}

void map::showP(int currentFrom)
{
    int position[2];
    position[0]=city[currentFrom][0];
    position[1]=city[currentFrom][1];
    ui->passenger->move(position[0],position[1]);
    ui->passenger->show();
}

void map::getData(int type, int currentFrom, int currentTo){
    this->type=type;
    from=currentFrom;
    to=currentTo;

        //设置位置
        currentPosition[0]=city[from][0];
        currentPosition[1]=city[from][1];
        destination[0]=city[to][0];
        destination[1]=city[to][1];

        ui->transposition->move(currentPosition[0],currentPosition[1]);
        //每次移动的步长
        switch (type) {
        case 1:
            ui->transposition->setStyleSheet("border-image: url(:/images/plane)");
            xMove=(int)(destination[0]-currentPosition[0])/10;
            yMove=(int)(destination[1]-currentPosition[1])/10;
            break;
        case 2:
            ui->transposition->setStyleSheet("border-image: url(:/images/train)");
            xMove=(int)(destination[0]-currentPosition[0])/20;
            yMove=(int)(destination[1]-currentPosition[1])/20;
            break;
        case 3:
            ui->transposition->setStyleSheet("border-image: url(:/images/car)");
            xMove=(int)(destination[0]-currentPosition[0])/30;
            yMove=(int)(destination[1]-currentPosition[1])/30;
            break;
        }
        ui->passenger->hide();
        ui->transposition->show();

        //设置定时器
        count=0;
        flag=1;
        mTimer=new QTimer(this);
        mTimer->setInterval(1000);
        mTimer->start();
        //mTimer->stop();
        connect(mTimer,SIGNAL(timeout()),this,SLOT(mTimer_timeout()));


}

void map::end()
{
    ui->transposition->hide();
    ui->passenger->move(destination[0],destination[1]);
    ui->passenger->show();
    mTimer->stop();
}
