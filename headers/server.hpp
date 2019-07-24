#pragma once

#include <QObject>

#include <QUdpSocket>
#include <QTcpSocket>

#include <queue>
#include <mutex>

#include "packet.hpp"

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

	void stop();

public slots:
	void tcpDisconnected() { this->tcpConnected = false; }

signals:
	void headerUpdate(PacketHeader header);
	void carStatusUpdate(CarStatusPacket packet);
	void carSetupsUpdate(CarSetupPacket packet);
	void carTelemetryUpdate(CarTelemetryPacket packet);
	void eventUpdate(EventPacket packet);
	void lapDataUpdate(LapDataPacket packet);
	void motionUpdate(MotionPacket packet);
	void participantsUpdate(ParticipantsPacket packet);
	void sessionUpdate(SessionPacket packet);

private:
    bool running = true;
    std::shared_ptr<QUdpSocket> udpSocket;
	//std::shared_ptr<QTcpSocket> tcpSocket;
	QTcpSocket* tcpSocket;

	uint64 sessionUid = 0;

	bool participantsNeedUpdate;

	void analyzePacket(Packet);

	void analyzeHeader(PacketHeader&);
	void analyzeCarSetups(CarSetupPacket&);
	void analyzeCarStatus(CarStatusPacket&);
	void analyzeCarTelemetry(CarTelemetryPacket&);
	void analyzeEvent(EventPacket&);
	void analyzeLapData(LapDataPacket&);
	void analyzeMotion(MotionPacket&);
	void analyzeParticipants(ParticipantsPacket&);
	void analyzeSession(SessionPacket&);

	std::queue<Packet> receivedPackets;
	std::queue<Packet> packetsToSend;

	std::mutex receivedPacketsMutex;
	std::mutex packetsToSendMutex;
	std::mutex tcpConnectedMutex;

	bool tcpConnected = false;

    std::thread analyzeThread;
	std::thread receiveThread;
	std::thread tcpSenderThread;
	std::thread tcpConnectingThread;
};
