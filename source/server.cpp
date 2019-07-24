#include "../headers/server.hpp"

#include "../headers/config.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <iostream>

Server::Server(QObject *parent) : QObject(parent) {
	udpSocket = std::make_shared<QUdpSocket>(this);
	tcpSocket = new QTcpSocket(this);

    udpSocket->bind(QHostAddress::LocalHost, Config::instance().getPort());

    auto analyzer = [this]() {
        while (running) {
            std::lock_guard<std::mutex> receivedGuard(receivedPacketsMutex);
            if (receivedPackets.size() > 0) {
                analyzePacket(receivedPackets.front());

				std::lock_guard<std::mutex> toSendGuard(packetsToSendMutex);
				packetsToSend.push(receivedPackets.front());

                receivedPackets.pop();
            }
        }
    };

	QObject::connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));

	auto receiver = [this]() {
		while (running) {
			Packet packet;
			long long packetSize;

			QHostAddress sender;
			quint16 senderPort;

			packetSize = udpSocket->readDatagram(packet.rawData, MaxSize, &sender, &senderPort);

			if (packetSize > 0) {
				std::lock_guard<std::mutex> guard(receivedPacketsMutex);
				receivedPackets.push(packet);
			}
		}
	};

	auto tcpConnecting = [this]() {
		tcpSocket->connectToHost(Config::instance().getServerAddress().c_str(), Config::instance().getPort());
		if (tcpSocket->waitForConnected(10000)) {
			tcpConnected = true;
		}
	};

	auto tcpSender = [this, tcpConnecting]() {
		while (running) {
			if (tcpConnected) {
				std::lock_guard<std::mutex> toSendGuard(packetsToSendMutex);
				if (packetsToSend.size() > 0) {
					tcpSocket->write(packetsToSend.front().rawData);
					packetsToSend.pop();
				}
			} else {
				tcpConnectingThread = std::thread(tcpConnecting);
				tcpConnectingThread.join();
			}
		}
	};

	receiveThread = std::thread(receiver);
    analyzeThread = std::thread(analyzer);
	tcpSenderThread = std::thread(tcpSender);
}

void Server::stop() {
	running = false;
	receiveThread.join();
    analyzeThread.join();
	tcpSenderThread.join();
}

void Server::analyzePacket(Packet packet) {
	analyzeHeader(packet.header);

	switch (packet.header.packetId) {
		case PacketID::PACKET_CARSETUPS:
			analyzeCarSetups(packet.carSetup);
			break;

		case PacketID::PACKET_CARSTATUS:
			analyzeCarStatus(packet.carStatus);
			break;

		case PacketID::PACKET_CARTELEMETRY:
			analyzeCarTelemetry(packet.carTelemetry);
			break;

		case PacketID::PACKET_EVENT:
			analyzeEvent(packet.event);
			break;

		case PacketID::PACKET_LAPDATA:
			analyzeLapData(packet.lapData);
			break;

		case PacketID::PACKET_MOTION:
			analyzeMotion(packet.motion);
			break;

		case PacketID::PACKET_PARTICIPANTS:
			analyzeParticipants(packet.participants);
			break;

		case PacketID::PACKET_SESSION:
			analyzeSession(packet.session);
			break;
	}
}

void Server::analyzeHeader(PacketHeader& header) {
	if (sessionUid != header.sessionUID) {
		emit this->headerUpdate(header);
	}
}

void Server::analyzeCarSetups(CarSetupPacket& packet) {
	emit this->carSetupsUpdate(packet);
}

void Server::analyzeCarStatus(CarStatusPacket& packet) {
	emit this->carStatusUpdate(packet);
}

void Server::analyzeCarTelemetry(CarTelemetryPacket& packet) {
	emit this->carTelemetryUpdate(packet);
}

void Server::analyzeEvent(EventPacket& packet) {
	emit this->eventUpdate(packet);
}

void Server::analyzeLapData(LapDataPacket& packet) {
	emit this->lapDataUpdate(packet);
}

void Server::analyzeMotion(MotionPacket& packet) {
	emit this->motionUpdate(packet);
}

void Server::analyzeParticipants(ParticipantsPacket& packet) {
	emit this->participantsUpdate(packet);
}

void Server::analyzeSession(SessionPacket& packet) {
	emit this->sessionUpdate(packet);
}
