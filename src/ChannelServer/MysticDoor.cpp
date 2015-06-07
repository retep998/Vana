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
#include "MysticDoor.hpp"
#include "Maps.hpp"
#include "Mob.hpp"
#include "Player.hpp"
#include "SkillDataProvider.hpp"

MysticDoor::MysticDoor(Player *owner, map_id_t townId, portal_id_t portalId, const Point &mapPos, const Point &townPos, bool isDisplacement, seconds_t doorTime) :
	m_mapId{owner->getMapId()},
	m_mapPos{mapPos},
	m_townId{townId},
	m_townPos{townPos},
	m_ownerId{owner->getId()},
	m_portalId{portalId},
	m_doorTime{doorTime}
{
}

MysticDoor::MysticDoor(player_id_t ownerId, map_id_t mapId, const Point &mapPos, map_id_t townId, portal_id_t portalId, const Point &townPos, seconds_t doorTime) :
	m_mapId{mapId},
	m_mapPos{mapPos},
	m_townId{townId},
	m_townPos{townPos},
	m_ownerId{ownerId},
	m_portalId{portalId},
	m_doorTime{doorTime}
{
}

auto MysticDoor::getOwnerId() const -> player_id_t {
	return m_ownerId;
}

auto MysticDoor::getPortalId() const -> portal_id_t {
	return m_portalId;
}

auto MysticDoor::getTownId() const -> map_id_t {
	return m_townId;
}

auto MysticDoor::getMapId() const -> map_id_t {
	return m_mapId;
}

auto MysticDoor::getTown() const -> Map * {
	return Maps::getMap(m_townId);
}

auto MysticDoor::getMap() const -> Map * {
	return Maps::getMap(m_mapId);
}

auto MysticDoor::getTownPos() const -> Point {
	return m_townPos;
}

auto MysticDoor::getMapPos() const -> Point {
	return m_mapPos;
}

auto MysticDoor::getDoorTime() const -> seconds_t {
	return m_doorTime;
}

auto MysticDoor::withNewPortal(portal_id_t portalId, const Point &townPos) const -> ref_ptr_t<MysticDoor> {
	return make_ref_ptr<MysticDoor>(
		m_ownerId,
		m_mapId,
		m_mapPos,
		m_townId,
		portalId,
		townPos,
		m_doorTime);
}