#ifndef MYDLG2_H
#define MYDLG2_H

#include <QDialog>
#include "ui_dialog2.h"

class MyDlg2:public QDialog, public Ui::Dialog2
{
    Q_OBJECT

public:
    MyDlg2(QWidget *parent=0)
        :QDialog(parent)
    {
        setupUi(this);

        layout()->setSizeConstraint(QLayout::SetFixedSize);

        comboBox->clear();
        comboBox_3->clear();

        for(QChar c='A';c<='Z';c =c.unicode()+1)
        {
            comboBox->addItem(QString(c));
            comboBox_3->addItem(QString(c));
        }
    }

};

#endif // MYDLG2_H
