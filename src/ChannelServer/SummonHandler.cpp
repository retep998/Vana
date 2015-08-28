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
#include "SummonHandler.hpp"
#include "BuffDataProvider.hpp"
#include "BuffsPacket.hpp"
#include "ChannelServer.hpp"
#include "GameLogicUtilities.hpp"
#include "IdPool.hpp"
#include "Map.hpp"
#include "Maps.hpp"
#include "MovementHandler.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "SkillConstants.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "SkillConstants.hpp"
#include "SkillDataProvider.hpp"
#include "Summon.hpp"
#include "SummonsPacket.hpp"

namespace Vana {

IdPool<summon_id_t> SummonHandler::summonIds;

auto SummonHandler::useSummon(Player *player, skill_id_t skillId, skill_level_t level) -> void {
	// Determine if any summons need to be removed and do it
	switch (skillId) {
		case Skills::Ranger::Puppet:
		case Skills::Sniper::Puppet:
		case Skills::WindArcher::Puppet:
			player->getSummons()->forEach([player, skillId](Summon *summon) {
				if (summon->getSkillId() == skillId) {
					removeSummon(player, summon->getId(), false, SummonMessages::None);
				}
			});
			break;
		case Skills::Ranger::SilverHawk:
		case Skills::Bowmaster::Phoenix:
		case Skills::Sniper::GoldenEagle:
		case Skills::Marksman::Frostprey:
			player->getSummons()->forEach([player, skillId](Summon *summon) {
				if (!GameLogicUtilities::isPuppet(summon->getSkillId())) {
					removeSummon(player, summon->getId(), false, SummonMessages::None);
				}
			});
			break;
		case Skills::Outlaw::Gaviota:
		case Skills::Outlaw::Octopus:
		case Skills::Corsair::WrathOfTheOctopi: {
			int8_t maxCount = -1;
			int8_t currentCount = 0;
			switch (skillId) {
				case Skills::Outlaw::Octopus: maxCount = 2; break;
				case Skills::Corsair::WrathOfTheOctopi: maxCount = 3; break;
				case Skills::Outlaw::Gaviota: maxCount = 4; break;
			}

			player->getSummons()->forEach([player, skillId, &currentCount](Summon *summon) {
				if (summon->getSkillId() == skillId) {
					currentCount++;
				}
			});

			if (currentCount == maxCount) {
				// We have to remove one
				bool removed = false;
				player->getSummons()->forEach([player, skillId, &removed](Summon *summon) {
					if (summon->getSkillId() != skillId || removed) {
						return;
					}
					removeSummon(player, summon->getId(), false, SummonMessages::None);
					removed = true;
				});
			}
			break;
		}
		default:
			// By default, you can only have one summon out
			player->getSummons()->forEach([player, skillId](Summon *summon) {
				removeSummon(player, summon->getId(), false, SummonMessages::None);
			});
	}

	Point playerPosition = player->getPos();
	Point summonPosition;
	foothold_id_t foothold = player->getFoothold();
	bool puppet = GameLogicUtilities::isPuppet(skillId);
	if (puppet) {
		// TODO FIXME formula
		// TODO FIXME skill
		// This is not kosher
		playerPosition.x += 200 * (player->isFacingRight() ? 1 : -1);
		player->getMap()->findFloor(playerPosition, summonPosition, -5);
		foothold = player->getMap()->getFootholdAtPosition(summonPosition);
	}
	else {
		summonPosition = playerPosition;
	}
	Summon *summon = new Summon(summonIds.lease(), skillId, level, player->isFacingLeft() && !puppet, summonPosition);
	if (summon->getMovementType() == Summon::Static) {
		summon->resetMovement(foothold, summon->getPos(), summon->getStance());
	}

	auto skill = ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level);
	player->getSummons()->addSummon(summon, skill->buffTime);
	player->sendMap(Packets::showSummon(player->getId(), summon, false));
}

auto SummonHandler::removeSummon(Player *player, summon_id_t summonId, bool packetOnly, int8_t showMessage, bool fromTimer) -> void {
	Summon *summon = player->getSummons()->getSummon(summonId);
	if (summon != nullptr) {
		player->sendMap(Packets::removeSummon(player->getId(), summon, showMessage));
		if (!packetOnly) {
			player->getSummons()->removeSummon(summonId, fromTimer);
		}
	}
}

auto SummonHandler::showSummon(Player *player) -> void {
	player->getSummons()->forEach([player](Summon *summon) {
		summon->setPos(player->getPos());
		player->sendMap(Packets::showSummon(player->getId(), summon));
	});
}

