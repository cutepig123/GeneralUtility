//#include "mainwindow.h"
//#include <QApplication>

//int mainx(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
//}

#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
        QLabel *label = new QLabel("Hello, world!");
        label->show();
        return a.exec();
}
