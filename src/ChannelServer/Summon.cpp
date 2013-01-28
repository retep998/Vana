/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Summon.h"
#include "SkillConstants.h"
#include "SkillDataProvider.h"

Summon::Summon(int32_t id, int32_t summonId, uint8_t level) :
	m_id(id),
	m_summonId(summonId),
	m_level(level),
	m_hp(0)
{
	switch (summonId) {
		case Skills::Ranger::Puppet:
		case Skills::Sniper::Puppet:
		case Skills::WindArcher::Puppet:
			m_hp = SkillDataProvider::Instance()->getSkill(summonId, level)->x; // Get HP for puppet
		case Skills::Outlaw::Octopus:
		case Skills::Corsair::WrathOfTheOctopi:
			m_type = Static;
			break;
		case Skills::Priest::SummonDragon:
		case Skills::Ranger::SilverHawk:
		case Skills::Sniper::GoldenEagle:
		case Skills::Bowmaster::Phoenix:
		case Skills::Marksman::Frostprey:
		case Skills::Outlaw::Gaviota:
			m_type = Flying;
			break;
		default:
			m_type = Follow;
			break;
	}
}