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
#include "Mobs.h"
#include "Maps.h"
#include "Player.h"
#include "MobsPacket.h"
#include "DropsPacket.h"
#include "SkillsPacket.h"
#include "Drops.h"
#include "Levels.h"
#include "Quests.h"
#include "Skills.h"
#include "Inventory.h"
#include "LoopingId.h"
#include "Movement.h"
#include "Randomizer.h"
#include "ReadPacket.h"

unordered_map<int32_t, MobInfo> Mobs::mobinfo;

/* Mob class */
Mob::Mob(int32_t id, int32_t mapid, int32_t mobid, Pos pos, int32_t spawnid, int16_t fh) :
MovableLife(fh, pos, 2),
id(id),
mapid(mapid),
spawnid(spawnid),
mobid(mobid),
control(0)
{
	this->hp = Mobs::mobinfo[mobid].hp;
	this->mp = Mobs::mobinfo[mobid].mp;
}

void Mob::setControl(Player *control) {
	/*if (this->control != 0)
		MobsPacket::endControlMob(this->control, this);*/
	this->control = control;
	if (control != 0)
		MobsPacket::spawnMob(control, this, true, false);
}

void Mob::die(Player *player) {
	MobsPacket::dieMob(this);

	// Account for Holy Symbol
	int16_t hsrate = 0;
	if (player->getActiveBuffs()->getActiveSkillLevel(2311003) > 0)
		hsrate = Skills::skills[2311003][player->getActiveBuffs()->getActiveSkillLevel(2311003)].x;
	else if (player->getActiveBuffs()->getActiveSkillLevel(9101002) > 0)
		hsrate = Skills::skills[9101002][player->getActiveBuffs()->getActiveSkillLevel(9101002)].x;

	Levels::giveEXP(player, (Mobs::mobinfo[mobid].exp + ((Mobs::mobinfo[mobid].exp * hsrate)/100)) * ChannelServer::Instance()->getExprate());
	Drops::doDrops(player, this->mobid, this->m_pos);

	// Spawn mob(s) the mob is supposed to spawn when it dies
	for (size_t i = 0; i < Mobs::mobinfo[mobid].summon.size(); i++)
		Mobs::spawnMobPos(mapid, Mobs::mobinfo[mobid].summon[i], m_pos);

	player->getQuests()->updateQuestMob(mobid);
	Maps::maps[mapid]->removeMob(id, spawnid);
	delete this;
}

void Mob::die() {
	Maps::maps[mapid]->removeMob(id, spawnid);
	delete this;
}

/* Mobs namespace */
void Mobs::monsterControl(Player *player, ReadPacket *packet) {
	int32_t mobid = packet->getInt();

	Mob *mob = Maps::maps[player->getMap()]->getMob(mobid);

	if (mob == 0) {
		return;
	}

	int16_t moveid = packet->getShort();
	bool useskill = (packet->getByte() != 0);
	int32_t skill = packet->getInt();
	packet->skipBytes(10);
	Pos cpos = Movement::parseMovement(mob, packet);
	if (cpos - mob->getPos() > 300) {
		if (player->addWarning())
			return;
	}
	MobsPacket::moveMobResponse(player, mobid, moveid, useskill, mob->getMP());
	packet->reset(19);
	MobsPacket::moveMob(player, mobid, useskill, skill, packet->getBuffer(), packet->getBufferLength());
}

void Mobs::addMob(int32_t id, MobInfo mob) {
	mobinfo[id] = mob;
}

