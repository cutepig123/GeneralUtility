#include <QtWidgets/QApplication>
#include <QtWidgets/QtWidgets>
//#include <QtWidgets/QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QLabel *label =new QLabel("<h2> <i>Hello </i><font color=red>Qt!</font> <a href=www.qq.com>QQ</a></h2>");
   // label->show();

    QPushButton *button=new QPushButton("Quit");
    QObject::connect(button, SIGNAL(clicked()), &a, SLOT(quit()));
//    button->show();

    QSpinBox *spin=new QSpinBox;
    QSlider *slider=new QSlider(Qt::Horizontal);
    slider->setRange(0,130);
    spin->setRange(0,130);
    QObject::connect(slider, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));
    QObject::connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));

    QHBoxLayout *hlayout =new QHBoxLayout;
    hlayout->addWidget(spin);
    hlayout->addWidget(slider);

    QVBoxLayout *layout =new QVBoxLayout();
    layout->addWidget(label);
    layout->addWidget(button);
    layout->addLayout(hlayout);

    QWidget *widget=new QWidget;
    widget->setLayout(layout);
    widget->show();

    qDebug() << QStyleFactory::keys().join(" ");

    return a.exec();
}
