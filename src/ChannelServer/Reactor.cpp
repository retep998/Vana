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

Reactor::Reactor(int32_t mapid, int32_t reactorId, const Pos &pos) :
	state(0),
	reactorId(reactorId),
	mapid(mapid),
	alive(true),
	pos(pos)
{
	Maps::getMap(mapid)->addReactor(this);
}

void Reactor::setState(int8_t state, bool is) {
	this->state = state;
	if (is) {
		ReactorPacket::triggerReactor(this);
	}
}

void Reactor::restore() {
	revive();
	setState(0, false);
	ReactorPacket::spawnReactor(this);
}

void Reactor::drop(Player *player) {
	DropHandler::doDrops(player->getId(), mapid, 0, reactorId, pos, false, false);
}