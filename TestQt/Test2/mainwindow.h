#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class Shape
{
public:

        enum Code {
                Line,
                Rect
        };

        Shape(){}

        void setStart(QPoint s)
        {
                start = s;
        }

        void setEnd(QPoint e)
        {
                end = e;
        }

        QPoint startPoint()
        {
                return start;
        }

        QPoint endPoint()
        {
                return end;
        }

        void virtual paint(QPainter & painter) = 0;

protected:
        QPoint start;
        QPoint end;
};


class PaintWidget : public QWidget
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
        void paintEvent(QPaintEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);

private:
        Shape::Code currShapeCode;
        Shape *shape;
        bool perm;
        QList<Shape*> shapeList;
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
