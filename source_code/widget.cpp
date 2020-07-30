#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //设置子窗口
    mapWidget=new map(this);
    mapWidget->move(10,10);
    mapWidget->show();
    //设置显示的图标
    this->setWindowIcon(QIcon(":/images/logo"));

    //数据库初始化
    process.start("D:/mysql/bin/mysqld.exe");
    //给城市风险度初值
    QSqlQuery query;
    QString sql="select city_id,value from city_id";
    query=mysql.selectCity(sql);
    while(query.next())
        value[query.value(0).toInt()-1]=query.value(1).toDouble();

    //初始化时间
    QFont font;
    font.setPointSize(12);
    ui->currentTime->setFont(font);
    day=0;hour=0;minute=0;
    sDay=QString::number(day,10);
    sHour=QString::number(hour,10);
    sMinute=QString::number(minute,10);
    txtContext.append(sDay);
    txtContext.append("天 ");
    txtContext.append(sHour);
    txtContext.append(":");
    txtContext.append(sMinute);
    ui->currentTime->setText(txtContext);
    //设置定时器
    fTimer=new QTimer(this);
    fTimer->setInterval(1000);
    fTimer->start();
    connect(fTimer,SIGNAL(timeout()),this,SLOT(fTimer_timeout()));
    //设置输入时间限制格式
    ui->setTimeLimite->setPlaceholderText("单位：小时");
    ui->setTimeLimite->setValidator(new QIntValidator(ui->setTimeLimite));
    //主窗口向子窗口传递数据
    QObject::connect(this,SIGNAL(sendData(int,int,int)),mapWidget,SLOT(getData(int,int,int)));
    QObject::connect(this,SIGNAL(showPassenger(int)),mapWidget,SLOT(showP(int)));

    pushBotton=false;
    qDebug("Initial window initialization");
}

Widget::~Widget()
{
    qDebug("End of operation");
    process.waitForFinished(-1);
    delete ui;
}

//时间向前6分钟
void Widget::updateCurrentTime()
{
    minute+=6;
    if(minute==60)
    {
        hour+=1;
        minute=0;
    }
    if(hour==24)
    {
        hour=0;
        day+=1;
    }
    txtContext="第";
    sDay=QString::number(day,10);
    sHour=QString::number(hour,10);
    sMinute=QString::number(minute,10);
    txtContext.append(sDay);
    txtContext.append("天 ");
    txtContext.append(sHour);
    txtContext.append(":");
    txtContext.append(sMinute);
    ui->currentTime->setText(txtContext);

   if(pushBotton&&!mapPath.isEmpty()){
    int temp=mapPath.pop();
    Way currentPlan=plan[temp];
    currentTime[0]=currentPlan.start;
    currentTime[1]=0;
    currentTime[2]=arriveTime[temp][2];
    if(day==currentTime[2]&&hour==currentTime[0]&&minute==0)
       emit sendData(currentPlan.transposition,currentPlan.from-1,currentPlan.to-1);
     else
       mapPath.push(temp);
     if(mapPath.isEmpty()){
         qDebug("passenger arrive destination!");
        ui->solve->clear();
        ui->arriveTime->clear();
        pushBotton=false;
     }
    }


}
//1s过去，更新时间
void Widget::fTimer_timeout()
{
    updateCurrentTime();
}

