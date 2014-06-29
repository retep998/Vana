/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Summon.hpp"
#include "ChannelServer.hpp"
#include "SkillConstants.hpp"
#include "SkillDataProvider.hpp"

Summon::Summon(summon_id_t id, skill_id_t summonId, skill_level_t level, bool isFacingLeft, const Point &position, foothold_id_t foothold) :
	MovableLife(foothold, position, 4),
	m_id(id),
	m_summonId(summonId),
	m_level(level)
{
	m_actionType = Attack;
	switch (summonId) {
		case Skills::Ranger::Puppet:
		case Skills::Sniper::Puppet:
		case Skills::WindArcher::Puppet:
			m_actionType = DoNothing;
			m_hp = ChannelServer::getInstance().getSkillDataProvider().getSkill(summonId, level)->x;
			// Intentional fallthrough
		case Skills::Outlaw::Octopus:
		case Skills::Corsair::WrathOfTheOctopi:
			m_movementType = Static;
			break;
		case Skills::Priest::SummonDragon:
		case Skills::Ranger::SilverHawk:
		case Skills::Sniper::GoldenEagle:
		case Skills::Bowmaster::Phoenix:
		case Skills::Marksman::Frostprey:
			m_movementType = FlyClose;
			break;
		case Skills::Outlaw::Gaviota:
			m_movementType = FlyFar;
			break;
		case Skills::DarkKnight::Beholder:
			m_actionType = Beholder;
			// Intentional fallthrough
		case Skills::Bishop::Bahamut:
		case Skills::FpArchMage::Elquines:
		case Skills::IlArchMage::Ifrit:
		case Skills::DawnWarrior::Soul:
		case Skills::BlazeWizard::Flame:
		case Skills::BlazeWizard::Ifrit:
		case Skills::WindArcher::Storm:
		case Skills::NightWalker::Darkness:
		case Skills::ThunderBreaker::Lightning:
			m_movementType = Follow;
			break;
		default:
			std::cerr << "Summon not accounted for in the types: " << summonId << std::endl;
			break;
	}
}