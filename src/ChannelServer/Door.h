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
#pragma once

#include "Map.h"
#include "MapObjects.h"
#include "Pos.h"
#include "Types.h"

class Map;
class Player;

class Door {
public:
	Door(Player *opener, Pos position, int32_t time);

	void closeDoor(bool displace = false);
	void warp(Player *player, bool totown);

	Pos getPos() const { return mappos; }
	int32_t getId() const { return doorid; }
	int32_t getSourceId() const { return mapid; }
	int32_t getTownId() const { return townid; }
	Map * getSource() const;
	Map * getTown() const;
	PortalInfo * getPortal();
	Player * getOwner() const { return owner; }
private:
	int32_t townid;
	int32_t mapid;
	int32_t doorid;
	int32_t doorindex;
	Pos mappos;
	Player *owner;
	PortalInfo townportal;
};