void Mobs::damageMob(Player *player, ReadPacket *packet) {
	MobsPacket::damageMob(player, packet);
	packet->reset(2);
	packet->skipBytes(1); // Useless
	uint8_t tbyte = packet->getByte();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet->getInt();
	packet->skipBytes(8); // In order: Display [1], Animation [1], Weapon subclass [1], Weapon speed [1], Tick count [4]
	switch (skillid) {
		case 5201002:
			packet->skipBytes(4); // Charge 
			break;
	}
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	int32_t useless = 0;
	uint32_t totaldmg = damageMobInternal(player, packet, targets, hits, skillid, useless);	
	switch (skillid) {
		case 4211006: { // Meso Explosion
			uint8_t items = packet->getByte();
			int32_t map = player->getMap();
			for (uint8_t i = 0; i < items; i++) {
				int32_t objID = packet->getInt();
				packet->skipBytes(1); // Boolean for hit a monster
				Drop *drop = Maps::maps[map]->getDrop(objID);
				if (drop != 0) {
					DropsPacket::explodeDrop(drop);
					Maps::maps[map]->removeDrop(drop->getID());
					delete drop;
				}
			}			
			break;
		}
		case 1111003: // Crusader finishers
		case 1111004:
		case 1111005:
		case 1111006:
			player->getActiveBuffs()->setCombo(0, true);
			break;
		case 1111008: // Shout
		case 9001001: // Super Dragon Roar
			break; 
		case 1311006: { // Dragon Roar
			int8_t roarlv = player->getSkills()->getSkillLevel(skillid);
			int16_t x_value = Skills::skills[skillid][roarlv].x;
			int16_t y_value = Skills::skills[skillid][roarlv].y; // Stun length in seconds
			int16_t hp = player->getHP();
			int16_t reduction = (player->getMHP() / 100) * x_value;
			if (hp - reduction > 0)
				player->setHP(hp - reduction, false);
			else {
				// Hacking
				return;
			}
			// TODO: Add stun here
			break;
		}
		case 1311005: { // Sacrifice
			int16_t hp_damage_x = static_cast<int16_t>(Skills::skills[skillid][player->getSkills()->getSkillLevel(skillid)].x);
			uint16_t hp_damage = static_cast<uint16_t>(totaldmg * hp_damage_x / 100);
			uint16_t hp = player->getHP();
			if (hp - hp_damage < 1)
				player->setHP(1);
			else
				player->setHP(hp - hp_damage);
			break;
		}
		case 1211002: { // Charged Blow
			int8_t acb_level = player->getSkills()->getSkillLevel(1220010);
			int16_t acb_x = 0;
			if (acb_level > 0)
				acb_x = Skills::skills[1220010][acb_level].x;
			int32_t charge_id = 0;
			if (player->getActiveBuffs()->getActiveSkillLevel(1211003) > 0) // Fire - Sword
				charge_id = 1211003;
			else if (player->getActiveBuffs()->getActiveSkillLevel(1211004) > 0) // Fire - BW
				charge_id = 1211004;
			else if (player->getActiveBuffs()->getActiveSkillLevel(1211005) > 0) // Ice - Sword
				charge_id = 1211005;
			else if (player->getActiveBuffs()->getActiveSkillLevel(1211006) > 0) // Ice - BW
				charge_id = 1211006;
			else if (player->getActiveBuffs()->getActiveSkillLevel(1211007) > 0) // Lightning - Sword
				charge_id = 1211007;
			else if (player->getActiveBuffs()->getActiveSkillLevel(1211008) > 0) // Lightning - BW
				charge_id = 1211008;
			else if (player->getActiveBuffs()->getActiveSkillLevel(1221003) > 0) // Holy - Sword
				charge_id = 1221003;
			else if (player->getActiveBuffs()->getActiveSkillLevel(1221004) > 0) // Holy - BW
				charge_id = 1221004;
			if (charge_id == 0) {
				// Hacking
				return;
			}
			if ((acb_x != 100) && (acb_x == 0 || Randomizer::Instance()->randShort(99) > (acb_x - 1))) 
				Skills::stopSkill(player, charge_id);
			break;
		}
		default:
			if (totaldmg > 0)
				player->getActiveBuffs()->addCombo();
			break;
	}
}

