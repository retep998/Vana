/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "Mist.h"
#include "Maps.h"
#include "Mob.h"
#include "Player.h"
#include "SkillDataProvider.h"

Mist::Mist(int32_t mapId, Player *owner, const Pos &origin, SkillLevelInfo *skill, int32_t skillId, uint8_t level, bool isPoison) :
	m_ownerMap(mapId),
	m_ownerId(owner->getId()),
	m_skill(skillId),
	m_level(level),
	m_origin(origin),
	m_time((int16_t)(skill->time)),
	m_delay(8),
	m_isMobMist(false),
	m_poison(isPoison)
{
	if (owner->isFacingRight()) {
		m_rb = Pos(skill->lt.x * -1 + origin.x, skill->rb.y + origin.y);
		m_lt = Pos(skill->rb.x * -1 + origin.x, skill->lt.y + origin.y);
	}
	else {
		m_lt = Pos(skill->lt.x + origin.x, skill->lt.y + origin.y);
		m_rb = Pos(skill->rb.x + origin.x, skill->rb.y + origin.y);
	}
	m_skillLt = skill->lt;
	m_skillRb = skill->rb;

	Maps::getMap(mapId)->addMist(this);
}

Mist::Mist(int32_t mapId, Mob *owner, const Pos &origin, MobSkillLevelInfo *skill, uint8_t skillId, uint8_t level) :
	m_ownerMap(mapId),
	m_ownerId(owner->getId()),
	m_skill(skillId),
	m_level(level),
	m_origin(origin),
	m_time(skill->time),
	m_delay(0),
	m_isMobMist(true),
	m_poison(true)
{
	if (owner->isFacingRight()) {
		m_rb = Pos(skill->lt.x * -1 + origin.x, skill->rb.y + origin.y);
		m_lt = Pos(skill->rb.x * -1 + origin.x, skill->lt.y + origin.y);
	}
	else {
		m_lt = Pos(skill->lt.x + origin.x, skill->lt.y + origin.y);
		m_rb = Pos(skill->rb.x + origin.x, skill->rb.y + origin.y);
	}
	m_skillLt = skill->lt;
	m_skillRb = skill->rb;

	Maps::getMap(mapId)->addMist(this);
}

Map * Mist::getMap() const {
	return Maps::getMap(m_ownerMap);
}