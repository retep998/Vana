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
#include "PlayersPacket.h"
#include "ChannelServer.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Pets.h"
#include "Player.h"
#include "Players.h"
#include "SendHeader.h"

void PlayersPacket::showMoving(Player *player, unsigned char *buf, size_t size) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_PLAYER);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(0);
	packet.addBuffer(buf, size);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::faceExpression(Player *player, int32_t face) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_FACE_EXPRESSION);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(face);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::showChat(Player *player, const string &msg, int8_t bubbleOnly) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CHAT);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(player->isGm());
	packet.addString(msg);
	packet.add<int8_t>(bubbleOnly);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PlayersPacket::damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, uint8_t type, uint8_t stance, int32_t nodamageskill, PGMRInfo pgmr) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_PLAYER);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(type);
	switch (type) {
		case 0xFE:
			packet.add<int32_t>(dmg);
			packet.add<int32_t>(dmg);
			break;
		default:
			packet.add<int32_t>(pgmr.reduction > 0 ? pgmr.damage : dmg);
			packet.add<int32_t>(mob);
			packet.add<int8_t>(hit);
			if (pgmr.reduction > 0) {
				packet.add<int8_t>(pgmr.reduction);
				packet.add<int8_t>(pgmr.isphysical); // Maybe? No Mana Reflection on global to test with
				packet.add<int32_t>(pgmr.mapmobid);
				packet.add<int8_t>(6);
				packet.addPos(pgmr.pos);
			}
			else
				packet.add<int8_t>(0);
			packet.add<int8_t>(stance);
			packet.add<int32_t>(dmg);
			if (nodamageskill > 0)
				packet.add<int32_t>(nodamageskill);
			break;
	}
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PlayersPacket::showMessage(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_NOTICE); 
	packet.add<int8_t>(type);
	packet.addString(msg);
	if (type == 6)
		packet.add<int32_t>(0);
	Players::Instance()->sendPacket(packet);
}

void PlayersPacket::showMessageWorld(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_PLAYERS);
	packet.add<int16_t>(SEND_NOTICE);
	packet.add<int8_t>(type);
	packet.addString(msg);
	if (type == 6)
		packet.add<int32_t>(0);
	ChannelServer::Instance()->sendToWorld(packet);
}

void PlayersPacket::showInfo(Player *player, Player *getinfo, uint8_t isself) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_PLAYER_INFO);
	packet.add<int32_t>(getinfo->getId());
	packet.add<int8_t>(getinfo->getLevel());
	packet.add<int16_t>(getinfo->getJob());
	packet.add<int16_t>(getinfo->getFame());
	packet.add<int8_t>(0); // Married
	packet.addString("-"); // Guild
	packet.addString(""); // Guild Alliance
	packet.add<int8_t>(isself); // Is 1 when the character is clicking themselves

	getinfo->getPets()->petInfoPacket(packet);
	getinfo->getMounts()->mountInfoPacket(packet);
	getinfo->getInventory()->wishListPacket(packet);
	getinfo->getMonsterBook()->infoData(packet);

	player->getSession()->send(packet);
}

void PlayersPacket::whisperPlayer(Player *target, const string &whisperer_name, uint16_t channel, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_COMMAND_RESPOND);
	packet.add<int8_t>(0x12);
	packet.addString(whisperer_name);
	packet.add<int16_t>(channel);
	packet.addString(message);
	target->getSession()->send(packet);
}