void Mobs::damageMobRanged(Player *player, ReadPacket *packet) {
	MobsPacket::damageMobRanged(player, packet);
	packet->reset(2); // Passing to the display function causes the buffer to be eaten, we need it
	packet->skipBytes(1); // Number of portals taken (not kidding)
	uint8_t tbyte = packet->getByte();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet->getInt();
	uint8_t display = 0;
	switch (skillid) {
		case 3121004:
		case 3221001:
		case 5221004:
			packet->skipBytes(4); // Charge time
			display = packet->getByte();
			if ((skillid == 3121004 || skillid == 5221004) && player->getSpecialSkill() == 0) { // Only Hurricane constantly does damage and display it if not displayed
				SpecialSkillInfo info;
				info.skillid = skillid;
				info.direction = packet->getByte();
				packet->skipBytes(1); // Weapon subclass
				info.w_speed = packet->getByte();
				info.level = player->getSkills()->getSkillLevel(info.skillid);
				player->setSpecialSkill(info);
				SkillsPacket::showSpecialSkill(player, info);
			}
			else
				packet->skipBytes(3);
			break;
		default:
			display = packet->getByte(); // Projectile display
			packet->skipBytes(1); // Direction/animation
			packet->skipBytes(1); // Weapon subclass
			packet->skipBytes(1); // Weapon speed
			break;
	}
	packet->skipBytes(4); // Ticks
	int16_t pos = packet->getShort();
	packet->skipBytes(2); // Cash Shop star cover
	packet->skipBytes(1); // 0x00 = AoE, 0x41 = other
	if (skillid != 4111004 && ((display & 0x40) > 0))
		packet->skipBytes(4); // Star ID added by Shadow Claw
	if (skillid > 0)
		Skills::useAttackSkillRanged(player, skillid, pos, display);
	else
		Skills::useAttackRanged(player, pos, display);
	int32_t mhp;
	uint32_t totaldmg = damageMobInternal(player, packet, targets, hits, skillid, mhp);
	if (skillid == 4101005) { // Drain
		int32_t hpRecover = ((totaldmg * Skills::skills[4101005][player->getSkills()->getSkillLevel(4101005)].x) / 100);
		if (hpRecover > mhp)
			hpRecover = mhp;
		if (hpRecover > (player->getMHP() / 2))
			hpRecover = player->getMHP() / 2;
		if ((player->getHP() + hpRecover) > player->getMHP())
			player->setHP(player->getMHP());
		else
			player->setHP(player->getHP() + (int16_t) hpRecover);
	}
}

void Mobs::damageMobSpell(Player *player, ReadPacket *packet) {
	MobsPacket::damageMobSpell(player, packet);
	packet->reset(2);
	packet->skipBytes(1);
	uint8_t tbyte = packet->getByte();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet->getInt();
	switch (skillid) {
		case 2121001: // Big Bang has a 4 byte charge time after skillid
		case 2221001:
		case 2321001:
			packet->skipBytes(4);
			break;
	}
	MPEaterInfo *eater = &MPEaterInfo();
	eater->id = (player->getJob() / 10) * 100000;
	eater->level = player->getSkills()->getSkillLevel(eater->id);
	if (eater->level > 0) {
		eater->prop = Skills::skills[eater->id][eater->level].prop;
		eater->x = Skills::skills[eater->id][eater->level].x;
	}
	packet->skipBytes(2); // Display, direction/animation
	packet->skipBytes(2); // Weapon subclass, casting speed
	packet->skipBytes(4); // Ticks
	if (skillid != 2301002) // Heal is sent as both an attack and as a use skill
		// Prevent this from incurring cost since Heal is always a used skill but only an attack in certain circumstances
		Skills::useAttackSkill(player, skillid);
	int32_t useless = 0;
	uint32_t totaldmg = damageMobInternal(player, packet, targets, hits, skillid, useless, eater);
}

