#include <QtWidgets/QApplication>
#include <QtWidgets/QtWidgets>
#include <QtUiTools/QtUiTools>
//#include <QtWidgets/QPushButton>
#include "ui_dialog.h"
#include "ui_dialog2.h"
#include "MyDlg2.h"
#include "hexspinbox.h"

template <class T>
void addDialog(QVBoxLayout *layout)
{
    QPushButton *buttonFind=new QPushButton("Find");
    T ui;
    QDialog *dlg=new QDialog;
    ui.setupUi(dlg);
    QObject::connect(buttonFind, SIGNAL(clicked()), dlg, SLOT(show()));

    layout->addWidget(buttonFind);
}

template <class T>
void addDialog2(QVBoxLayout *layout)
{
    QPushButton *buttonFind=new QPushButton("Find");

    T *dlg=new T;

    QObject::connect(buttonFind, SIGNAL(clicked()), dlg, SLOT(show()));

    layout->addWidget(buttonFind);
}

void addDialog3(QVBoxLayout *layout, QString file_name, QString btn_name)
{
    QPushButton *buttonFind=new QPushButton(btn_name);

    QUiLoader uiloader;
    QFile file(file_name);
    QWidget *dlg=uiloader.load((&file));
    //dlg->show();

    QObject::connect(buttonFind, SIGNAL(clicked()), dlg, SLOT(show()));

    layout->addWidget(buttonFind);
}

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

//    QPushButton *buttonFind=new QPushButton("Find");
//    Ui::Dialog ui;
//    QDialog *dlg=new QDialog;
//    ui.setupUi(dlg);
//    QObject::connect(buttonFind, SIGNAL(clicked()), dlg, SLOT(show()));

    HexSpinBox *hspinbox =new HexSpinBox();

    QVBoxLayout *layout =new QVBoxLayout();
    layout->addWidget(label);
    layout->addWidget(button);
    layout->addLayout(hlayout);
    layout->addWidget(hspinbox);
//    layout->addWidget(buttonFind);

    addDialog< Ui::Dialog>(layout);
    addDialog< Ui::Dialog2>(layout);
    addDialog2< MyDlg2>(layout);
    addDialog3(layout,"dialog2.ui","dynamic load");

    QWidget *widget=new QWidget;
    widget->setLayout(layout);
    widget->show();

    qDebug() << QStyleFactory::keys().join(" ");

    return a.exec();
}
