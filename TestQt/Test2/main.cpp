#include "mainwindow.h"
#include <Qtgui/QtGui>
#include <QtWidgets/QtWidgets>
#include <QApplication>
#include <QLabel>
//http://devbean.blog.51cto.com/448512/243546


class Line : public Shape
{
public:
    Line(){}

        void paint(QPainter &painter){
            painter.drawLine(start, end);
    }
};

class Rect : public Shape
{
public:
    Rect(){;}

        void paint(QPainter &painter){
            painter.drawRect(start.x(), start.y(),
                                             end.x() - start.x(), end.y() - start.y());
    }
};




PaintWidget::PaintWidget(QWidget *parent)
        : QWidget(parent), currShapeCode(Shape::Line), shape(NULL), perm(false)
{
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void PaintWidget::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        painter.setBrush(Qt::white);
        painter.drawRect(0, 0, size().width(), size().height());
        foreach(Shape * shape, shapeList) {
                shape->paint(painter);
        }
        if(shape) {
                shape->paint(painter);
        }
}

void PaintWidget::mousePressEvent(QMouseEvent *event)
{
        switch(currShapeCode)
        {
        case Shape::Line:
                {
                        shape = new Line;
                        break;
                }
        case Shape::Rect:
                {
                        shape = new Rect;
                        break;
                }
        }
        if(shape != NULL) {
                perm = false;
                shapeList<<shape;
                shape->setStart(event->pos());
                shape->setEnd(event->pos());
        }
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
        if(shape && !perm) {
                shape->setEnd(event->pos());
                update();
        }
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event)
{
        perm = true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
