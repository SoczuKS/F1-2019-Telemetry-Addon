#include "../headers/overlay.hpp"
#include "../ui_overlay.h"

#include "../headers/config.hpp"
#include "../headers/mainwindow.hpp"

#include <QMessageBox>

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

Q_DECLARE_METATYPE(CarStatusPacket)
Q_DECLARE_METATYPE(CarSetupPacket)
Q_DECLARE_METATYPE(CarTelemetryPacket)
Q_DECLARE_METATYPE(EventPacket)
Q_DECLARE_METATYPE(LapDataPacket)
Q_DECLARE_METATYPE(MotionPacket)
Q_DECLARE_METATYPE(ParticipantsPacket)
Q_DECLARE_METATYPE(SessionPacket)
Q_DECLARE_METATYPE(PacketHeader)

Overlay::Overlay(QWidget* parent) : QMainWindow(parent), ui(new Ui::Overlay) {
	setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_TransparentForMouseEvents);

	server = new Server;

	qRegisterMetaType<CarStatusPacket>();
	qRegisterMetaType<CarSetupPacket>();
	qRegisterMetaType<CarTelemetryPacket>();
	qRegisterMetaType<EventPacket>();
	qRegisterMetaType<LapDataPacket>();
	qRegisterMetaType<MotionPacket>();
	qRegisterMetaType<ParticipantsPacket>();
	qRegisterMetaType<SessionPacket>();
	qRegisterMetaType<PacketHeader>();

	QObject::connect(server, SIGNAL(carStatusUpdate(CarStatusPacket)), this, SLOT(updateCarStatusOverlay(CarStatusPacket)));
	QObject::connect(server, SIGNAL(carSetupsUpdate(CarSetupPacket)), this, SLOT(updateCarSetupsOverlay(CarSetupPacket)));
	QObject::connect(server, SIGNAL(carTelemetryUpdate(CarTelemetryPacket)), this, SLOT(updateCarTelemetryOverlay(CarTelemetryPacket)));
	QObject::connect(server, SIGNAL(eventUpdate(EventPacket)), this, SLOT(updateEventOverlay(EventPacket)));
	QObject::connect(server, SIGNAL(lapDataUpdate(LapDataPacket)), this, SLOT(updateLapDataOverlay(LapDataPacket)));
	QObject::connect(server, SIGNAL(motionUpdate(MotionPacket)), this, SLOT(updateMotionOverlay(MotionPacket)));
	QObject::connect(server, SIGNAL(participantsUpdate(ParticipantsPacket)), this, SLOT(updateParticipantsOverlay(ParticipantsPacket)));
	QObject::connect(server, SIGNAL(sessionUpdate(SessionPacket)), this, SLOT(updateSessionOverlay(SessionPacket)));
	QObject::connect(server, SIGNAL(headerUpdate(PacketHeader)), this, SLOT(updateHeaderInfo(PacketHeader)));

	this->setupUi();

	HWND h = HWND(winId());
	for (auto s : Config::instance().getShortcuts()) {
		int id = static_cast<int>(s.first);
		UINT m = s.second.modifiers;
		UINT k = s.second.key;

		if (!RegisterHotKey(h, id, m, k)) {
			std::string msg = "Cannot create hotkey with id " + std::to_string(static_cast<int>(s.first));
			QMessageBox::warning(this, "WARNING", msg.c_str());
		}
	}
}

