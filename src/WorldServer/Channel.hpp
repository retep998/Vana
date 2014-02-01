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

#include "Ip.hpp"
#include "ExternalIp.hpp"
#include "ExternalIpResolver.hpp"
#include "Types.hpp"
#include <vector>

class PacketBuilder;
class WorldServerAcceptConnection;

class Channel : public ExternalIpResolver {
	NONCOPYABLE(Channel);
public:
	Channel() = default;
	auto setId(channel_id_t id) -> void { m_id = id; }
	auto setPort(port_t port) -> void { m_port = port; }
	auto setConnection(WorldServerAcceptConnection *connection) -> void { m_connection = connection; }
	auto setPlayers(int32_t players) -> void { m_players = players; }

	auto increasePlayers() -> int32_t { return ++m_players; }
	auto decreasePlayers() -> int32_t { return --m_players; }
	auto getId() const -> channel_id_t { return m_id; }
	auto getPort() const -> port_t { return m_port; }
	auto send(const PacketBuilder &builder) -> void;
private:
	channel_id_t m_id = 0;
	port_t m_port = 0;
	int32_t m_players = 0;
	WorldServerAcceptConnection *m_connection = nullptr;
};