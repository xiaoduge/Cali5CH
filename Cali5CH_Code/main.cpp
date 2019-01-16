#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator trans;
    if(trans.load("myapp.qm"))
    {
        a.installTranslator(&trans);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
