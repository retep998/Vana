/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "Common/SkillConstants.hpp"
#include "Common/SkillDataProvider.hpp"
#include "ChannelServer/ChannelServer.hpp"

namespace Vana {
namespace ChannelServer {

Summon::Summon(summon_id_t id, skill_id_t summonId, skill_level_t level, bool isFacingLeft, const Point &position, foothold_id_t foothold) :
	MovableLife{foothold, position, 4},
	m_id{id},
	m_summonId{summonId},
	m_level{level}
{
	m_actionType = Attack;
	switch (summonId) {
		case Vana::Skills::Ranger::Puppet:
		case Vana::Skills::Sniper::Puppet:
		case Vana::Skills::WindArcher::Puppet:
			m_actionType = DoNothing;
			m_hp = ChannelServer::getInstance().getSkillDataProvider().getSkill(summonId, level)->x;
			// Intentional fallthrough
		case Vana::Skills::Outlaw::Octopus:
		case Vana::Skills::Corsair::WrathOfTheOctopi:
			m_movementType = Static;
			break;
		case Vana::Skills::Priest::SummonDragon:
		case Vana::Skills::Ranger::SilverHawk:
		case Vana::Skills::Sniper::GoldenEagle:
		case Vana::Skills::Bowmaster::Phoenix:
		case Vana::Skills::Marksman::Frostprey:
			m_movementType = FlyClose;
			break;
		case Vana::Skills::Outlaw::Gaviota:
			m_movementType = FlyFar;
			break;
		case Vana::Skills::DarkKnight::Beholder:
			m_actionType = Beholder;
			// Intentional fallthrough
		case Vana::Skills::Bishop::Bahamut:
		case Vana::Skills::FpArchMage::Elquines:
		case Vana::Skills::IlArchMage::Ifrit:
		case Vana::Skills::DawnWarrior::Soul:
		case Vana::Skills::BlazeWizard::Flame:
		case Vana::Skills::BlazeWizard::Ifrit:
		case Vana::Skills::WindArcher::Storm:
		case Vana::Skills::NightWalker::Darkness:
		case Vana::Skills::ThunderBreaker::Lightning:
			m_movementType = Follow;
			break;
		default:
			// Might be a processing problem or it might be a hacking problem
			ChannelServer::getInstance().log(LogType::MalformedPacket, [&](out_stream_t &str) {
				str << "Summon not accounted for in the types: " << summonId;
			});
			break;
	}
}

}
}