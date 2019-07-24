#pragma once

#include <QMainWindow>
#include <QStackedWidget>

#include "../headers/overlay.hpp"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void start();
	void options();

private:
    Ui::MainWindow *ui;

	Overlay* overlay;

	QStackedWidget* stackedWidget;
};
