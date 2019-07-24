#pragma once

#include <string>
#include <array>
#include <map>

#include <Windows.h>

#include "types.hpp"

#include <QVector2D>

constexpr uint8 propertiesNum = 6;

enum class Format { FORMAT_2019, FORMAT_2018, FORMAT_OLD };	// Currently using only the latest format

enum class ShortcutId { CLOSE = 1, HIDE_SHOW = 2 };

struct Shortcut {
	Shortcut() = default;
	Shortcut(unsigned int m, unsigned int k) : modifiers(m), key(k) {}

	unsigned int modifiers;
	unsigned int key;
};

class Config {
public:
	static Config& instance() {
		static Config cfg;
		return cfg;
	}

	~Config() { save(); }

	void save();
	void reset();

	Format getFormat() const { return format; }
	void setFormat(const Format f) { format = f; }

	uint16 getPort() const { return port; }
	void setPort(uint16 p) { port = p; }

	std::map<ShortcutId, Shortcut> getShortcuts() { return shortcuts; }
	void setShortcut(ShortcutId sid, Shortcut s) { shortcuts[sid] = s; }

	std::string getServerAddress() const { return serverAddress; }
	void setServerAddress(const std::string& addr) { serverAddress = addr; }

	QVector2D getCarStatusWidgetPosition() const { return carStatusWidgetPosition; }
	void setCarStatusWidgetPosition(const QVector2D& pos) { carStatusWidgetPosition = pos; }

	QVector2D getTimeStatusWidgetPosition() const { return timeStatusWidgetPosition; }
	void setTimeStatusWidgetPosition(const QVector2D& pos) { timeStatusWidgetPosition = pos; }

	QVector2D getSessionWidgetPosition() const { return sessionStatusWidgetPosition; }
	void setSessionWidgetPosition(const QVector2D& pos) { sessionStatusWidgetPosition = pos; }


private:
	Config() { load(); }
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	Format format;
	uint16 port;
	std::string serverAddress;

	QVector2D carStatusWidgetPosition;
	QVector2D timeStatusWidgetPosition;
	QVector2D sessionStatusWidgetPosition;

	const std::string configFileName = "config.cfg";

	std::map<ShortcutId, Shortcut> shortcuts = { 
		{ ShortcutId::CLOSE, { NULL, VK_PAUSE } },
		{ ShortcutId::HIDE_SHOW, { MOD_CONTROL | MOD_ALT, VK_NUMPAD0 } }
	};

	void load();
};
