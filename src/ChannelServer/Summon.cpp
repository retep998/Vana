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
		case Jobs::Ranger::Puppet:
		case Jobs::Sniper::Puppet:
		case Jobs::WindArcher::Puppet:
			m_hp = SkillDataProvider::Instance()->getSkill(summonId, level)->x; // Get HP for puppet
		case Jobs::Outlaw::Octopus:
		case Jobs::Corsair::WrathOfTheOctopi:
			m_type = Static;
			break;
		case Jobs::Priest::SummonDragon:
		case Jobs::Ranger::SilverHawk:
		case Jobs::Sniper::GoldenEagle:
		case Jobs::Bowmaster::Phoenix:
		case Jobs::Marksman::Frostprey:
		case Jobs::Outlaw::Gaviota:
			m_type = Flying;
			break;
		default:
			m_type = Follow;
			break;
	}
}