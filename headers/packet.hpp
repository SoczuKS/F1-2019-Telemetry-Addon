#pragma once

#include "carSetupsPacket.hpp"
#include "carStatusPacket.hpp"
#include "carTelemetryPacket.hpp"
#include "eventPacket.hpp"
#include "lapDataPacket.hpp"
#include "motionPacket.hpp"
#include "participantsPacket.hpp"
#include "sessionPacket.hpp"

#include <vector>

constexpr uint16 MaxSize = CarTelemetryPacketSize;

union Packet {
    char rawData[MaxSize];

	PacketHeader header;

	CarSetupPacket carSetup;
	CarStatusPacket carStatus;
	CarTelemetryPacket carTelemetry;
	EventPacket event;
	LapDataPacket lapData;
	MotionPacket motion;
	ParticipantsPacket participants;
	SessionPacket session;
};
