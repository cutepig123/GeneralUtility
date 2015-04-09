#include "hexspinbox.h"

HexSpinBox::HexSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
    setRange(0,255);
    validator =new QRegExpValidator(QRegExp("[0-9a-fA-f]{1,8}"),this);
}

/*virtual*/ QValidator::State HexSpinBox::validate(QString &input, int &pos) const
{
    return validator->validate(input,pos);
}
/*virtual*/ int HexSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return text.toInt(&ok,16);
}
/*virtual*/ QString HexSpinBox::textFromValue(int val) const
{
    return QString::number(val,16).toUpper();
}
