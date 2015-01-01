//#include "mainwindow.h"
//#include <QApplication>

//int mainx(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
//}

#include <Qtgui/QtGui>
#include <QtWidgets/QtWidgets>
#include <QApplication>
#include <QLabel>

class DrawApp : public QWidget {
public:
        DrawApp();
protected:
        void paintEvent(QPaintEvent *event);
};

DrawApp::DrawApp()
{

}

void DrawApp::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        painter.setWindow(0,0,300,300);
        painter.drawLine(10, 10, 150, 300);
}

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        QGraphicsScene *scene = new QGraphicsScene;
        scene->addLine(10, 10, 150, 300);
        QGraphicsView *view = new QGraphicsView(scene);
        view->resize(500, 500);
        view->setWindowTitle("Graphics View");
        view->show();

        DrawApp *da = new DrawApp;
        da->resize(500, 500);
        da->setWindowTitle("QWidget");
        da->show();
        return app.exec();
}