void Overlay::setupUi() {
	ui->setupUi(this);

	Config& cfg = Config::instance();

	ui->carStatusWidget->move(cfg.getCarStatusWidgetPosition().x(), cfg.getCarStatusWidgetPosition().y());
	ui->sessionStatusWidget->move(cfg.getSessionWidgetPosition().x(), cfg.getSessionWidgetPosition().y());
	ui->timeStatusWidget->move(cfg.getTimeStatusWidgetPosition().x(), cfg.getTimeStatusWidgetPosition().y());
	ui->carConditionWidget->move(cfg.getCarConditionWidgetPosition().x(), cfg.getCarConditionWidgetPosition().y());

#pragma region carStatus
#pragma region pedals
	throttle = ui->throttle;
	brake = ui->brake;
#pragma endregion

#pragma region weather
	trackTemp = ui->track;
	airTemp = ui->air;
	weather = ui->weather;
#pragma endregion

#pragma region maintenance
#pragma region ers
	ersCharge = ui->charge;
	ersMode = ui->ersMode;
#pragma endregion
#pragma region fuel
	fuelMode = ui->fuelMode;
	fuelLaps = ui->fuelLaps;
#pragma endregion
#pragma region drs
	drs = ui->drs;
#pragma endregion
#pragma endregion

#pragma region engine
	gear = ui->gear;
	rpm = ui->rpm;
	speed = ui->speed;
	topSpeed = ui->topSpeed;
#pragma endregion
#pragma endregion

#pragma region carCondition

#pragma region tyres
#pragma region front
	frontLeftDamage = ui->frontLeftDamage;
	frontRightDamage = ui->frontRightDamage;
	frontLeftWear = ui->frontLeftWear;
	frontRightWear = ui->frontRightWear;
#pragma endregion
#pragma region rear
	rearLeftDamage = ui->rearLeftDamage;
	rearRightDamage = ui->rearRightDamage;
	rearLeftWear = ui->rearLeftWear;
	rearRightWear = ui->rearRightWear;
#pragma endregion
#pragma endregion

#pragma region wing
	frontLeftWingDamage = ui->frontLeftWingDamage;
	frontRightWingDamage = ui->frontRightWingDamage;
	rearWingDamage = ui->rearWingDamage;
#pragma endregion

#pragma region
	engineDamage = ui->engineDamage;
	gearboxDamage = ui->gearboxDamage;
#pragma endregion
#pragma endregion
}

Overlay::~Overlay() {
	delete ui;
}