void PlayersPacket::findPlayer(Player *player, const string &name, int32_t map, uint8_t is, bool is_channel) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_COMMAND_RESPOND);
	if (map != -1) {
		packet.add<int8_t>(0x09);
		packet.addString(name);
		packet.add<int8_t>(is_channel ? 0x03 : 0x01);
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

void PlayersPacket::sendToPlayers(unsigned char *data, int32_t len) {
	PacketCreator packet;
	packet.addBuffer(data, len);
	Players::Instance()->sendPacket(packet);
}

void PlayersPacket::useMeleeAttack(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	uint8_t targets = tbyte / 0x10;
	uint8_t hits = tbyte % 0x10;
	int32_t skillid = pack.get<int32_t>();
	bool mesoexplosion = false;
	if (skillid == Jobs::ChiefBandit::MesoExplosion) {
		tbyte = (targets * 0x10) + 0x0A;
		mesoexplosion = true;
	}
	PacketCreator packet;
	packet.add<int16_t>(SEND_ATTACK_MELEE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<int8_t>(player->getSkills()->getSkillLevel(skillid));
	if (skillid != Jobs::All::RegularAttack) {
		packet.add<int32_t>(skillid);
	} 

	pack.skipBytes(4); // Unk
	packet.add<int8_t>(pack.get<int8_t>()); // Projectile display
	switch (skillid) {
		case Jobs::Gunslinger::Grenade:
		case Jobs::Brawler::CorkscrewBlow:
			pack.skipBytes(4); // Charge
			break;
	}
	packet.add<int8_t>(pack.get<int8_t>()); // Direction/animation
	pack.skipBytes(1); // Weapon subclass
	packet.add<int8_t>(pack.get<int8_t>()); // Weapon speed
	pack.skipBytes(4); // Ticks

	int32_t masteryid = player->getSkills()->getMastery();
	packet.add<int8_t>(masteryid > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryid)) : 0);
	packet.add<int32_t>(0);

	for (int8_t i = 0; i < targets; i++) {
		int32_t mapmobid = pack.get<int32_t>();
		packet.add<int32_t>(mapmobid);
		packet.add<int8_t>(0x06);
		pack.skipBytes(12);
		if (mesoexplosion) {
			hits = pack.get<int8_t>();
			packet.add<int8_t>(hits);
		}
		else
			pack.skipBytes(2);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			packet.add<int32_t>(damage);
		}
		pack.skipBytes(4);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useRangedAttack(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = pack.get<int32_t>();
	pack.skipBytes(4); // Unk
	switch (skillid) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::WindArcher::Hurricane:
		case Jobs::Corsair::RapidFire:
			pack.skipBytes(4);
			break;
	}
	bool shadow_meso = (skillid == Jobs::Hermit::ShadowMeso);

	uint8_t display = pack.get<int8_t>(); // Projectile display
	uint8_t animation = pack.get<int8_t>(); // Direction/animation
	uint8_t w_class = pack.get<int8_t>(); // Weapon subclass
	uint8_t w_speed = pack.get<int8_t>(); // Weapon speed
	pack.skipBytes(4); // Ticks
	int16_t slot = pack.get<int16_t>(); // Slot
	int16_t csstar = pack.get<int16_t>(); // Cash Shop star

	if (!shadow_meso && player->getActiveBuffs()->hasShadowStars()) {
		pack.skipBytes(4); // Shadow Stars star ID
	}
	PacketCreator packet;
	packet.add<int16_t>(SEND_ATTACK_RANGED);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<int8_t>(player->getSkills()->getSkillLevel(skillid));
	if (skillid != Jobs::All::RegularAttack) {
		packet.add<int32_t>(skillid);
	}
	packet.add<int8_t>(display);
	packet.add<int8_t>(animation);
	packet.add<int8_t>(w_speed);

	int32_t masteryid = player->getSkills()->getMastery();
	packet.add<int8_t>(masteryid > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryid)) : 0);
	// Bug in global:
	// The colored swoosh does not display as it should

	int32_t itemid = 0;
	if (!shadow_meso) {
		if (csstar > 0)
			itemid = player->getInventory()->getItem(Inventories::CashInventory, csstar)->id;
		else if (slot > 0) {
			Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
			if (item != 0)
				itemid = item->id;
		}
	}
	packet.add<int32_t>(itemid);
	pack.skipBytes(1); // 0x00 = AoE, 0x41 = other
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int8_t>(0x06);
		pack.skipBytes(14);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			switch (skillid) {
				case Jobs::Marksman::Snipe: // Snipe is always crit
					damage += 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			packet.add<int32_t>(damage); 
		}
		pack.skipBytes(4);
	}
	pack.skipBytes(4);
	packet.add<int32_t>(pack.get<int32_t>()); // Position
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useSpellAttack(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	PacketCreator packet;
	packet.add<int16_t>(SEND_ATTACK_SPELL);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<int8_t>(1); // Spells are always a skill
	int32_t skillid = pack.get<int32_t>();
	int32_t charge = 0;
	packet.add<int32_t>(skillid);

	pack.skipBytes(4); // Unk
	packet.add<int8_t>(pack.get<int8_t>()); // Projectile display

	switch (skillid) {
		case Jobs::FPArchMage::BigBang:
		case Jobs::ILArchMage::BigBang:
		case Jobs::Bishop::BigBang: // Big Bang has a 4 byte charge time after skillid
			charge = pack.get<int32_t>();
			break;
	}
	packet.add<int8_t>(pack.get<int8_t>()); // Direction/animation
	packet.add<int8_t>(pack.get<int8_t>()); // Casting speed
	pack.skipBytes(1); // Weapon subclass
	pack.skipBytes(4); // Ticks
	packet.add<int8_t>(0); // Mastery byte is always 0 because spells don't have a swoosh
	packet.add<int32_t>(0); // No clue
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int8_t>(-1);
		pack.skipBytes(3); // Useless crap for display
		pack.skipBytes(1); // State
		pack.skipBytes(10); // Useless crap for display continued
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			packet.add<int32_t>(damage);
		}
		pack.skipBytes(4);
	}
	if (charge > 0)
		packet.add<int32_t>(charge);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useEnergyChargeAttack(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	// Not sure about this packet at the moment, will finish later

	//PacketCreator packet;
	//packet.add<int16_t>(SEND_ATTACK_ENERGYCHARGE);
	//packet.add<int32_t>(player->getId());
	//packet.add<int8_t>(tbyte);
	//packet.add<int8_t>(1);
	//int32_t skillid = pack.get<int32_t>();
	//packet.add<int32_t>(skillid);

	//Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::useSummonAttack(Player *player, PacketReader &pack) {
	int32_t summonid = pack.get<int32_t>();
	pack.skipBytes(5);
	int8_t targets = pack.get<int8_t>();
	PacketCreator packet;
	packet.add<int16_t>(SEND_ATTACK_SUMMON);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(summonid);
	packet.add<int8_t>(4);
	packet.add<int8_t>(targets);
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int8_t>(6);

		pack.skipBytes(14); // Crap

		int32_t damage = pack.get<int32_t>();
		packet.add<int32_t>(damage);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}