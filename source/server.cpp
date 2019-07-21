#include "../headers/server.hpp"

#include "../headers/config.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <iostream>

Server::Server(QObject *parent) : QObject(parent) {
	socket = std::make_shared<QUdpSocket>(this);

    socket->bind(QHostAddress::LocalHost, Config::instance().getPort());

    auto analyzer = [this]() {
        while (running) {
            std::lock_guard<std::mutex> guard(receivedPacketsMutex);
            if (receivedPackets.size() > 0) {
                analyzePacket(receivedPackets.front());
                receivedPackets.pop();
            }
        }
    };

	auto receiver = [this]() {
		while (running) {
			Packet packet;
			long long packetSize;

			QHostAddress sender;
			quint16 senderPort;

			packetSize = socket->readDatagram(packet.rawData, MaxSize, &sender, &senderPort);

			if (packetSize > 0) {
				std::lock_guard<std::mutex> guard(receivedPacketsMutex);
				receivedPackets.push(packet);
			}
		}
	};

	receiveThread = std::thread(receiver);
    analyzeThread = std::thread(analyzer);
}

void Server::stop() {
	running = false;
	receiveThread.join();
    analyzeThread.join();
}

void Server::analyzePacket(Packet packet) {
	// TODO: read header


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

void Server::analyzeCarSetups(CarSetupPacket& packet) {

}

void Server::analyzeCarStatus(CarStatusPacket& packet) {

}

void Server::analyzeCarTelemetry(CarTelemetryPacket& packet) {

}

void Server::analyzeEvent(EventPacket& packet) {

}

void Server::analyzeLapData(LapDataPacket& packet) {

}

void Server::analyzeMotion(MotionPacket& packet) {

}

void Server::analyzeParticipants(ParticipantsPacket& packet) {

}

void Server::analyzeSession(SessionPacket& packet) {

}
