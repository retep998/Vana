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
#pragma once

#include "Point.hpp"
#include "Types.hpp"

namespace Vana {
	class Map;
	class Player;

	class MysticDoor {
		NONCOPYABLE(MysticDoor);
	public:
		MysticDoor(Player *owner, map_id_t townId, portal_id_t portalId, const Point &mapPos, const Point &townPos, bool isDisplacement, seconds_t doorTime);
		MysticDoor(player_id_t ownerId, map_id_t mapId, const Point &mapPos, map_id_t townId, portal_id_t portalId, const Point &townPos, seconds_t doorTime);

		static const portal_id_t PortalId = 80;

		auto getOwnerId() const -> player_id_t;
		auto getTownId() const -> map_id_t;
		auto getMapId() const -> map_id_t;
		auto getTown() const -> Map *;
		auto getMap() const -> Map *;
		auto getTownPos() const -> Point;
		auto getMapPos() const -> Point;
		auto getPortalId() const -> portal_id_t;
		auto getDoorTime() const -> seconds_t;
		auto withNewPortal(portal_id_t portalId, const Point &townPos) const -> ref_ptr_t<MysticDoor>;
	private:

		map_id_t m_townId = 0;
		map_id_t m_mapId = 0;
		portal_id_t m_portalId = 0;
		player_id_t m_ownerId = 0;
		seconds_t m_doorTime = seconds_t{0};
		Point m_mapPos;
		Point m_townPos;
	};
}