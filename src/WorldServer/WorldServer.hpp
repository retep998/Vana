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

#include "AbstractServer.hpp"
#include "Channels.hpp"
#include "ConfigFile.hpp"
#include "Configuration.hpp"
#include "Ip.hpp"
#include "LoginServerConnection.hpp"
#include "PlayerDataProvider.hpp"
#include "Types.hpp"
#include "WorldServerAcceptConnection.hpp"
#include <string>

class PacketBuilder;

class WorldServer final : public AbstractServer {
	SINGLETON(WorldServer);
public:
	auto shutdown() -> void override;
	auto establishedLoginConnection(world_id_t worldId, port_t port, const WorldConfig &conf) -> void;
	auto rehashConfig(const WorldConfig &config) -> void;
	auto setScrollingHeader(const string_t &message) -> void;
	auto setRates(const Rates &rates) -> void;
	auto resetRates() -> void;
	auto getPlayerDataProvider() -> PlayerDataProvider &;
	auto getChannels() -> Channels &;
	auto isConnected() const -> bool;
	auto getWorldId() const -> world_id_t;
	auto makeChannelPort(channel_id_t channelId) const -> port_t;
	auto getConfig() -> const WorldConfig &;
	auto sendLogin(const PacketBuilder &builder) -> void;
protected:
	auto listen() -> void;
	auto loadData() -> Result override;
	auto makeLogIdentifier() const -> opt_string_t override;
	auto getLogPrefix() const -> string_t override;
private:
	world_id_t m_worldId = -1;
	port_t m_port = 0;
	WorldConfig m_config;
	Rates m_defaultRates;
	LoginServerConnection *m_loginConnection;
	PlayerDataProvider m_playerDataProvider;
	Channels m_channels;
};