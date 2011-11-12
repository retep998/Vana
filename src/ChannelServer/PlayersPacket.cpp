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
#include "PlayersPacket.h"
#include "GameLogicUtilities.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "SkillConstants.h"

void PlayersPacket::showMoving(Player *player, unsigned char *buf, size_t size) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_MOVEMENT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(0);
	packet.addBuffer(buf, size);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::faceExpression(Player *player, int32_t face) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_EMOTE);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(face);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::showChat(Player *player, const string &msg, bool bubbleOnly) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_CHAT);
	packet.add<int32_t>(player->getId());
	packet.addBool(player->isGm());
	packet.addString(msg);
	packet.addBool(bubbleOnly);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PlayersPacket::damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, int8_t type, uint8_t stance, int32_t nodamageskill, const ReturnDamageInfo &pgmr) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	const int8_t BumpDamage = -1;
	const int8_t MapDamage = -2;

	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_DAMAGE);
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
				packet.addBool(pgmr.isPhysical); // Maybe? No Mana Reflection on global to test with
				packet.add<int32_t>(pgmr.mapMobId);
				packet.add<int8_t>(6);
				packet.addPos(pgmr.pos);
			}
			packet.add<int8_t>(stance);
			packet.add<int32_t>(dmg);
			if (nodamageskill > 0) {
				packet.add<int32_t>(nodamageskill);
			}
			break;
	}
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PlayersPacket::showInfo(Player *player, Player *getinfo, bool isself) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_INFO);
	packet.add<int32_t>(getinfo->getId());
	packet.add<int8_t>(getinfo->getStats()->getLevel());
	packet.add<int16_t>(getinfo->getStats()->getJob());
	packet.add<int16_t>(getinfo->getStats()->getFame());
	packet.addBool(false); // Married
	if (Guild *guild = PlayerDataProvider::Instance()->getGuild(getinfo->getGuildId())) {
		packet.addString(guild->name);
		if (Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(guild->allianceid)) {
			packet.addString(alliance->name);
		}
		else {
			packet.addString("");
		}
	}
	else {
		packet.addString("-");
		packet.addString("");
	}
	packet.addBool(isself); // Is 1 when the character is clicking themselves

	getinfo->getPets()->petInfoPacket(packet);
	getinfo->getMounts()->mountInfoPacket(packet);
	getinfo->getInventory()->wishListPacket(packet);
	getinfo->getMonsterBook()->infoData(packet);

	player->getSession()->send(packet);
}

void PlayersPacket::whisperPlayer(Player *target, const string &whisperer_name, uint16_t channel, const string &message) {
	PacketCreator packet;
	packet.addHeader(SMSG_COMMAND);
	packet.add<int8_t>(0x12);
	packet.addString(whisperer_name);
	packet.add<int16_t>(channel);
	packet.addString(message);
	target->getSession()->send(packet);
}

void PlayersPacket::findPlayer(Player *player, const string &name, int32_t map, uint8_t is, bool isChannel) {
	PacketCreator packet;
	packet.addHeader(SMSG_COMMAND);
	if (map != -1) {
		packet.add<int8_t>(0x09);
		packet.addString(name);
		if (map == -2) {
			// Player is in the cashshop
			packet.add<int8_t>(0x02);
		}
		else if (isChannel) {
			// Player is in the same channel
			packet.add<int8_t>(0x01);
			packet.add<int32_t>(map);
			packet.add<int32_t>(0);
		}
		else {
			// Player is in an other channel
			packet.add<int8_t>(0x03);
			packet.add<int32_t>(map);
		}
		packet.add<int32_t>(0);
	}
	else {
		packet.add<int8_t>(0x0A);
		packet.addString(name);
		packet.add<int8_t>(is);
	}

	player->getSession()->send(packet);
}

void PlayersPacket::sendToPlayers(unsigned char *data, int32_t len) {
	PacketCreator packet;
	packet.addBuffer(data, len);
	PlayerDataProvider::Instance()->sendPacket(packet);
}