//单击确认提交按钮
void Widget::on_submit_clicked()
{
    //传入数据
    qDebug("Incoming data");
    strategy=ui->selectStrategy->currentIndex();
    from=ui->selectFrom->currentIndex();//输入为城市的id-1
    to=ui->selectTo->currentIndex();//输入为城市的id-1
    timeLimite=ui->setTimeLimite->text().toInt();
    startTime[0]=hour;
    startTime[1]=minute;
    startTime[2]=day;

    qDebug()<<"find path from "<<from<<" to "<<to<<" start "<<startTime[2]<<" "<<startTime[0]<<":"<<startTime[1];
    qDebug()<<"strategy is "<<strategy<<"time limite is "<<timeLimite;
    //暂停计时直到计算完路径
    fTimer->stop();

    //寻找符合策略的路径
    findWay();



    if(success)
    {
        //输出
        QString arriOut="第";
        QString arriDay=QString::number(arriveTime[to][2],10);
        QString arriMinute=QString::number(arriveTime[to][1],10);
        QString arriHour=QString::number(arriveTime[to][0],10);
        arriOut.append(arriDay);
        arriOut.append("天 ");
        arriOut.append(arriHour);
        arriOut.append(":");
        arriOut.append(arriMinute);
        ui->arriveTime->setText(arriOut);

        ui->solve->clear();
        QSqlQuery query;
        QString sql;

        while(!path.isEmpty())
        {
            QString outTime;
            QString cityPiont;
            QString output;
            int temp=path.pop();

            Way currentPlan=plan[temp];
            switch (currentPlan.transposition) {
            case 1:
                ui->solve->insertHtml("<img src=:/images/plane />");
                break;
            case 2:
                ui->solve->insertHtml("<img src=:/images/train />");
                break;
            case 3:
                ui->solve->insertHtml("<img src=:/images/car />");
                break;
            }
            ui->solve->insertHtml("<br>");

            cityPiont=QString::number(currentPlan.from,10);
            sql="select city from city where id=";
            sql.append(cityPiont);
            qDebug()<<sql;
            query=mysql.selectCity(sql);
            query.next();
            output.append(query.value(0).toString());
            output.append(" 至 ");
            cityPiont=QString::number(currentPlan.to,10);
            sql="select city from city where id=";
            sql.append(cityPiont);
            qDebug()<<sql;
            query=mysql.selectCity(sql);
            query.next();
            output.append(query.value(0).toString());
            qDebug()<<output;
            ui->solve->insertPlainText(output);

            ui->solve->insertHtml("<br>");

            output.clear();
            outTime=QString::number(currentPlan.start,10);
            output.append(outTime);
            output.append(":00 至 ");
            outTime=QString::number(currentPlan.end,10);
            output.append(outTime);
            output.append(":00");
            qDebug()<<output;
            ui->solve->insertPlainText(output);

            ui->solve->insertHtml("<br>");
        }
        pushBotton=true;
        fTimer->start();
        //画图
        emit showPassenger(from);
    }
    else
    {
        ui->solve->clear();
        QString output;
        output="can not find way!";
        qDebug()<<output;
        ui->solve->insertPlainText(output);
        fTimer->start();
    }
}

void Widget::findWay()
{
    success=false;
    for(int i=0;i<10;i++)
        throughCity[i]=false;
    minCost=INT_MAX;//风险最小

    path.clear();
    mapPath.clear();

    for(int i=0;i<10;i++)
        distance[i]=INT_MAX;//每个城市到初始城市风险度之和
    currentTime[0]=startTime[0];
    currentTime[1]=startTime[1];
    currentTime[2]=startTime[2];
    arriveTime[from][0]=currentTime[0];
    arriveTime[from][1]=currentTime[1];
    arriveTime[from][2]=currentTime[2];
    throughCity[from]=true;
    distance[from]=0;

    if(strategy==1)
    {
        DFS(from,0);
    }
    else
        Djkstra();
    if(success)
    {
        qDebug("find the way to destionation successfully");
        Makeplan(to);
    }

}