auto SummonHandler::showSummons(Player *fromPlayer, Player *toPlayer) -> void {
	fromPlayer->getSummons()->forEach([fromPlayer, toPlayer](Summon *summon) {
		toPlayer->send(Packets::showSummon(fromPlayer->getId(), summon));
	});
}

auto SummonHandler::moveSummon(Player *player, PacketReader &reader) -> void {
	summon_id_t summonId = reader.get<summon_id_t>();

	// I am not certain what this is, but in the Odin source they seemed to think it was original position. However, it caused AIDS.
	reader.unk<uint32_t>();

	Summon *summon = player->getSummons()->getSummon(summonId);
	if (summon == nullptr || summon->getMovementType() == Summon::Static) {
		// Up to no good, lag, or something else
		return;
	}

	MovementHandler::parseMovement(summon, reader);
	reader.reset(10);
	player->sendMap(Packets::moveSummon(player->getId(), summon, summon->getPos(), reader.getBuffer(), (reader.getBufferLength() - 9)));
}

auto SummonHandler::damageSummon(Player *player, PacketReader &reader) -> void {
	summon_id_t summonId = reader.get<summon_id_t>();
	int8_t unk = reader.get<int8_t>();
	damage_t damage = reader.get<damage_t>();
	map_object_t mobId = reader.get<map_object_t>();

	if (Summon *summon = player->getSummons()->getSummon(summonId)) {
		if (!GameLogicUtilities::isPuppet(summon->getSkillId())) {
			// Hacking
			return;
		}

		summon->doDamage(damage);
		if (summon->getHp() <= 0) {
			removeSummon(player, summonId, false, SummonMessages::None, true);
		}
	}
}

auto SummonHandler::makeBuff(Player *player, item_id_t itemId) -> BuffInfo {
	const auto &buffData = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	switch (itemId) {
		case Items::BeholderHexWatk: return buffData.physicalAttack;
		case Items::BeholderHexWdef: return buffData.physicalDefense;
		case Items::BeholderHexMdef: return buffData.magicDefense;
		case Items::BeholderHexAcc: return buffData.accuracy;
		case Items::BeholderHexAvo: return buffData.avoid;
	}
	// Hacking?
	throw std::invalid_argument{"invalid_argument"};
}

auto SummonHandler::makeActiveBuff(Player *player, const BuffInfo &data, item_id_t itemId, const SkillLevelInfo *skillInfo) -> BuffPacketValues {
	BuffPacketValues buff;
	buff.player.types[data.getBuffByte()] = static_cast<uint8_t>(data.getBuffType());
	switch (itemId) {
		case Items::BeholderHexWdef: buff.player.values.push_back(BuffPacketValue::fromValue(2, skillInfo->wDef)); break;
		case Items::BeholderHexMdef: buff.player.values.push_back(BuffPacketValue::fromValue(2, skillInfo->mDef)); break;
		case Items::BeholderHexAcc: buff.player.values.push_back(BuffPacketValue::fromValue(2, skillInfo->acc)); break;
		case Items::BeholderHexAvo: buff.player.values.push_back(BuffPacketValue::fromValue(2, skillInfo->avo)); break;
		case Items::BeholderHexWatk: buff.player.values.push_back(BuffPacketValue::fromValue(2, skillInfo->wAtk)); break;
	}
	return buff;
}

auto SummonHandler::summonSkill(Player *player, PacketReader &reader) -> void {
	summon_id_t summonId = reader.get<summon_id_t>();
	Summon *summon = player->getSummons()->getSummon(summonId);
	if (summon == nullptr) {
		return;
	}

	skill_id_t skillId = reader.get<skill_id_t>();
	uint8_t display = reader.get<uint8_t>();
	skill_level_t level = player->getSkills()->getSkillLevel(skillId);
	auto skillInfo = ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level);
	if (skillInfo == nullptr) {
		// Hacking
		return;
	}
	switch (skillId) {
		case Skills::DarkKnight::HexOfBeholder: {
			int8_t buffId = reader.get<int8_t>();
			if (buffId < 0 || buffId > ((level - 1) / 5)) {
				// Hacking
				return;
			}

			item_id_t itemId = Items::BeholderHexWdef + buffId;
			seconds_t duration = skillInfo->buffTime;
			if (Buffs::addBuff(player, itemId, duration) == Result::Failure) {
				return;
			}
			break;
		}
		case Skills::DarkKnight::AuraOfBeholder:
			player->getStats()->modifyHp(skillInfo->hpProp);
			break;
		default:
			// Hacking
			return;
	}

	player->sendMap(Packets::summonSkill(player->getId(), skillId, display, level), true);
	player->sendMap(Packets::summonSkillEffect(player->getId(), Skills::DarkKnight::Beholder, display, level));
}

}