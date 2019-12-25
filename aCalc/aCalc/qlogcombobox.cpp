#include "qlogcombobox.h"

QLogComboBox::QLogComboBox(QWidget *parent) : QComboBox(parent), sg(this)
{
    this->setFixedHeight(height());
    this->setStyleSheet("QComboBox { combobox-popup: 0 }");
    this->setFont(QFont("arial", 10));
    tv.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tv.horizontalHeader()->hide();


    this->setView(&tv);
    this->view()->setCornerWidget(&sg);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //this->setStyleSheet("QAbstractItemView {margin-top: 0px; padding-bottom: 0px;");

}
