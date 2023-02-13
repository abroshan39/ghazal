#include "common_functions.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    argumentParser(argc, argv);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
