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
#include "Mist.h"
#include "Maps.h"
#include "Mob.h"
#include "Player.h"

Mist::Mist(int32_t mapid, Player *owner, Pos origin, SkillLevelInfo skill, int32_t skillid, uint8_t level) :
m_ownermap(mapid),
m_ownerid(owner->getId()),
m_skill(skillid),
m_level(level),
m_origin(origin),
m_time((int16_t)(skill.time)),
m_delay(8),
m_ismobmist(false)
{
	switch (skillid) {
		case Jobs::FPMage::PoisonMist:
			m_poison = true;
			break;
		default:
			m_poison = false;
	}
	Pos lt = skill.lt;
	Pos rb = skill.rb;
	if (owner->isFacingRight()) {
		m_rb = Pos(lt.x * -1 + origin.x, rb.y + origin.y);
		m_lt = Pos(rb.x * -1 + origin.x, lt.y + origin.y);
	}
	else {
		m_lt = Pos(lt.x + origin.x, lt.y + origin.y);
		m_rb = Pos(rb.x + origin.x, rb.y + origin.y);
	}
	m_skilllt = lt;
	m_skillrb = rb;

	m_id = Maps::getMistId();
	Maps::getMap(mapid)->addMist(this);
}

Mist::Mist(int32_t mapid, Mob *owner, Pos origin, MobSkillLevelInfo skill, uint8_t skillid, uint8_t level) :
m_ownermap(mapid),
m_ownerid(owner->getId()),
m_skill(skillid),
m_level(level),
m_origin(origin),
m_time(skill.time),
m_delay(0),
m_ismobmist(true),
m_poison(true)
{
	Pos lt = skill.lt;
	Pos rb = skill.rb;
	if (owner->isFacingRight()) {
		m_rb = Pos(lt.x * -1 + origin.x, rb.y + origin.y);
		m_lt = Pos(rb.x * -1 + origin.x, lt.y + origin.y);
	}
	else {
		m_lt = Pos(lt.x + origin.x, lt.y + origin.y);
		m_rb = Pos(rb.x + origin.x, rb.y + origin.y);
	}
	m_skilllt = lt;
	m_skillrb = rb;

	m_id = Maps::getMistId();
	Maps::getMap(mapid)->addMist(this);
}

Map * Mist::getMap() const {
	return Maps::getMap(m_ownermap);
}