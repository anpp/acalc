#ifndef HAVE_QT5
#include <QtGui/QApplication>
#else
#include <QApplication>
#endif

#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;
    w.show();
    
    return a.exec();
}
