#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>

//http://www.qtcentre.org/threads/6929-Any-fast-way(-lt-10ms)-to-display-640*480-QImage-on-QGraphicsScene

class ImageSource : QObject
{    Q_OBJECT
public:
    ImageSource(QString text = "Hello!",int w = 640,int h = 480) : m_width(w),m_height(h)
    {
        if (text.length() == 0) text = "Hello!";
        // Create pixmaps for each character
        for (int i=0;i<text.length();++i)
        {
            QString imageText(text.length(),' ');
            imageText[i] = text.at(i);
			
			//create pixmap
            QPixmap* pm = new QPixmap(w,h);
            pm->fill(Qt::white);
			
			//fill pixmap using painter
            QPainter p(pm);
            p.setFont(QFont("Courier"));
            p.setPen(Qt::black);
            p.drawText(QRectF(0,0,w,h),Qt::AlignCenter,imageText);
			
			//add pixmap to vector
            dummies.append(pm);
        }
        current = dummies.length();
    }
    QPixmap* nextImage()
    {
        --current;
        if (current < 0)
            current = dummies.length()-1;
        //return dummies.at(3);
        return dummies.at(current);
    }
    int width() {return m_width;}
    int height() {return m_height;}
private:
    QList<QPixmap* > dummies;
    int current;
    int m_width;
    int m_height;
};

class MainWindow : public QMainWindow
{    Q_OBJECT
public:
    MainWindow()
    {
		// create scene
        scene = new QGraphicsScene(this);

        imageSrc = new ImageSource();

		// add pixmapitem to scene
        pixmapItem = new QGraphicsPixmapItem();
        scene->addItem(pixmapItem);

		// create view based on scene
        view = new QGraphicsView(scene);
        //view->setViewport(new QGLWidget(QGLFormat()));
        //view->setScene(scene);

        setCentralWidget(view);

		// create timer to update the pixmapItem as fast as possible
        QTimer* updateTimer = new QTimer(this);
        QObject::connect(updateTimer,SIGNAL(timeout()),this,SLOT(doUpdate()));
        updateTimer->start(1);

        QTimer* fpsTimer = new QTimer(this);
        QObject::connect(fpsTimer,SIGNAL(timeout()),this,SLOT(showFPS()));
        fpsTimer->start(1000);
    }
    ~MainWindow() {}
protected slots:
    void doUpdate()
    {
        pixmapItem->setPixmap(*imageSrc->nextImage());
        ++frameCounter;
    }
    void showFPS()
    {
        setWindowTitle(QString("FPS: %1").arg(frameCounter));
        frameCounter = 0;
    }

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    ImageSource* imageSrc;
    QGraphicsPixmapItem* pixmapItem;
    int frameCounter;
};

#endif // MAINWINDOW_H
