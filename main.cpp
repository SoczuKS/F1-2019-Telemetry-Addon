#include "mainwindow.h"
#include <QApplication>

#include "headers/config.hpp"
#include "headers/server.hpp"

int main(int argc, char *argv[])
{
    Config::instance();

    Server server;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int result = a.exec();

    server.stop();

    return result;
}
