#include "mylabel.h"

MyLabel::MyLabel(const QString& label)
{
    this->setText(label);
    this->setStyleSheet("border: 2px solid black");
}
