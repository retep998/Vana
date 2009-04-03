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
#include "PlayerHandler.h"
#include "GameConstants.h"
#include "Maps.h"
#include "Mobs.h"
#include "Movement.h"
#include "Player.h"
#include "Players.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "PacketReader.h"
#include "SkillsPacket.h"

void PlayerHandler::handleDamage(Player *player, PacketReader &packet) {
	packet.skipBytes(4); // Ticks
	uint8_t type = packet.get<int8_t>();
	uint8_t hit = 0;
	uint8_t stance = 0;
	packet.skipBytes(1); // Element - 0x00 = elementless, 0x01 = ice, 0x02 = fire, 0x03 = lightning
	int16_t job = player->getJob();
	int16_t disease = 0;
	int32_t mapmobid = 0; // Map Mob ID
	Mob *mob = 0;
	int32_t mobid = 0; // Actual Mob ID - i.e. 8800000 for Zak
	int32_t nodamageid = 0;
	int32_t damage = packet.get<int32_t>();
	bool applieddamage = false;
	PGMRInfo pgmr;
	MobAttackInfo attack;
	switch (type) {
		case 0xFE: // Map/fall damage is an oddball packet
			break;
		default: // Code in common, minimizes repeated code
			packet.skipBytes(4); // Mob ID
			mapmobid = packet.get<int32_t>();
			mob = Maps::getMap(player->getMap())->getMob(mapmobid);
			if (mob == 0) {
				// Hacking
				return;
			}

			mobid = mob->getMobID();
			if (type != 0xFF) {
				try {
					attack = mob->getAttackInfo(type);
					disease = attack.disease;
				}
				catch (std::out_of_range) {
					// Not having data about linked monsters causes crashes with linked monsters
					// For example - those quest Dark Lords or the El Nath PQ Lycanthropes
					// We have no way of transferring the data at the moment

					// Now we leave attack and disease at their default values
				}
			}
			hit = packet.get<int8_t>(); // Knock direction
			break;
	}
	switch (type) { // Account for special sections of the damage packet
		case 0xFE:
			break;
		default: // Else: Power Guard/Mana Reflection
			pgmr.reduction = packet.get<int8_t>();
			packet.skipBytes(1); // I think reduction is a short, but it's a byte in the S -> C packet, so..
			if (pgmr.reduction != 0) {
				if (packet.get<int8_t>() == 0)
					pgmr.isphysical = false;
				pgmr.mapmobid = packet.get<int32_t>();
				packet.skipBytes(1); // 0x06 for Power Guard, 0x00 for Mana Reflection?
				packet.skipBytes(4); // Mob position garbage
				pgmr.pos.x = packet.get<int16_t>();
				pgmr.pos.y = packet.get<int16_t>();
				pgmr.damage = damage;
				if (pgmr.isphysical) // Only Power Guard decreases damage
					damage = (damage - (damage * pgmr.reduction / 100)); 
				mob->applyDamage(player->getId(), (pgmr.damage * pgmr.reduction / 100));
			}
			break;
	}
	switch (type) { // Packet endings
		case 0xFE:
			disease = packet.get<int16_t>(); // Disease
			break;
		default:  {
			stance = packet.get<int8_t>(); // Power Stance
			if (stance > 0) {
				int32_t skillid = 0;
				if (player->getActiveBuffs()->getActiveSkillLevel(Hero::POWERSTANCE) > 0)
					skillid = Hero::POWERSTANCE;
				else if (player->getActiveBuffs()->getActiveSkillLevel(Paladin::POWERSTANCE) > 0)
					skillid = Paladin::POWERSTANCE;
				else if (player->getActiveBuffs()->getActiveSkillLevel(DarkKnight::POWERSTANCE) > 0)
					skillid = DarkKnight::POWERSTANCE;
				else if (player->getActiveBuffs()->getActiveSkillLevel(Marauder::ENERGYCHARGE) > 0)
					skillid = Marauder::ENERGYCHARGE;
				if (skillid == 0 || player->getSkills()->getSkillLevel(skillid) == 0) {
					// Hacking
					return;
				}
			}
			break;
		}
	}
	if (damage == -1) {
		switch (job) {
			case 412: nodamageid = NightLord::SHADOWSHIFTER; break; // Fake
			case 422: nodamageid = Shadower::SHADOWSHIFTER; break; // Fake
			case 112: nodamageid = Hero::GUARDIAN; break; // Guardian
			case 122: nodamageid = Paladin::GUARDIAN; break; // Guardian
		}
		if (nodamageid == 0 || player->getSkills()->getSkillLevel(nodamageid) == 0) {
			// Hacking
			return;
		}
	}
	if (disease > 0 && damage != 0) { // Fake/Guardian don't prevent disease
		// Status ailment processing here
	}
	if (damage > 0 && !player->hasGMEquip()) {
		if (player->getActiveBuffs()->getActiveSkillLevel(ChiefBandit::MESOGUARD) > 0 && player->getInventory()->getMesos() > 0) { // Meso Guard 
			int16_t mesorate = Skills::skills[ChiefBandit::MESOGUARD][player->getActiveBuffs()->getActiveSkillLevel(ChiefBandit::MESOGUARD)].x; // Meso Guard meso %
			int16_t hp = player->getHP();
			int16_t mesoloss = (int16_t)(mesorate * damage / 2 / 100);
			int32_t mesos = player->getInventory()->getMesos();
			int32_t newmesos = mesos - mesoloss;
			if (newmesos < 0) { // Special damage calculation for not having enough mesos
				double mesos2 = mesos + 0.0; // You can't get a double from math involving 2 ints, even if a decimal results
				double reduction = 2.0 - ((mesos2 / mesoloss) / 2);
				damage = (uint16_t)(damage / reduction); // This puts us pretty close to the damage observed clientside, needs improvement
			}
			else
				damage /= 2; // Usually displays 1 below the actual damage but is sometimes accurate - no clue why
			player->getInventory()->setMesos(newmesos);
			SkillsPacket::showSkillEffect(player, ChiefBandit::MESOGUARD);
			player->damageHP((uint16_t) damage);
			if (attack.deadlyattack && player->getMP() > 0)
				player->setMP(1);
			if (attack.mpburn > 0)
				player->damageMP(attack.mpburn);
			applieddamage = true;
		}
		if (player->getActiveBuffs()->getActiveSkillLevel(Magician::MAGICGUARD) > 0) { // Magic Guard
			int16_t mp = player->getMP();
			int16_t hp = player->getHP();
			if (attack.deadlyattack) {
				if (mp > 0)
					player->setMP(1);
				player->setHP(1);
			}
			else if (attack.mpburn > 0) {
				player->damageMP(attack.mpburn);
				player->damageHP((uint16_t) damage);
			}
			else {
				int16_t reduc = Skills::skills[Magician::MAGICGUARD][player->getActiveBuffs()->getActiveSkillLevel(Magician::MAGICGUARD)].x;
				uint16_t mpdamage = (uint16_t)((damage * reduc) / 100);
				uint16_t hpdamage = (uint16_t)(damage - mpdamage);
				bool ison = false;
				if (job % 10 == 2) {
					int32_t infinity = player->getJob() * 10000 + 1004;
					if (player->getActiveBuffs()->getActiveSkillLevel(infinity) > 0)
						ison = true;
				}
				if (mpdamage < mp || ison) {
					player->damageMP(mpdamage);
					player->damageHP(hpdamage);
				}
				else if (mpdamage >= mp) {
					player->setMP(0);
					player->damageHP(hpdamage + (mpdamage - mp));
				}
			}
			applieddamage = true;
		}
		if (((job / 100) == 1) && ((job % 10) == 2)) { // Achilles for 4th job warriors
			float achx = 1000.0;
			int32_t sid = Hero::ACHILLES;
			switch (job) {
				case 112: sid = Hero::ACHILLES; break;
				case 122: sid = Paladin::ACHILLES; break;
				case 132: sid = DarkKnight::ACHILLES; break;
			}
			uint8_t slv = player->getSkills()->getSkillLevel(sid);
			if (slv > 0)
				achx = Skills::skills[sid][slv].x;
			double red = (2.0 - achx / 1000.0);
			player->damageHP((uint16_t) (damage / red));
			if (attack.deadlyattack && player->getMP() > 0)
				player->setMP(1);
			if (attack.mpburn > 0)
				player->damageMP(attack.mpburn);
			applieddamage = true;
		}
		if (applieddamage == false) {
			if (attack.deadlyattack) {
				if (player->getMP() > 0)
					player->setMP(1);
				player->setHP(1);
			}
			else
				player->damageHP((uint16_t) damage);
			if (attack.mpburn > 0)
				player->damageMP(attack.mpburn);
		}
	}
	PlayersPacket::damagePlayer(player, damage, mobid, hit, type, stance, nodamageid, pgmr);
}

