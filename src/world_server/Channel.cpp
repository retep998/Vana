/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "Channel.hpp"
#include "common_temp/PacketBuilder.hpp"
#include "world_server/WorldServerAcceptedSession.hpp"

namespace vana {
namespace world_server {

channel::channel(ref_ptr<world_server_accepted_session> session, game_channel_id id, connection_port port) :
	m_session{session},
	m_id{id},
	m_port{port}
{
}

auto channel::send(const packet_builder &builder) -> void {
	m_session->send(builder);
}

auto channel::increase_players() -> int32_t {
	return ++m_players;
}

auto channel::decrease_players() -> int32_t {
	return --m_players;
}

auto channel::get_id() const -> game_channel_id {
	return m_id;
}

auto channel::get_port() const -> connection_port {
	return m_port;
}

auto channel::disconnect() -> void {
	m_session->disconnect();
}

}
}