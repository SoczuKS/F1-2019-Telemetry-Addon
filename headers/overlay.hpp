#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>

#include <memory>

#include "../headers/server.hpp"

namespace Ui {
	class Overlay;
}

class Overlay : public QMainWindow {
	Q_OBJECT

public:
	explicit Overlay(QWidget* parent = nullptr);
	~Overlay();

public slots:
	void updateHeaderInfo(PacketHeader packet);
	void updateCarStatusOverlay(CarStatusPacket packet);
	void updateCarSetupsOverlay(CarSetupPacket packet);
	void updateCarTelemetryOverlay(CarTelemetryPacket packet);
	void updateEventOverlay(EventPacket packet);
	void updateLapDataOverlay(LapDataPacket packet);
	void updateMotionOverlay(MotionPacket packet);
	void updateParticipantsOverlay(ParticipantsPacket packet);
	void updateSessionOverlay(SessionPacket packet);

private:
	Ui::Overlay* ui;
	void setupUi();

	bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

	Server* server;

	QLabel* airTemp;
	QLabel* trackTemp;
	QLabel* fuelMode;
	QLabel* fuelLaps;
	QLabel* ersMode;
	QLabel* ersCharge;
	QLabel* gear;
	QLabel* rpm;
	QLabel* speed;
	QLabel* topSpeed;
	QLabel* weather;
	QLabel* drs;
	QLabel* frontLeftWingDamage;
	QLabel* frontRightWingDamage;
	QLabel* rearWingDamage;
	QLabel* engineDamage;
	QLabel* gearboxDamage;
	QLabel* frontRightWear;
	QLabel* frontRightDamage;
	QLabel* frontLeftWear;
	QLabel* frontLeftDamage;
	QLabel* rearRightWear;
	QLabel* rearRightDamage;
	QLabel* rearLeftWear;
	QLabel* rearLeftDamage;

	QProgressBar* throttle;
	QProgressBar* brake;

	std::array<Participant, 20> participants;
	Participant* me;

	uint8 playerCarIndex;
	uint16 topSpeedI = 0;
};
