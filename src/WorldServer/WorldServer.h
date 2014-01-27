/*
Copyright (C) 2008-2014 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

#include "AbstractServer.h"
#include "ConfigFile.h"
#include "Configuration.h"
#include "Ip.h"
#include "LoginServerConnection.h"
#include "Types.h"
#include "WorldServerAcceptConnection.h"
#include <string>

class PacketCreator;

class WorldServer : public AbstractServer {
	SINGLETON_CUSTOM_CONSTRUCTOR(WorldServer);
public:
	auto loadData() -> void override;
	auto loadConfig() -> void override;
	auto loadLogConfig() -> void override;
	auto listen() -> void override;
	auto makeLogIdentifier() -> opt_string_t override;

	auto rehashConfig(const WorldConfig &config) -> void;

	auto setWorldId(int8_t id) -> void { m_worldId = id; }
	auto setInterPort(port_t port) -> void { m_port = port; }
	auto setScrollingHeader(const string_t &message) -> void;
	auto setConfig(const WorldConfig &config) -> void;
	auto setRates(const Rates &rates) -> void;
	auto resetRates() -> void;
	auto sendPacketToLogin(const PacketCreator &packet) -> void;

	auto isConnected() const -> bool { return m_worldId != -1; }
	auto getWorldId() const -> int8_t { return m_worldId; }
	auto getInterPort() const -> port_t { return m_port; }
	auto getMaxChannels() const -> int32_t { return m_config.maxChannels; }
	auto getScrollingHeader() -> string_t { return m_config.scrollingHeader.c_str(); }
	auto getConfig() -> WorldConfig & { return m_config; }
private:
	int8_t m_worldId = -1;
	port_t m_loginPort = 0;
	port_t m_port = 0;
	Ip m_loginIp;
	WorldConfig m_config;
	Rates m_defaultRates;
	LoginServerConnection *m_loginConnection = nullptr;
};