void PlayersPacket::useMeleeAttack(Player *player, const Attack &attack) {
	int8_t tbyte = (attack.targets * 0x10) + attack.hits;
	int32_t skillid = attack.skillId;
	bool mesoexplosion = attack.isMesoExplosion;
	if (mesoexplosion) {
		tbyte = (attack.targets * 0x10) + 0x0A;
	}

	PacketCreator packet;
	packet.addHeader(SMSG_ATTACK_MELEE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<uint8_t>(attack.skillLevel);
	if (skillid != Jobs::All::RegularAttack) {
		packet.add<int32_t>(skillid);
	}

	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);

	int32_t masteryid = player->getSkills()->getMastery();
	packet.add<uint8_t>(masteryid > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryid)) : 0);
	packet.add<int32_t>(0);

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		packet.add<int32_t>(i->first);
		packet.add<int8_t>(0x06);
		if (mesoexplosion) {
			packet.add<uint8_t>(i->second.size());
		}
		for (Attack::diterator j = i->second.begin(); j != i->second.end(); ++j) {
			packet.add<int32_t>(*j);
		}
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useRangedAttack(Player *player, const Attack &attack) {
	int8_t tbyte = (attack.targets * 0x10) + attack.hits;
	int32_t skillid = attack.skillId;

	PacketCreator packet;
	packet.addHeader(SMSG_ATTACK_RANGED);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<uint8_t>(attack.skillLevel);
	if (skillid != Jobs::All::RegularAttack) {
		packet.add<int32_t>(skillid);
	}
	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);

	int32_t masteryid = player->getSkills()->getMastery();
	packet.add<uint8_t>(masteryid > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryid)) : 0);
	// Bug in global:
	// The colored swoosh does not display as it should

	packet.add<int32_t>(attack.starId);

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		packet.add<int32_t>(i->first);
		packet.add<int8_t>(0x06);
		for (Attack::diterator j = i->second.begin(); j != i->second.end(); ++j) {
			int32_t damage = *j;
			switch (skillid) {
				case Jobs::Marksman::Snipe: // Snipe is always crit
					damage += 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			packet.add<int32_t>(damage);
		}
	}
	packet.addPos(attack.projectilePos);

	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useSpellAttack(Player *player, const Attack &attack) {
	int8_t tbyte = (attack.targets * 0x10) + attack.hits;
	int32_t skillid = attack.skillId;

	PacketCreator packet;
	packet.addHeader(SMSG_ATTACK_MAGIC);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<uint8_t>(attack.skillLevel);
	packet.add<int32_t>(skillid);

	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);
	packet.add<uint8_t>(0); // Mastery byte is always 0 because spells don't have a swoosh

	packet.add<int32_t>(0); // No clue

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		packet.add<int32_t>(i->first);
		packet.add<int8_t>(0x06);
		for (Attack::diterator j = i->second.begin(); j != i->second.end(); ++j) {
			packet.add<int32_t>(*j);
		}
	}

	if (attack.charge > 0) {
		packet.add<int32_t>(attack.charge);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useEnergyChargeAttack(Player *player, const Attack &attack) {
	int8_t tbyte = (attack.targets * 0x10) + attack.hits;
	int32_t skillid = attack.skillId;

	PacketCreator packet;
	packet.addHeader(SMSG_ATTACK_ENERGYCHARGE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<int8_t>(attack.skillLevel);
	packet.add<int32_t>(skillid);

	packet.add<uint8_t>(attack.display);
	packet.add<uint8_t>(attack.animation);
	packet.add<uint8_t>(attack.weaponSpeed);

	int32_t masteryid = player->getSkills()->getMastery();
	packet.add<uint8_t>(masteryid > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryid)) : 0);

	packet.add<int32_t>(0);

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		packet.add<int32_t>(i->first);
		packet.add<int8_t>(0x06);
		for (Attack::diterator j = i->second.begin(); j != i->second.end(); ++j) {
			packet.add<int32_t>(*j);
		}
	}

	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useSummonAttack(Player *player, const Attack &attack) {
	PacketCreator packet;
	packet.addHeader(SMSG_SUMMON_ATTACK);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(attack.summonId);
	packet.add<int8_t>(attack.animation);
	packet.add<int8_t>(attack.targets);
	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		packet.add<int32_t>(i->first);
		packet.add<int8_t>(0x06);
		for (Attack::diterator j = i->second.begin(); j != i->second.end(); ++j) {
			packet.add<int32_t>(*j);
		}
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::usePirateGrenade(Player *player, int32_t posx, int32_t posy, int32_t charge, int32_t unknown) {
	PacketCreator packet;
	packet.addHeader(SMSG_PIRATE_GRENADE);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(posx);
	packet.add<int32_t>(posy);
	packet.add<int32_t>(charge);
	packet.add<int32_t>(unknown);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}