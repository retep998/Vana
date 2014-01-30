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
#include "Configuration.hpp"
#include "Ip.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

class PacketCreator;
class WorldServerConnection;

class ChannelServer final : public AbstractServer {
	SINGLETON_CUSTOM_CONSTRUCTOR(ChannelServer);
public:
	auto shutdown() -> void override;
	auto connectToWorld(world_id_t worldId, port_t port, const Ip &ip) -> void;
	auto establishedWorldConnection(channel_id_t channelId, port_t port, const WorldConfig &config) -> void;

	// TODO FIXME
	// Eyeball these for potential refactoring - they involve world<->channel operations and I don't want to dig into that now
	auto setScrollingHeader(const string_t &message) -> void;
	auto modifyRate(int32_t rateType, int32_t newValue) -> void;
	auto setConfig(const WorldConfig &config) -> void;
	auto setRates(const Rates &rates) -> void;

	auto isConnected() const -> bool;
	auto getWorldId() const -> world_id_t;
	auto getChannelId() const -> channel_id_t;
	auto getOnlineId() const -> int32_t;
	auto getConfig() const -> const WorldConfig &;
	auto sendPacketToWorld(PacketCreator &packet) -> void;
protected:
	auto loadData() -> void override;
	auto listen() -> void override;
	auto makeLogIdentifier() const -> opt_string_t override;
	auto getLogPrefix() const -> string_t override;
private:
	world_id_t m_worldId = -1;
	channel_id_t m_channelId = -1;
	port_t m_worldPort = 0;
	port_t m_port = 0;
	Ip m_worldIp;
	WorldConfig m_config;
	WorldServerConnection *m_worldConnection = nullptr;
};