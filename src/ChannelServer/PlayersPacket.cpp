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
#include "PlayersPacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Session.h"
#include "SmsgHeader.h"

auto PlayersPacket::showMoving(Player *player, unsigned char *buf, size_t size) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_MOVEMENT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(0);
	packet.addBuffer(buf, size);
	player->getMap()->sendPacket(packet, player);
}

auto PlayersPacket::faceExpression(Player *player, int32_t face) -> void {
	if (player->isUsingGmHide())
		return;
	PacketCreator packet;
	packet.add<header_t>(SMSG_EMOTE);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(face);
	player->getMap()->sendPacket(packet, player);
}

auto PlayersPacket::showChat(Player *player, const string_t &msg, bool bubbleOnly) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_CHAT);
	packet.add<int32_t>(player->getId());
	packet.add<bool>(player->isGm());
	packet.addString(msg);
	packet.add<bool>(bubbleOnly);
	player->getMap()->sendPacket(packet);
}

auto PlayersPacket::damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, int8_t type, uint8_t stance, int32_t noDamageSkill, const ReturnDamageInfo &pgmr) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	const int8_t BumpDamage = -1;
	const int8_t MapDamage = -2;

	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_DAMAGE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(type);
	switch (type) {
		case MapDamage:
			packet.add<int32_t>(dmg);
			packet.add<int32_t>(dmg);
			break;
		default:
			packet.add<int32_t>(pgmr.reduction > 0 ? pgmr.damage : dmg);
			packet.add<int32_t>(mob);
			packet.add<int8_t>(hit);
			packet.add<int8_t>(pgmr.reduction);
			if (pgmr.reduction > 0) {
				packet.add<bool>(pgmr.isPhysical); // Maybe? No Mana Reflection on global to test with
				packet.add<int32_t>(pgmr.mapMobId);
				packet.add<int8_t>(6);
				packet.addClass<Pos>(pgmr.pos);
			}
			packet.add<int8_t>(stance);
			packet.add<int32_t>(dmg);
			if (noDamageSkill > 0) {
				packet.add<int32_t>(noDamageSkill);
			}
			break;
	}
	player->getMap()->sendPacket(packet);
}

auto PlayersPacket::showInfo(Player *player, Player *getInfo, bool isSelf) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_INFO);
	packet.add<int32_t>(getInfo->getId());
	packet.add<int8_t>(getInfo->getStats()->getLevel());
	packet.add<int16_t>(getInfo->getStats()->getJob());
	packet.add<int16_t>(getInfo->getStats()->getFame());
	packet.add<bool>(false); // Married
	packet.addString("-"); // Guild
	packet.addString(""); // Guild Alliance
	packet.add<bool>(isSelf); // Is 1 when the character is clicking themselves

	getInfo->getPets()->petInfoPacket(packet);
	getInfo->getMounts()->mountInfoPacket(packet);
	getInfo->getInventory()->wishListPacket(packet);
	getInfo->getMonsterBook()->infoData(packet);

	player->getSession()->send(packet);
}

auto PlayersPacket::whisperPlayer(Player *target, const string_t &whispererName, uint16_t channel, const string_t &message) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_COMMAND);
	packet.add<int8_t>(0x12);
	packet.addString(whispererName);
	packet.add<int16_t>(channel);
	packet.addString(message);
	target->getSession()->send(packet);
}

auto PlayersPacket::findPlayer(Player *player, const string_t &name, int32_t map, uint8_t is, bool isChannel) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_COMMAND);
	if (map != -1) {
		packet.add<int8_t>(0x09);
		packet.addString(name);
		packet.add<int8_t>(isChannel ? 0x03 : 0x01);
		packet.add<int32_t>(map);
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
	}
	else {
		packet.add<int8_t>(0x0A);
		packet.addString(name);
		packet.add<int8_t>(is);
	}

	player->getSession()->send(packet);
}

auto PlayersPacket::sendToPlayers(unsigned char *data, int32_t len) -> void {
	PacketCreator packet;
	packet.addBuffer(data, len);
	PlayerDataProvider::getInstance().sendPacket(packet);
}

