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
#include "Mist.hpp"
#include "Maps.hpp"
#include "Mob.hpp"
#include "Player.hpp"
#include "SkillDataProvider.hpp"

namespace Vana {

Mist::Mist(map_id_t mapId, Player *owner, seconds_t time, const Rect &area, skill_id_t skillId, skill_level_t level, bool isPoison) :
	m_ownerMap{mapId},
	m_ownerId{owner->getId()},
	m_skill{skillId},
	m_level{level},
	m_area{area},
	m_time{time},
	m_delay{8},
	m_isMobMist{false},
	m_poison{isPoison}
{
	Maps::getMap(mapId)->addMist(this);
}

Mist::Mist(map_id_t mapId, Mob *owner, seconds_t time, const Rect &area, mob_skill_id_t skillId, mob_skill_level_t level) :
	m_ownerMap{mapId},
	m_ownerId{owner->getMapMobId()},
	m_skill{skillId},
	m_level{level},
	m_area{area},
	m_time{time}
{
	Maps::getMap(mapId)->addMist(this);
}

auto Mist::getMap() const -> Map * {
	return Maps::getMap(m_ownerMap);
}

}