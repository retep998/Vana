/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Common/PacketBuilder.hpp"
#include "WorldServer/WorldServerAcceptedSession.hpp"

namespace Vana {
namespace WorldServer {

Channel::Channel(ref_ptr_t<WorldServerAcceptedSession> session, channel_id_t id, port_t port) :
	m_session{session},
	m_id{id},
	m_port{port}
{
}

auto Channel::send(const PacketBuilder &builder) -> void {
	m_session->send(builder);
}

auto Channel::increasePlayers() -> int32_t {
	return ++m_players;
}

auto Channel::decreasePlayers() -> int32_t {
	return --m_players;
}

auto Channel::getId() const -> channel_id_t {
	return m_id;
}

auto Channel::getPort() const -> port_t {
	return m_port;
}

auto Channel::disconnect() -> void {
	m_session->disconnect();
}

}
}