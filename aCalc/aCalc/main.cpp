#include <QApplication>


#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("acalc");
    QCoreApplication::setApplicationName("acalc");
    QCoreApplication::setApplicationVersion("0.3");

    MainWidget w;
    w.show();
    
    return a.exec();
}
