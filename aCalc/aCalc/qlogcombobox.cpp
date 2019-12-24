#include "qlogcombobox.h"

QLogComboBox::QLogComboBox(QWidget *parent) : QComboBox(parent), sg(this)
{
    setFixedHeight(height());
    this->setFont(QFont("arial", 10));
    tv.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tv.horizontalHeader()->hide();


    this->setView(&tv);
    this->view()->setCornerWidget(&sg);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


}
