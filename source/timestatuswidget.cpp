#include "../headers/timestatuswidget.h"

#include "qt_windows.h"

#include "../headers/config.hpp"

TimeStatusWidget::TimeStatusWidget(QWidget *parent) : QMainWindow(parent), ui(new Ui::TimeStatusWidget) {
	setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_TransparentForMouseEvents);

	ui->setupUi(this);
}

TimeStatusWidget::~TimeStatusWidget() {
	delete ui;
}

bool TimeStatusWidget::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	Q_UNUSED(eventType);
	Q_UNUSED(result);

	MSG* msg = static_cast<MSG*>(message);

	if (msg->message == WM_HOTKEY) {
		switch (static_cast<ShortcutId>(msg->wParam)) {
			case ShortcutId::CLOSE:
				close();
				break;

			case ShortcutId::HIDE_SHOW:
				if (isVisible()) hide();
				else show();
				break;
		}

		return true;
	}

	return false;
}