void Widget::DFS(int city,double totalCost)
{
    int cFrom=city+1;
    bool crossFlag=false;//是否跨天
    bool overFlag=true;//是否为整数小时
    QString sql;
    QSqlQuery query;
    QString sCurrentCity=QString::number(cFrom,10);
    QString sCurrentTime=QString::number(currentTime[0],10);

    if(currentTime[1]>0)
        overFlag=false;
    sql="select to_id,min(start),transpotation from time_table_id where from_id=";
    sql.append(sCurrentCity);
    sql.append(" and start>");
    if(overFlag)
       sql.append("=");
    sql.append(sCurrentTime);
    sql.append(" group by(to_id)");

    query=mysql.selectCity(sql);

    qDebug()<<sql;
    while(query.next())
    {
        int cTo=query.value(0).toInt();

        qDebug()<<"find path from"<<cFrom<<"to"<<cTo;
        //时间cost
        double add=calculateTotalCost(crossFlag,cFrom-1,query.value(1).toInt());
        totalCost+=add;

        if(!throughCity[cTo-1]&&timeLimite>totalCost&&cTo!=to+1)
        {
            qDebug()<<"update path from"<<cFrom<<"to"<<cTo;
            throughCity[cTo-1]=true;
            //风险cost
            calculateCost(crossFlag,cFrom-1,query.value(1).toInt(),query.value(2).toInt());
            distance[cTo-1]=distance[cFrom-1]+cost;

            arriveTime[cTo-1][0]=query.value(1).toInt()+query.value(2).toInt();
            arriveTime[cTo-1][1]=0;
            arriveTime[cTo-1][2]=arriveTime[cFrom-1][2];
            if(crossFlag)
                arriveTime[cTo-1][2]++;

            currentTime[0]=arriveTime[cTo-1][0];
            currentTime[1]=arriveTime[cTo-1][1];
            currentTime[2]=arriveTime[cTo-1][2];

                tempPlan[cTo-1].from=cFrom;
                tempPlan[cTo-1].to=cTo;
                tempPlan[cTo-1].start=query.value(1).toInt();
                tempPlan[cTo-1].transposition=query.value(2).toInt();
                switch (tempPlan[cTo-1].transposition) {
                case 1:
                    tempPlan[cTo-1].cost=1;
                    break;
                case 2:
                    tempPlan[cTo-1].cost=2;
                    break;
                case 3:
                    tempPlan[cTo-1].cost=3;
                    break;
                }
                tempPlan[cTo-1].end=tempPlan[cTo-1].start+tempPlan[cTo-1].cost;

            qDebug()<<"DFS from"<<cFrom<<"to"<<cTo;
            DFS(cTo-1,totalCost);
            throughCity[cTo-1]=false;
            totalCost-=add;
        }
        else if(timeLimite>=totalCost&&cTo==to+1)
        {

            qDebug("find destination!");
            //风险cost
            calculateCost(crossFlag,cFrom-1,query.value(1).toInt(),query.value(2).toInt());
            distance[to]=distance[cFrom-1]+cost;

            arriveTime[to][0]=query.value(1).toInt()+query.value(2).toInt();
            arriveTime[to][1]=0;
            arriveTime[to][2]=arriveTime[cFrom-1][2];
            if(crossFlag)
                arriveTime[to][2]++;

            currentTime[0]=arriveTime[to][0];
            currentTime[1]=arriveTime[to][1];
            currentTime[2]=arriveTime[to][2];

           tempPlan[to].from=cFrom;
            tempPlan[to].to=cTo;
            tempPlan[to].start=query.value(1).toInt();
            tempPlan[to].transposition=query.value(2).toInt();
            switch (tempPlan[to].transposition) {
            case 1:
                tempPlan[to].cost=1;
                break;
            case 2:
                tempPlan[to].cost=2;
                break;
            case 3:
                tempPlan[to].cost=3;
                break;
            }
            tempPlan[to].end=tempPlan[to].start+tempPlan[to].cost;


            if(distance[to]<minCost)
            {
                minCost=distance[to];
                for(int i=0;i<10;i++)
                    plan[i]=tempPlan[i];
                success=true;
                qDebug("arrive destination and update!");
            }            
        }        
}
}

void Widget::Djkstra()
{
    int currentCity=from;
    while(1)
    {
        //更新相邻节点的值
        updateCity(currentCity);
        int oldCity=currentCity;
        currentCity=-1;

        double minCost=INT_MAX;
        for(int i=0;i<10;i++)
            if(!throughCity[i]&&minCost>distance[i])
            {
                minCost=distance[i];
                currentCity=i;
            }

        qDebug()<<"update path from"<<oldCity+1<<"to"<<currentCity+1;
        if(currentCity==to)
            break;
        throughCity[currentCity]=true;
        currentTime[0]=arriveTime[currentCity][0];
        currentTime[1]=arriveTime[currentCity][1];
        currentTime[2]=arriveTime[currentCity][2];

    }
    success=true;
}

