/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "Reactor.h"
#include "DropHandler.h"
#include "Maps.h"
#include "Player.h"
#include "ReactorPacket.h"

Reactor::Reactor(int32_t mapId, int32_t reactorId, const Pos &pos) :
m_state(0),
m_reactorId(reactorId),
m_mapId(mapId),
m_alive(true),
m_pos(pos)
{
	Maps::getMap(mapId)->addReactor(this);
}

void Reactor::setState(int8_t state, bool sendPacket) {
	m_state = state;
	if (sendPacket) {
		ReactorPacket::triggerReactor(this);
	}
}

void Reactor::restore() {
	revive();
	setState(0, false);
	ReactorPacket::spawnReactor(this);
}

void Reactor::drop(Player *player) {
	DropHandler::doDrops(player->getId(), getMapId(), 0, getReactorId(), getPos(), false, false);
}