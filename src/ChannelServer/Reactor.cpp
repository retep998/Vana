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
#include "Reactor.hpp"
#include "ChannelServer/DropHandler.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/ReactorPacket.hpp"

namespace Vana {
namespace ChannelServer {

Reactor::Reactor(map_id_t mapId, reactor_id_t reactorId, const Point &pos, bool facesLeft) :
	m_reactorId{reactorId},
	m_mapId{mapId},
	m_pos{pos},
	m_facesLeft{facesLeft}
{
	Maps::getMap(mapId)->addReactor(this);
}

auto Reactor::setState(int8_t state, bool sendPacket) -> void {
	m_state = state;
	if (sendPacket) {
		getMap()->send(Packets::triggerReactor(this));
	}
}

auto Reactor::restore() -> void {
	revive();
	setState(0, false);
	getMap()->send(Packets::spawnReactor(this));
}

auto Reactor::drop(ref_ptr_t<Player> player) -> void {
	DropHandler::doDrops(player->getId(), m_mapId, 0, m_reactorId, m_pos, false, false);
}

auto Reactor::getMap() const -> Map * {
	return Maps::getMap(m_mapId);
}

}
}