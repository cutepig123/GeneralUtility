#include "widget.h"
#include "ui_widget.h"
#include <QtCore>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //http://stackoverflow.com/questions/1357960/qt-jpg-image-display
    image = new QPixmap("F:\\wamp\\www\\git_repos\\GitHub\\GeneralUtility\\TestQt\\testFastPaint\\pattern\\0.bmp");

    ui->label->setPixmap(*image);

    this->setMouseTracking(true);
}

Widget::~Widget()
{
    delete ui;
}

//http://stackoverflow.com/questions/24908213/show-grey-level-value-of-pixel-in-qt-when-the-mouse-is-over-it
//It's not straight forward to catch mose move event
// seems connect() cannot be used
// need overload label's v function
void Widget::mouseMoveEvent(QMouseEvent * event)
{
    //qDebug()<<"xx";

    QPoint  local_pt = ui->label->mapFromGlobal(event->globalPos());
   //QPointF img_coord_pt = ui->graphicsView->mapToScene(local_pt);

   double x = local_pt.x();
   double y = local_pt.y();

   /* How can I get a gray level image here */
   QRgb rgbValue = image->toImage().pixel(x,y);

   QString text=QString("Color %1").arg(rgbValue);

   ui->label_2 ->setText(text);

}
