/*
Copyright (C) 2008 Vana Development Team

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
#include "Maps.h"
#include "Mobs.h"
#include "Movement.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "ReadPacket.h"
#include "SkillsPacket.h"
#include <iostream>

void PlayerHandler::handleDamage(Player *player, ReadPacket *packet) {
	packet->skipBytes(4); // Ticks
	unsigned char type = packet->getByte();
	unsigned char hit = 0;
	unsigned char stance = 0;
	packet->skipBytes(1); // Element - 0x00 = elementless, 0x01 = ice, 0x02 = fire, 0x03 = lightning
	int damage = packet->getInt();
	int mobid = 0; // Actual Mob ID - i.e. 8800000 for Zak
	int mapmobid = 0; // Map Mob ID
	int nodamageid = 0;
	short job = player->getJob();
	short disease = 0;
	bool applieddamage = false;
	PGMRInfo pgmr;
	MobAttackInfo attack;
	switch (type) {
		case 0xFE: // Map/fall damage is an oddball packet
			break;
		default: // Code in common, minimizes repeated code
			mobid = packet->getInt();
			if (Mobs::mobinfo.find(mobid) == Mobs::mobinfo.end()) {
				// Hacking
				return;
			}
			mapmobid = packet->getInt();
			if (type != 0xFF) {
				try {
					attack = Mobs::mobinfo[mobid].skills.at(type);
					disease = attack.disease;
				}
				catch (std::out_of_range) {
					// Not having data about linked monsters causes crashes with linked monsters
					// For example - those quest Dark Lords or the El Nath PQ Lycanthropes
					// We have no way of transferring the data at the moment

					// Now we leave attack and disease at their default values
				}
			}
			hit = packet->getByte(); // Knock direction
			break;
	}
	switch (type) { // Account for special sections of the damage packet
		case 0xFE:
			break;		
		default: // Else: Power Guard/Mana Reflection
			pgmr.reduction = packet->getByte();
			packet->skipBytes(1); // I think reduction is a short, but it's a byte in the S -> C packet, so..
			if (pgmr.reduction != 0) {
				if (packet->getByte() == 0)
					pgmr.isphysical = false; // Initialized as true, so the opposite case doesn't matter
				pgmr.mapmobid = packet->getInt();
				packet->skipBytes(1); // 0x06 for Power Guard, 0x00 for Mana Reflection?
				packet->skipBytes(4); // Mob position garbage
				pgmr.pos_x = packet->getShort();
				pgmr.pos_y = packet->getShort();
				pgmr.damage = damage;
				if (pgmr.isphysical) // Mana Reflection does not decrease damage
					damage = (damage - (damage * pgmr.reduction / 100)); 
				Mob *mob = Maps::maps[player->getMap()]->getMob(mapmobid);
				if (mob != 0) {
					mob->setHP(mob->getHP() - (pgmr.damage * pgmr.reduction / 100));
					Mobs::displayHPBars(player, mob);
					if (mob->getHP() <= 0)
						mob->die(player);
				}
			}
			break;
	}
	switch (type) { // Yes, another one, end of packets
		case 0xFE:
			disease = packet->getShort(); // Disease, normal end of packet
			break;
		default:  {
			stance = packet->getByte(); // Power Stance, normal end of packet
			if (stance > 0) {
				int skillid = 0;
				if (player->getSkills()->getActiveSkillLevel(1121002) > 0)
					skillid = 1121002;
				else if (player->getSkills()->getActiveSkillLevel(1221002) > 0)
					skillid = 1221002;
				else if (player->getSkills()->getActiveSkillLevel(1321002) > 0)
					skillid = 1321002;
				if (skillid == 0 || player->getSkills()->getSkillLevel(skillid) == 0) {
					// Hacking
					return;
				}
			}
			break;
		}
	}
	if (damage == -1) { // 0 damage = regular miss, -1 = Fake/Guardian
		short job = player->getJob();
		switch (job) {
			case 412: nodamageid = 4120002; break; // Fake
			case 422: nodamageid = 4220002; break; // Fake
			case 112: nodamageid = 1120005; break; // Guardian
			case 122: nodamageid = 1220006; break; // Guardian
		}
		if (player->getSkills()->getSkillLevel(nodamageid) < 1 || nodamageid == 0) {
			//hacking
			return;
		}
	}
	if (disease > 0 && damage != 0) { // Fake/Guardian don't prevent disease
		// Status ailment processing here
	}
	if (damage > 0 && !player->hasGMEquip()) {
		if (player->getSkills()->getActiveSkillLevel(4211005) > 0 && player->getInventory()->getMesos() > 0) { // Meso Guard 
			int mesorate = Skills::skills[4211005][player->getSkills()->getActiveSkillLevel(4211005)].x; // Meso Guard meso %
			unsigned short hp = player->getHP();
			int mesoloss = (int)(mesorate * (damage / 2) / 100);
			int mesos = player->getInventory()->getMesos();
			int newmesos = mesos - mesoloss;
			if (newmesos > -1) {
				damage = (int)(damage / 2); // Usually displays 1 below the actual damage but is sometimes accurate - no clue why
			}
			else { // Special damage calculation for not having enough mesos
				double mesos2 = mesos + 0.0; // You can't get a double from math involving 2 ints, even if a decimal results
				double reduction = 2.0 - ((mesos2 / mesoloss) / 2);
				damage = (int)(damage / reduction); // This puts us pretty close to the damage observed clientside, needs improvement
			}
			player->getInventory()->setMesos(newmesos);
			SkillsPacket::showSkillEffect(player, 4211005);
			player->setHP(player->getHP() - damage);
			if (attack.deadlyattack && player->getMP() > 0)
				player->setMP(1);
			if (attack.mpburn > 0)
				player->setMP(player->getMP() - attack.mpburn);
			applieddamage = true;
		}
		if (player->getSkills()->getActiveSkillLevel(2001002) > 0) { // Magic Guard
			unsigned short mp = player->getMP();
			unsigned short hp = player->getHP();
			if (attack.deadlyattack) {
				if (mp > 0)
					player->setMP(1);
				player->setHP(1);
			}
			else if (attack.mpburn > 0) {
				player->setMP(mp - attack.mpburn);
				player->setHP(hp - damage);
			}
			else {
				unsigned short reduc = Skills::skills[2001002][player->getSkills()->getActiveSkillLevel(2001002)].x;
				int mpdamage = ((damage * reduc) / 100);
				int hpdamage = damage - mpdamage;
				bool ison = false;
				if (player->getJob() % 10 == 2) {
						int infinity = player->getJob() * 10000 + 1004;
						if (player->getSkills()->getActiveSkillLevel(infinity) > 0)
							ison = true;
				}
				if (mpdamage < mp || ison) {
					player->setMP(mp - mpdamage);
					player->setHP(hp - hpdamage);
				}
				else if (mpdamage >= mp) {
					player->setMP(0);
					player->setHP(hp - (hpdamage + (mpdamage - mp)));
				}
			}
			applieddamage = true;
		}
		if (((job / 100) == 1) && ((job % 10) == 2)) { // Achilles for 4th job warriors
			float achx = 1000.0;
			int sid = 1120004;
			switch (job) {
				case 112: sid = 1120004; break;
				case 122: sid = 1220005; break;
				case 132: sid = 1230005; break;
			}
			int slv = player->getSkills()->getSkillLevel(sid);
			if (slv > 0) { achx = Skills::skills[sid][slv].x; }
			double red = (2.0 - achx / 1000.0);
			player->setHP(player->getHP() - (int)(damage / red));
			if (attack.deadlyattack && player->getMP() > 0)
				player->setMP(1);
			if (attack.mpburn > 0)
				player->setMP(player->getMP() - attack.mpburn);
			applieddamage = true;
		}
		if (applieddamage == false) {
			if (attack.deadlyattack) {
				if (player->getMP() > 0)
					player->setMP(1);
				player->setHP(1);
			}
			else
				player->setHP(player->getHP() - damage);
			if (attack.mpburn > 0)
				player->setMP(player->getMP() - attack.mpburn);
		}
	}
	PlayersPacket::damagePlayer(player, damage, mobid, hit, type, stance, nodamageid, pgmr);
}

void PlayerHandler::handleFacialExpression(Player *player, ReadPacket *packet) {
	int face = packet->getInt();
	PlayersPacket::faceExpression(player, face);
}

void PlayerHandler::handleGetInfo(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int playerid = packet->getInt();
	PlayersPacket::showInfo(player, Players::Instance()->getPlayer(playerid), packet->getByte());
}

void PlayerHandler::handleHeal(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short hp = packet->getShort();
	short mp = packet->getShort();
	player->setHP(player->getHP() + hp);
	player->setMP(player->getMP() + mp);
}

void PlayerHandler::handleMoving(Player *player, ReadPacket *packet) {
	packet->reset(7);
	Movement::parseMovement(player, packet);
	packet->reset(7);
	PlayersPacket::showMoving(player, packet->getBuffer(), packet->getBufferLength());
}

void PlayerHandler::handleSpecialSkills(Player *player, ReadPacket *packet) {
	int skillid = packet->getInt();
	switch (skillid) {
		case 1121001: // Monster Magnet
		case 1221001:
		case 1321001:
		case 3221001: // Pierce
		case 2121001: // Big Bang x3
		case 2221001:
		case 2321001: {
			SpecialSkillInfo info;
			info.skillid = skillid;
			info.level = packet->getByte();
			info.direction = packet->getByte();
			info.w_speed = packet->getByte();
			player->setSpecialSkill(info);
			SkillsPacket::showSpecialSkill(player, info);
			break;
		}
		case 4211001: { // Chakra, unknown heal formula
			short dex = player->getDex();
			short luk = player->getLuk();
			short recovery = Skills::skills[4211001][player->getSkills()->getSkillLevel(4211001)].y;
			short minimum = (luk / 2) * ((1 + 3 / 10) + recovery / 100) + (dex * recovery / 100);
			short maximum = luk * ((1 + 3 / 10) + recovery / 100) + (dex * recovery / 100);
			short range = maximum - minimum;
			player->setHP(player->getHP() + (Randomizer::Instance()->randInt(range) + minimum));
			break;
		}
	}
}