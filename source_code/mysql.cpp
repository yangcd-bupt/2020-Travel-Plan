#include "mysql.h"

MySql::MySql()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("travel_plan");//数据库名
    db.setUserName("root");
    db.setPassword("");   //密码
    if (!db.open()) {
    QMessageBox::critical(0, QObject::tr("无法打开数据库"),
    "无法创建数据库连接！ ", QMessageBox::Cancel);

    }
}

QSqlQuery MySql::selectCity(QString sql)
{
    QSqlQuery query(db);
    query.exec(sql);
    return query;
}
