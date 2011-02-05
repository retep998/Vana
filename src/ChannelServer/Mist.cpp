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
#include "Mist.h"
#include "Maps.h"
#include "Mob.h"
#include "Player.h"
#include "SkillDataProvider.h"

Mist::Mist(int32_t mapid, Player *owner, const Pos &origin, SkillLevelInfo *skill, int32_t skillid, uint8_t level, bool ispoison) :
m_ownermap(mapid),
m_ownerid(owner->getId()),
m_skill(skillid),
m_level(level),
m_origin(origin),
m_time((int16_t)(skill->time)),
m_delay(8),
m_ismobmist(false),
m_poison(ispoison)
{
	if (owner->isFacingRight()) {
		m_rb = Pos(skill->lt.x * -1 + origin.x, skill->rb.y + origin.y);
		m_lt = Pos(skill->rb.x * -1 + origin.x, skill->lt.y + origin.y);
	}
	else {
		m_lt = Pos(skill->lt.x + origin.x, skill->lt.y + origin.y);
		m_rb = Pos(skill->rb.x + origin.x, skill->rb.y + origin.y);
	}
	m_skilllt = skill->lt;
	m_skillrb = skill->rb;

	Maps::getMap(mapid)->addMist(this);
}

Mist::Mist(int32_t mapid, Mob *owner, const Pos &origin, MobSkillLevelInfo *skill, uint8_t skillid, uint8_t level) :
m_ownermap(mapid),
m_ownerid(owner->getId()),
m_skill(skillid),
m_level(level),
m_origin(origin),
m_time(skill->time),
m_delay(0),
m_ismobmist(true),
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
	m_skilllt = skill->lt;
	m_skillrb = skill->rb;

	Maps::getMap(mapid)->addMist(this);
}

Map * Mist::getMap() const {
	return Maps::getMap(m_ownermap);
}