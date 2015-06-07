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
#include "StatusInfo.hpp"
#include "Randomizer.hpp"
#include "SkillConstants.hpp"

StatusInfo::StatusInfo(int32_t status, int32_t val, int32_t skillId, int32_t timeInSeconds) :
	status(status),
	val(val),
	skillId(skillId),
	time(seconds_t(timeInSeconds)),
	reflection(0)
{
	switch (val) {
		case StatusEffects::Mob::Freeze:
			if (skillId == Skills::FpArchMage::Paralyze) {
				break;
			}
		case StatusEffects::Mob::Stun:
			time = seconds_t(timeInSeconds + (skillId == Skills::IlArchMage::Blizzard ? 3 : 1) + Randomizer::rand<int32_t>(timeInSeconds * 2));
			// The 1 accounts for the skill cast time
			// Ideally we'd like to remove both these additions with MCDB suport for cast times
			break;
	}
}

StatusInfo::StatusInfo(int32_t status, int32_t val, int16_t mobSkill, int16_t level, int32_t timeInSeconds) :
	status(status),
	val(val),
	skillId(-1),
	mobSkill(mobSkill),
	level(level),
	time(seconds_t(timeInSeconds))
{
}

StatusInfo::StatusInfo(int32_t status, int32_t val, int16_t mobSkill, int16_t level, int32_t reflect, int32_t timeInSeconds) :
	status(status),
	val(val),
	skillId(-1),
	mobSkill(mobSkill),
	level(level),
	time(seconds_t(timeInSeconds)),
	reflection(reflect)
{
}