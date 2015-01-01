#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qtgui/QtGui>
#include <QtWidgets/QtWidgets>


class Shape
{
public:

        enum Code {
                Line,
                Rect
        };

        Shape(){;}

        virtual void startDraw(QGraphicsSceneMouseEvent * event) = 0;
        virtual void drawing(QGraphicsSceneMouseEvent * event) = 0;
};

class PaintWidget : public QGraphicsScene
{
        Q_OBJECT

public:
        PaintWidget(QWidget *parent = 0);

public slots:
        void setCurrentShape(Shape::Code s)
        {
                if(s != currShapeCode) {
                        currShapeCode = s;
                }
        }

protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *event);
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
        Shape::Code currShapeCode;
        Shape *currItem;
        bool perm;
};


class MainWindow : public QMainWindow
{
        Q_OBJECT

public:
        MainWindow(QWidget *parent = 0);

signals:
        void changeCurrentShape(Shape::Code newShape);

private slots:
        void drawLineActionTriggered();
        void drawRectActionTriggered();

};

#endif // MAINWINDOW_H
