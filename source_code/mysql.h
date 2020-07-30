#ifndef CONNECTION_H
#define CONNECTION_H
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class MySql
{
public :
    MySql();

    QSqlQuery selectCity(QString sql);
private:
    QSqlDatabase db;
};


#endif // CONNECTION_H
