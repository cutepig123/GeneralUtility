#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <QSpinBox>

class HexSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit HexSpinBox(QWidget *parent = 0);

protected:
    virtual QValidator::State validate(QString &input, int &pos) const;
    virtual int valueFromText(const QString &text) const;
    virtual QString textFromValue(int val) const;

private:
    QRegExpValidator    *validator;
signals:

public slots:

};

#endif // HEXSPINBOX_H
