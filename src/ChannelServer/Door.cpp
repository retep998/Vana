/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Door.h"
#include "MapConstants.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::pair;
using std::tr1::bind;

Door::Door(Player *opener, Pos position, int32_t time) : 
owner(opener), 
mappos(position), 
doorindex(-1) 
{
	mapid = opener->getMap();
	townid = getSource()->getReturnMap();
	if (townid == Maps::NoMap) {
		// Hacking, I guess
		delete this;
	}
	else {
		pair<int32_t, PortalInfo> portalpair = getTown()->getOpenDoorIndexAndPoint();
		doorindex = portalpair.first;
		if (doorindex != -1) {
			townportal = portalpair.second;
			doorid = opener->getId();
			opener->setDoor(this);

			new Timer::Timer(bind(&Door::closeDoor, this, false),
				Timer::Id(Timer::Types::DoorTimer, doorid, 0),
				opener->getTimers(), Timer::Time::fromNow(time * 1000));

			getSource()->addDoor(this);
			getTown()->addDoor(this);
			MapPacket::spawnDoor(this);
			MapPacket::spawnPortal(mapid, townid, getPos(), getPortal()->pos);
		}
		else {
			// Fail packet - no open Door spots
			delete this;
		}
	}
}

void Door::closeDoor(bool displace) {
	Timer::Id id(Timer::Types::DoorTimer, doorid, 0);
	if (owner->getTimers()->checkTimer(id)) {
		owner->getTimers()->removeTimer(id);
	}

	owner->setDoor(nullptr);
	getSource()->removeDoor(-1, this);
	getTown()->removeDoor(doorindex, this);
	MapPacket::removeDoor(this, displace);
	if (!displace) {
		MapPacket::removePortal(mapid, townid);
	}
	delete this;
}

void Door::warp(Player *player, bool totown) {
	int32_t destination = (totown ? townid : mapid);
	int8_t pid = (totown ? 80 : 0);
	Pos spawnpos = (totown ? getPortal()->pos : mappos);

	player->setMap(destination, pid, spawnpos);
}

Map * Door::getSource() const {
	return Maps::getMap(mapid);
}

Map * Door::getTown() const {
	return Maps::getMap(townid);
}

PortalInfo * Door::getPortal() {
	return &townportal;
}
