#include "mainwindow.h"
#include <Qtgui/QtGui>
#include <QtWidgets/QtWidgets>
#include <QApplication>
#include <QLabel>
//http://devbean.blog.51cto.com/448512/243546


class Line : public Shape, public QGraphicsLineItem
{
public:
        Line();

        void startDraw(QGraphicsSceneMouseEvent * event);
        void drawing(QGraphicsSceneMouseEvent * event);
};

Line::Line()
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void Line::startDraw(QGraphicsSceneMouseEvent * event)
{
        setLine(QLineF(event->scenePos(), event->scenePos()));
}

void Line::drawing(QGraphicsSceneMouseEvent * event)
{
        QLineF newLine(line().p1(), event->scenePos());
        setLine(newLine);
}


class Rect : public Shape, public QGraphicsRectItem
{
public:
        Rect();

        void startDraw(QGraphicsSceneMouseEvent * event);
        void drawing(QGraphicsSceneMouseEvent * event);
};

Rect::Rect()
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void Rect::startDraw(QGraphicsSceneMouseEvent * event)
{
        setRect(QRectF(event->scenePos(), QSizeF(0, 0)));
}

void Rect::drawing(QGraphicsSceneMouseEvent * event)
{
        QRectF r(rect().topLeft(),
                         QSizeF(event->scenePos().x() - rect().topLeft().x(), event->scenePos().y() - rect().topLeft().y()));
        setRect(r);
}

PaintWidget::PaintWidget(QWidget *parent)
        : QGraphicsScene(parent), currShapeCode(Shape::Line), currItem(NULL), perm(false)
{

}

void PaintWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
        switch(currShapeCode)
        {
        case Shape::Line:
                {
                        Line *line = new Line;
                        currItem = line;
                        addItem(line);
                        break;
                }
        case Shape::Rect:
                {
                        Rect *rect = new Rect;
                        currItem = rect;
                        addItem(rect);
                        break;
                }
        }
        if(currItem) {
                currItem->startDraw(event);
                perm = false;
        }
        QGraphicsScene::mousePressEvent(event);
}

void PaintWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
        if(currItem && !perm) {
                currItem->drawing(event);
        }
        QGraphicsScene::mouseMoveEvent(event);
}

void PaintWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
        perm = true;
        QGraphicsScene::mouseReleaseEvent(event);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
