#pragma once

#include <QObject>

#include <QUdpSocket>

#include <queue>
#include <mutex>

#include "packet.hpp"

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

	void stop();

private:
    bool running = true;
    std::shared_ptr<QUdpSocket> socket;

	void analyzePacket(Packet);

	void analyzeCarSetups(CarSetupPacket&);
	void analyzeCarStatus(CarStatusPacket&);
	void analyzeCarTelemetry(CarTelemetryPacket&);
	void analyzeEvent(EventPacket&);
	void analyzeLapData(LapDataPacket&);
	void analyzeMotion(MotionPacket&);
	void analyzeParticipants(ParticipantsPacket&);
	void analyzeSession(SessionPacket&);

	std::queue<Packet> receivedPackets;
	std::mutex receivedPacketsMutex;
    std::thread analyzeThread;
	std::thread receiveThread;
};