void PlayerHandler::handleFacialExpression(Player *player, PacketReader &packet) {
	int32_t face = packet.get<int32_t>();
	PlayersPacket::faceExpression(player, face);
}

void PlayerHandler::handleGetInfo(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int32_t playerid = packet.get<int32_t>();
	PlayersPacket::showInfo(player, Players::Instance()->getPlayer(playerid), packet.get<int8_t>());
}

void PlayerHandler::handleHeal(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t hp = packet.get<int16_t>();
	int16_t mp = packet.get<int16_t>();
	player->modifyHP(hp);
	player->modifyMP(mp);
}

void PlayerHandler::handleMoving(Player *player, PacketReader &packet) {
	packet.reset(7);
	Movement::parseMovement(player, packet);
	packet.reset(7);
	PlayersPacket::showMoving(player, packet.getBuffer(), packet.getBufferLength());
}

void PlayerHandler::handleSpecialSkills(Player *player, PacketReader &packet) {
	int32_t skillid = packet.get<int32_t>();
	switch (skillid) {
		case Hero::MONSTERMAGNET: // Monster Magnet x3
		case Paladin::MONSTERMAGNET:
		case DarkKnight::MONSTERMAGNET:
		case Marksman::PIERCINGARROW: // Pierce
		case FPArchMage::BIGBANG: // Big Bang x3
		case ILArchMage::BIGBANG:
		case Bishop::BIGBANG: {
			SpecialSkillInfo info;
			info.skillid = skillid;
			info.level = packet.get<int8_t>();
			info.direction = packet.get<int8_t>();
			info.w_speed = packet.get<int8_t>();
			player->setSpecialSkill(info);
			SkillsPacket::showSpecialSkill(player, info);
			break;
		}
		case ChiefBandit::CHAKRA: { // Chakra
			int16_t dex = player->getDex();
			int16_t luk = player->getLuk();
			int16_t recovery = Skills::skills[skillid][player->getSkills()->getSkillLevel(skillid)].y;
			int16_t maximum = (luk * 66 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			int16_t minimum = (luk * 33 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			// Maximum = (luk * 6.6 + dex) * 0.2 * (recovery% / 100 + 1)
			// Minimum = (luk * 3.3 + dex) * 0.2 * (recovery% / 100 + 1)
			// I used 66 / 10 and 2 / 10 respectively to get 6.6 and 0.2 without using floating points
			int16_t range = maximum - minimum;
			player->modifyHP(Randomizer::Instance()->randShort(range) + minimum);
			break;
		}
	}
}