bool Overlay::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	Q_UNUSED(eventType);
	Q_UNUSED(result);

	MSG* msg = static_cast<MSG*>(message);

	if (msg->message == WM_HOTKEY) {
		switch (static_cast<ShortcutId>(msg->wParam)) {
			case ShortcutId::CLOSE: {
				HWND h = HWND(winId());
				for (auto s : Config::instance().getShortcuts()) {
					int id = static_cast<int>(s.first);

					UnregisterHotKey(h, id);
				}
				this->close();
				server->stop();
				(new MainWindow)->show();
			}
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

void Overlay::updateHeaderInfo(PacketHeader header) {
	playerCarIndex = header.playerCarIndex;
}

void Overlay::updateCarStatusOverlay(CarStatusPacket packet) {
	std::stringstream str;
#pragma region MyCarOnly
	auto carStatus = packet.carStatus[playerCarIndex];

	fuelMode->setText((std::to_string(static_cast<int>(carStatus.fuelMix))).c_str());

	str << std::fixed << std::setprecision(1) << carStatus.fuelRemainingLaps;
	fuelLaps->setText(str.str().c_str());
	str.flush();

	ersMode->setText((std::to_string(static_cast<int>(carStatus.ersDeployMode))).c_str());

	float ersPercentage = (static_cast<float>(carStatus.ersStoreEnergy) / 4000000.0f) * 100.0f;
	str << std::fixed << std::setprecision(1) << ersPercentage;
	ersCharge->setText(str.str().c_str());
	str.flush();

	frontLeftWingDamage->setText((std::to_string(carStatus.frontLeftWingDamage)).c_str());
	frontRightWingDamage->setText((std::to_string(carStatus.frontRightWingDamage)).c_str());

	rearWingDamage->setText((std::to_string(carStatus.rearWingDamage)).c_str());

	engineDamage->setText((std::to_string(carStatus.engineDamage)).c_str());
	gearboxDamage->setText((std::to_string(carStatus.gearboxDamage)).c_str());

	frontRightWear->setText((std::to_string(carStatus.tyresWear[static_cast<uint8>(Tyres::FRONT_RIGHT)])).c_str());
	frontRightDamage->setText((std::to_string(carStatus.tyresDamage[static_cast<uint8>(Tyres::FRONT_RIGHT)])).c_str());

	frontLeftWear->setText((std::to_string(carStatus.tyresWear[static_cast<uint8>(Tyres::FRONT_LEFT)])).c_str());
	frontLeftDamage->setText((std::to_string(carStatus.tyresDamage[static_cast<uint8>(Tyres::FRONT_LEFT)])).c_str());

	rearRightWear->setText((std::to_string(carStatus.tyresWear[static_cast<uint8>(Tyres::REAR_RIGHT)])).c_str());
	rearRightDamage->setText((std::to_string(carStatus.tyresDamage[static_cast<uint8>(Tyres::REAR_RIGHT)])).c_str());

	rearLeftWear->setText((std::to_string(carStatus.tyresWear[static_cast<uint8>(Tyres::REAR_LEFT)])).c_str());
	rearLeftDamage->setText((std::to_string(carStatus.tyresDamage[static_cast<uint8>(Tyres::REAR_LEFT)])).c_str());
#pragma endregion


}

void Overlay::updateCarSetupsOverlay(CarSetupPacket packet) {
#pragma region MyCarOnly

#pragma endregion


}

void Overlay::updateCarTelemetryOverlay(CarTelemetryPacket packet) {
#pragma region MyCarOnly
	auto carTelemetry = packet.carTelemetry[playerCarIndex];

	if (topSpeedI < carTelemetry.speed) {
		topSpeedI = carTelemetry.speed;
		this->topSpeed->setText(std::to_string(topSpeedI).c_str());
	}
	this->speed->setText(std::to_string(carTelemetry.speed).c_str());

	if (carTelemetry.gear == Gear::NEUTRAL) this->gear->setText("N");
	else if (carTelemetry.gear == Gear::REVERSE) this->gear->setText("R");
	else this->gear->setText(std::to_string(static_cast<int>(carTelemetry.gear)).c_str());

	this->rpm->setText(std::to_string(carTelemetry.engineRPM).c_str());

	this->throttle->setValue(carTelemetry.throttle * 100);
	this->brake->setValue(carTelemetry.brake * 100);

	this->drs->setText(carTelemetry.drs ? "ON" : "OFF");
#pragma endregion


}

void Overlay::updateEventOverlay(EventPacket packet) {
#pragma region MyCarOnly

#pragma endregion


}

void Overlay::updateLapDataOverlay(LapDataPacket packet) {
#pragma region MyCarOnly

#pragma endregion


}

void Overlay::updateMotionOverlay(MotionPacket packet) {
#pragma region MyCarOnly

#pragma endregion


}

void Overlay::updateParticipantsOverlay(ParticipantsPacket packet) {
#pragma region MyCarOnly
	participants = packet.participants;
	me = &(participants[playerCarIndex]);
#pragma endregion


}

void Overlay::updateSessionOverlay(SessionPacket packet) {
#pragma region MyCarOnly
	this->airTemp->setText(std::to_string(packet.airTemperature).c_str());
	this->trackTemp->setText(std::to_string(packet.trackTemperature).c_str());

	switch (packet.weather) {
		case Weather::CLEAR:
			this->weather->setText("CLEAR");
			break;

		case Weather::LIGHT_CLOUD:
			this->weather->setText("LIGHT CLOUD");
			break;

		case Weather::OVERCAST:
			this->weather->setText("OVERCAST");
			break;

		case Weather::LIGHT_RAIN:
			this->weather->setText("LIGHT RAIN");
			break;

		case Weather::HEAVY_RAIN:
			this->weather->setText("HEAVY RAIN");
			break;

		case Weather::STORM:
			this->weather->setText("STORM");
			break;
	}
#pragma endregion


}

