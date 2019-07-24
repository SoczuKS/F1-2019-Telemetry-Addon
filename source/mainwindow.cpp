#include "../headers/mainwindow.hpp"
#include "../ui_mainwindow.h"

#include <qshortcut.h>
#include <qmessagebox.h>

#include <Windows.h>

#include "../headers/config.hpp"

#include <thread>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	setWindowFlags(Qt::Widget);

    ui->setupUi(this);

	stackedWidget = ui->stackedWidget;

	QObject::connect(ui->startButton, &QPushButton::released, this, &MainWindow::start);
	QObject::connect(ui->optionsButton, &QPushButton::released, this, &MainWindow::options);
	QObject::connect(ui->quitButton, &QPushButton::released, this, &QApplication::quit);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::start() {
	this->close();

	overlay = new Overlay;
	overlay->showFullScreen();
}

void MainWindow::options() {
	stackedWidget->setCurrentIndex(1);
}
