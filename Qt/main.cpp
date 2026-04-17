#include "mainwindow.h"

#include <QApplication>

#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDateTime>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");


    db.setHostName("localhost");
    db.setDatabaseName("recko");
    db.setUserName("miha907");
    db.setPassword("miha907miha907");


    if (!db.open())
    {
        qDebug() << "Greška! Baza nije povezana";
        qDebug() << db.lastError().text();
        return -1;
    }

    qDebug() << "Baza je povezana!";

    return a.exec();
}