uint32_t Mobs::damageMobInternal(Player *player, ReadPacket *packet, int8_t targets, int8_t hits, int32_t skillid, int32_t &extra, MPEaterInfo *eater) {
	int32_t map = player->getMap();
	uint32_t total = 0;
	bool isHorntail = false;
	for (int8_t i = 0; i < targets; i++) {
		int32_t mapmobid = packet->getInt();
		Mob *mob = Maps::maps[map]->getMob(mapmobid);
		if (mob == 0)
			return 0;
		int32_t mobid = mob->getMobID();
		Mob *htabusetaker = 0;
		switch (mobid) {
			case 8810002:
			case 8810003:
			case 8810004:
			case 8810005:
			case 8810006:
			case 8810007:
			case 8810008:
			case 8810009:
				isHorntail = true;
				htabusetaker = Maps::maps[map]->getMob(8810018, false);
				break;
		}
		packet->skipBytes(3); // Useless
		packet->skipBytes(1); // State
		packet->skipBytes(8); // Useless
		if (skillid != 4211006)
			packet->skipBytes(1); // Distance, first half for non-Meso Explosion
		packet->skipBytes(1); // Distance, second half for non-Meso Explosion
		for (int8_t k = 0; k < hits; k++) {
			int32_t damage = packet->getInt();
			total += damage;
			if (skillid == 1221011 && Mobs::mobinfo[mob->getMobID()].boss) {
				// Damage calculation goes in here, I think? Hearing conflicted views.
			}
			else {
				if (skillid == 1221011)
					mob->setHP(1);
				else {
					int32_t temphp = mob->getHP();
					mob->setHP(temphp - damage);
					if (htabusetaker != 0) {
						if (temphp - damage <= 0) // Horntail will die before all of his parts otherwise
							damage = temphp; // Damage isn't used again from here on anyway
						htabusetaker->setHP(htabusetaker->getHP() - damage);
					}
				}
			}
			extra = mobinfo[mob->getMobID()].hp;
			if (eater != 0) { // MP Eater
				int32_t cmp = mob->getMP();
				if ((!eater->onlyonce) && (damage != 0) && (cmp > 0) && (Randomizer::Instance()->randInt(99) < eater->prop)) {
					eater->onlyonce = true;
					int32_t mp = mobinfo[mob->getMobID()].mp * eater->x / 100;
					if (mp > cmp)
						mp = cmp;
					mob->setMP(cmp - mp);
					player->setMP(player->getMP() + (int16_t) mp);
					SkillsPacket::showSkillEffect(player, eater->id);
				}
			}
			displayHPBars(player, (isHorntail ? htabusetaker : mob));
			if (mob->getHP() <= 0) {
				packet->skipBytes(4 * (hits - 1 - k));
				mob->die(player);
				if (htabusetaker != 0) {
					if (htabusetaker->getHP() <= 0) {
						for (int8_t q = 0; q < 8; q++) {
							Maps::maps[map]->killMobs(player, (8810010 + q)); // Dead Horntail's parts
						}
						htabusetaker->die(player);
					}
				}
				break;
			}
		}
		packet->skipBytes(4); // 4 bytes of unknown purpose, new in .56
	}
	packet->skipBytes(4); // Character positioning, end of packet, might eventually be useful for hacking detection
	return total;
}
void Mobs::spawnMob(Player *player, int32_t mobid, int32_t amount) {
	for (int32_t i = 0; i < amount; i++)
		spawnMobPos(player->getMap(), mobid, player->getPos());
}

void Mobs::spawnMobPos(int32_t mapid, int32_t mobid, Pos pos) {
	Maps::maps[mapid]->spawnMob(mobid, pos);
}

void Mobs::displayHPBars(Player *player, Mob *mob) {
	MobHPInfo hpinfo;
	hpinfo.mobid = mob->getMobID();
	hpinfo.hp = mob->getHP();
	hpinfo.mhp = mobinfo[hpinfo.mobid].hp;
	hpinfo.boss = mobinfo[hpinfo.mobid].boss;
	hpinfo.hpcolor = mobinfo[hpinfo.mobid].hpcolor;
	hpinfo.hpbgcolor = mobinfo[hpinfo.mobid].hpbgcolor;
	hpinfo.mapmobid = mob->getID();

	uint8_t percent = static_cast<uint8_t>(hpinfo.hp * 100 / hpinfo.mhp);

	if ((hpinfo.boss || hpinfo.mobid == 8810018) && hpinfo.hpcolor > 0) // Boss HP bars - Horntail's damage sponge isn't a boss in the data
		MobsPacket::showBossHP(player, hpinfo);
	else if (hpinfo.boss) // Miniboss HP bars
		MobsPacket::showMinibossHP(player, hpinfo.mobid, percent);
	else // Normal HP bars
		MobsPacket::showHP(player, hpinfo.mapmobid, percent);
}
