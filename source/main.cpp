#include "../headers/mainwindow.hpp"

#include <QApplication>
#include <QFontDatabase>

#include "../headers/config.hpp"

int main(int argc, char *argv[]) {
    Config::instance();

	QFontDatabase::addApplicationFont(":/fonts/Formula1-Bold.otf");
	QFont font = QFont("Formula1 Display Regular");

    QApplication a(argc, argv);
	a.setFont(font);
    MainWindow w;
    w.show();

    int result = a.exec();

    return result;
}