void Widget::Makeplan(int city)
{

    if(city!=from)
    {
        path.push(city);
        mapPath.push(city);
        Makeplan(plan[city].from-1);
    }
}

void Widget::updateCity(int currentCity)
{
    currentFrom=currentCity+1;
    bool crossFlag=false;//是否跨天
    bool overFlag=true;//是否为整数小时
    QString sql;
    QSqlQuery query;
    QString sCurrentCity=QString::number(currentFrom,10);
    QString sCurrentTime=QString::number(currentTime[0],10);

    if(currentTime[1]>0)
        overFlag=false;
    sql="select to_id,min(start),transpotation from time_table_id where from_id=";
    sql.append(sCurrentCity);
    sql.append(" and start>");
    if(overFlag)
       sql.append("=");
    sql.append(sCurrentTime);
    sql.append(" group by(to_id)");
    qDebug()<<sql;

    query=mysql.selectCity(sql);
    if(query.size()==0)
    {
       crossFlag=true;
       sql="select to_id,min(start),transpotation from time_table_id where from_id=";
       sql.append(sCurrentCity);
       sql.append(" group by(to_id)");
       query=mysql.selectCity(sql);
    }

    while(query.next()){

        currentTo=query.value(0).toInt();
        calculateCost(crossFlag,currentFrom-1,query.value(1).toInt(),query.value(2).toInt());

        qDebug()<<"find path from"<<currentFrom<<"to"<<currentTo;
        //更新
        if(!throughCity[currentTo-1]&&distance[currentTo-1]>distance[currentFrom-1]+cost)
        {
            qDebug()<<"update city from"<<currentFrom<<"to"<<currentTo;
            distance[currentTo-1]=distance[currentFrom-1]+cost;
            plan[currentTo-1].from=currentFrom;
            plan[currentTo-1].to=currentTo;
            plan[currentTo-1].start=query.value(1).toInt();
            plan[currentTo-1].transposition=query.value(2).toInt();
            switch (plan[currentTo-1].transposition) {
            case 1:
                plan[currentTo-1].cost=1;
                break;
            case 2:
                plan[currentTo-1].cost=2;
                break;
            case 3:
                plan[currentTo-1].cost=3;
                break;
            }
            plan[currentTo-1].end=plan[currentTo-1].start+plan[currentTo-1].cost;
            arriveTime[currentTo-1][0]=plan[currentTo-1].end;
            arriveTime[currentTo-1][1]=0;
            arriveTime[currentTo-1][2]=arriveTime[currentFrom-1][2];
            if(crossFlag)
                arriveTime[currentTo-1][2]++;
        }

    }
}

void Widget::calculateCost(bool flag, int from,int start,int strategy){
    if(flag)
        costTime[0]=24+start-arriveTime[from][0];
    else
        costTime[0]=start-arriveTime[from][0];
    costTime[1]=0-arriveTime[from][1];
    if(costTime[1]<0)
    {
        costTime[1]+=60;
        costTime[0]--;
    }
    cost=(costTime[1]/60+costTime[0])*value[from];
    switch (strategy) {
    case 1:
        cost+=strategy*9*value[from];
        break;
    case 2:
        cost+=strategy*5*value[from];
        break;
    case 3:
        cost+=strategy*2*value[from];
        break;
    }
}

double Widget::calculateTotalCost(bool flag, int from,int start)
{
    if(flag)
        costTime[0]=24+start-arriveTime[from][0];
    else
        costTime[0]=start-arriveTime[from][0];
    costTime[1]=0-arriveTime[from][1];
    if(costTime[1]<0)
    {
        costTime[1]+=60;
        costTime[0]--;
    }
    return costTime[0]+costTime[1]/60;
}

void Widget::on_pushButton_clicked()
{
    hour++;
    if(hour==24)
    {
        hour=0;
        day++;
    }
}