auto PlayersPacket::useMeleeAttack(Player *player, const Attack &attack) -> void {
	int8_t hitByte = (attack.targets * 0x10) + attack.hits;
	int32_t skillId = attack.skillId;
	bool isMesoExplosion = attack.isMesoExplosion;
	if (isMesoExplosion) {
		hitByte = (attack.targets * 0x10) + 0x0A;
	}

	PacketCreator packet;
	packet.add<header_t>(SMSG_ATTACK_MELEE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(hitByte);
	packet.add<uint8_t>(attack.skillLevel);
	if (skillId != Skills::All::RegularAttack) {
		packet.add<int32_t>(skillId);
	}

	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);

	int32_t masteryId = player->getSkills()->getMastery();
	packet.add<uint8_t>(masteryId > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryId)) : 0);
	packet.add<int32_t>(0);

	for (const auto &target : attack.damages) {
		packet.add<int32_t>(target.first);
		packet.add<int8_t>(0x06);
		if (isMesoExplosion) {
			packet.add<uint8_t>(target.second.size());
		}
		for (const auto &hit : target.second) {
			packet.add<int32_t>(hit);
		}
	}
	player->getMap()->sendPacket(packet, player);
}

auto PlayersPacket::useRangedAttack(Player *player, const Attack &attack) -> void {
	int8_t tbyte = (attack.targets * 0x10) + attack.hits;
	int32_t skillId = attack.skillId;

	PacketCreator packet;
	packet.add<header_t>(SMSG_ATTACK_RANGED);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<uint8_t>(attack.skillLevel);
	if (skillId != Skills::All::RegularAttack) {
		packet.add<int32_t>(skillId);
	}
	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);

	int32_t masteryId = player->getSkills()->getMastery();
	packet.add<uint8_t>(masteryId > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryId)) : 0);
	// Bug in global:
	// The colored swoosh does not display as it should

	packet.add<int32_t>(attack.starId);

	for (const auto &target : attack.damages) {
		packet.add<int32_t>(target.first);
		packet.add<int8_t>(0x06);
		for (const auto &hit : target.second) {
			int32_t damage = hit;
			switch (skillId) {
				case Skills::Marksman::Snipe: // Snipe is always crit
					damage += 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			packet.add<int32_t>(damage);
		}
	}
	packet.addClass<Pos>(attack.projectilePos);

	player->getMap()->sendPacket(packet, player);
}

auto PlayersPacket::useSpellAttack(Player *player, const Attack &attack) -> void {
	int8_t tbyte = (attack.targets * 0x10) + attack.hits;
	int32_t skillId = attack.skillId;

	PacketCreator packet;
	packet.add<header_t>(SMSG_ATTACK_MAGIC);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<uint8_t>(attack.skillLevel);
	packet.add<int32_t>(skillId);

	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);
	packet.add<uint8_t>(0); // Mastery byte is always 0 because spells don't have a swoosh

	packet.add<int32_t>(0); // No clue

	for (const auto &target : attack.damages) {
		packet.add<int32_t>(target.first);
		packet.add<int8_t>(0x06);
		for (const auto &hit : target.second) {
			packet.add<int32_t>(hit);
		}
	}

	if (attack.charge > 0) {
		packet.add<int32_t>(attack.charge);
	}
	player->getMap()->sendPacket(packet, player);
}

auto PlayersPacket::useSummonAttack(Player *player, const Attack &attack) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SUMMON_ATTACK);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(attack.summonId);
	packet.add<int8_t>(attack.animation);
	packet.add<int8_t>(attack.targets);
	for (const auto &target : attack.damages) {
		packet.add<int32_t>(target.first);
		packet.add<int8_t>(0x06);
		for (const auto &hit : target.second) {
			packet.add<int32_t>(hit);
		}
	}
	player->getMap()->sendPacket(packet, player);
}

auto PlayersPacket::useEnergyChargeAttack(Player *player, const Attack &attack) -> void {
	int8_t tbyte = (attack.targets * 0x10) + attack.hits;
	int32_t skillId = attack.skillId;

	PacketCreator packet;
	packet.add<header_t>(SMSG_ATTACK_ENERGYCHARGE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<int8_t>(attack.skillLevel);
	packet.add<int32_t>(skillId);

	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);

	int32_t masteryId = player->getSkills()->getMastery();
	packet.add<uint8_t>(masteryId > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryId)) : 0);

	packet.add<int32_t>(0);

	for (const auto &target : attack.damages) {
		packet.add<int32_t>(target.first);
		packet.add<int8_t>(0x06);
		for (const auto &hit : target.second) {
			packet.add<int32_t>(hit);
		}
	}

	player->getMap()->sendPacket(packet, player);
}