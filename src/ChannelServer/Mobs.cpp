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
#include "Randomizer.h"
#include "ReadPacket.h"

hash_map <int, MobInfo> Mobs::mobinfo;

/* Mob class */
Mob::Mob(int mapid, int mobid, Pos pos, int spawnid, short fh) : mapid(mapid), id(id), mobid(mobid), spawnid(spawnid), pos(pos), type(2), fh(fh), control(0) {
	this->hp = Mobs::mobinfo[mobid].hp;
	this->mp = Mobs::mobinfo[mobid].mp;
	Maps::maps[mapid]->addMob(this);
}

void Mob::setControl(Player *control) {
	if (this->control != 0)
		MobsPacket::endControlMob(this->control, this);
	this->control = control;
	if (control != 0)
		MobsPacket::spawnMob(control, this, true, false);
}

void Mob::die(Player *player) {
	MobsPacket::dieMob(this);

	// Account for Holy Symbol
	int hsrate = 0;
	if (player->getSkills()->getActiveSkillLevel(2311003) > 0)
		hsrate = Skills::skills[2311003][player->getSkills()->getActiveSkillLevel(2311003)].x;
	else if (player->getSkills()->getActiveSkillLevel(9101002) > 0)
		hsrate = Skills::skills[9101002][player->getSkills()->getActiveSkillLevel(9101002)].x;

	Levels::giveEXP(player, (Mobs::mobinfo[mobid].exp + ((Mobs::mobinfo[mobid].exp * hsrate)/100)) * ChannelServer::Instance()->getExprate());
	Drops::dropMob(player, this);

	// Spawn mob(s) the mob is supposed to spawn when it dies
	for (size_t i = 0; i < Mobs::mobinfo[mobid].summon.size(); i++)
		Mobs::spawnMobPos(mapid, Mobs::mobinfo[mobid].summon[i], pos);

	player->getQuests()->updateQuestMob(mobid);
	Maps::maps[mapid]->removeMob(id, spawnid);
	delete this;
}

/* Mobs namespace */
void Mobs::monsterControl(Player *player, ReadPacket *packet) {
	int mobid = packet->getInt();

	Mob *mob = Maps::maps[player->getMap()]->getMob(mobid);

	if (mob == 0) {
		return;
	}

	short moveid = packet->getShort();
	bool useskill = (packet->getByte() != 0);
	int skill = packet->getInt();

	packet->reset(-12);
	char type = packet->getByte();

	packet->reset(-4);

	Pos cpos;
	cpos.x = packet->getShort();
	cpos.y = packet->getShort();
	if (cpos - mob->getPos() > 300) {
		if (player->addWarning()) return;
	}
	mob->setPos(cpos);
	mob->setType(type);

	MobsPacket::moveMobResponse(player, mobid, moveid, useskill, mob->getMP());
	packet->reset(19);
	MobsPacket::moveMob(player, mobid, useskill, skill, packet->getBuffer(), packet->getBufferLength());
}

void Mobs::addMob(int id, MobInfo mob) {
	mobinfo[id] = mob;
}

void Mobs::damageMob(Player *player, ReadPacket *packet) {
	int map = player->getMap();
	MobsPacket::damageMob(player, packet);
	packet->reset(2);
	packet->skipBytes(1); // Useless
	unsigned char tbyte = packet->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	int skillid = packet->getInt();
	bool heavenHammer = false;
	if (skillid == 1221011)
		heavenHammer = true;	
	packet->skipBytes(8); // In order: Display [1], Animation [1], Weapon subclass [1], Weapon speed [1], Tick count [4]
	if (skillid == 5201002)
		packet->skipBytes(4); // Charge 
	unsigned int totaldmg = 0;
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for (char i = 0; i < targets; i++) {
		int mapmobid = packet->getInt();
		Mob *mob = Maps::maps[map]->getMob(mapmobid);
		if (mob == 0)
			return;
		packet->skipBytes(12); // 0x06 [1], Mob animation [1], Mob animation frame [1], State [1], Mob pos [4], Damage pos [4]
		if (skillid == 4211006)
			packet->skipBytes(1); // Meso Explosion is weird
		else
			packet->skipBytes(2); // Distance
		int mobid = mob->getMobID();		
		for (char k = 0; k < hits; k++) {
			int damage = packet->getInt();
			if (heavenHammer && Mobs::mobinfo[mob->getMobID()].boss) {
				// Damage calculation goes in here, it's skill % * range
				// Can't calculate range without weapon class
				// Thus, can't calculate damage because there's no indication of weapon class anywhere
				// Yes, I know "weapon subclass" is in the packet, but that doesn't segregate them
				// 1H anything except wand/staff = 1. This could be a dagger, it could be a sword, it could be a mace, etc.
			}
			else {
				if (heavenHammer)
					mob->setHP(1);
				else
					mob->setHP(mob->getHP() - damage);
			}
			totaldmg = totaldmg + damage;
			displayHPBars(player, mob);
			if (mob->getHP() <= 0) {
				packet->skipBytes(4 * (hits - 1 - k));
				mob->die(player);
				break;
			}
		}
		packet->skipBytes(4); // 4 bytes of unknown purpose, new in .56
	}
	packet->skipBytes(4); // Character positioning, normal end of packet
	switch (skillid) {
		case 4211006: { // Meso Explosion
			unsigned char items = packet->getByte();
			for (unsigned char i = 0; i < items; i++) {
				int objID = packet->getInt();
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
			player->getSkills()->setCombo(0, true);
			break;
		case 1111008: // Shout
		case 9001001: // Super Dragon Roar
			break; 
		case 1311006: { // Dragon Roar
			char roarlv = player->getSkills()->getSkillLevel(skillid);
			short x_value = Skills::skills[skillid][roarlv].x;
			short y_value = Skills::skills[skillid][roarlv].y; // Stun length in seconds
			short hp = player->getHP();
			short reduction = (player->getMHP() / 100) * x_value;
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
			short hp_damage_x = Skills::skills[skillid][player->getSkills()->getSkillLevel(skillid)].x;
			short hp_damage = totaldmg * hp_damage_x / 100;
			short hp = player->getHP();
			if (hp - hp_damage < 1)
				player->setHP(1);
			else
				player->setHP(hp - hp_damage);
			break;
		}
		case 1211002: { // Charged Blow
			char acb_level = player->getSkills()->getSkillLevel(1220010);
			short acb_x = 0;
			if (acb_level > 0)
				acb_x = Skills::skills[1220010][acb_level].x;
			int charge_id = 0;
			if (player->getSkills()->getActiveSkillLevel(1211003) > 0) // Fire - Sword
				charge_id = 1211003;
			else if (player->getSkills()->getActiveSkillLevel(1211004) > 0) // Fire - BW
				charge_id = 1211004;
			else if (player->getSkills()->getActiveSkillLevel(1211005) > 0) // Ice - Sword
				charge_id = 1211005;
			else if (player->getSkills()->getActiveSkillLevel(1211006) > 0) // Ice - BW
				charge_id = 1211006;
			else if (player->getSkills()->getActiveSkillLevel(1211007) > 0) // Lightning - Sword
				charge_id = 1211007;
			else if (player->getSkills()->getActiveSkillLevel(1211008) > 0) // Lightning - BW
				charge_id = 1211008;
			else if (player->getSkills()->getActiveSkillLevel(1221003) > 0) // Holy - Sword
				charge_id = 1221003;
			else if (player->getSkills()->getActiveSkillLevel(1221004) > 0) // Holy - BW
				charge_id = 1221004;
			if (charge_id == 0) {
				// Hacking
				return;
			}
			if ((acb_x != 100) && (acb_x == 0 || Randomizer::Instance()->randInt(99) > (acb_x - 1))) 
				Skills::endSkill(player, charge_id);
			break;
		}
		default:
			if (totaldmg > 0)
				player->getSkills()->addCombo();
			break;
	}
}

void Mobs::damageMobRanged(Player *player, ReadPacket *packet) {
	int map = player->getMap();
	MobsPacket::damageMobRanged(player, packet);
	packet->reset(2); // Passing to the display function causes the buffer to be eaten, we need it
	packet->skipBytes(1); // Number of portals taken (not kidding)
	char targets = 0;
	char hits = 0;
	if (1) { // Don't need this variable for more than a couple operations
		unsigned char tbyte = packet->getByte();
		targets = tbyte / 0x10;
		hits = tbyte % 0x10;
	}
	int skillid = packet->getInt();
	unsigned char display = 0;
	bool shadow_meso = false;
	if (skillid == 4111004)
		shadow_meso = true;
	if (skillid == 3121004 || skillid == 3221001) {
		packet->skipBytes(4); // Charge time
		display = packet->getByte();
		if (skillid == 3121004 && player->getSpecialSkill() == 0) { // Only Hurricane constantly does damage and display it if not displayed
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
	}
	else {
		display = packet->getByte(); // Projectile display
		packet->skipBytes(1); // Direction/animation
		packet->skipBytes(1); // Weapon subclass
		packet->skipBytes(1); // Weapon speed
	}
	packet->skipBytes(4); // Ticks
	short pos = packet->getShort();
	packet->skipBytes(2); // Cash Shop star cover
	packet->skipBytes(1); // 0x00 = AoE, 0x41 = other
	if (!shadow_meso) {
		if (!(display == 0x40 || display == 0x48)) { // Shadow Claw doesn't take stars
			if (skillid == 4111005) // Avenger
				Inventory::takeItemSlot(player, 2, pos, 3*hits);
			else
				Inventory::takeItemSlot(player, 2, pos, hits);
		}
		else
			packet->skipBytes(4); // Star ID added by Shadow Claw
	}
	else { // This will be moved to useAttackSkill when the moneyCon property is added
			char shadow_level = player->getSkills()->getSkillLevel(skillid);
			short midpoint = 120 + (shadow_level * 15); // Midpoint is given in moneyCon property (doesn't exist yet)
			short mesos_min = midpoint - (80 + shadow_level * 5);
			short mesos_max = midpoint + (80 + shadow_level * 5);
			short difference = mesos_max - mesos_min; // Randomize up to this, add minimum for range
			short amount = Randomizer::Instance()->randInt(difference) + mesos_min;
			int mesos = player->getInventory()->getMesos();
			if (mesos - amount > -1) 
				player->getInventory()->setMesos(mesos - amount);
			else {
				// Hacking
				return;
			}
	}
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	int damage, mhp;
	unsigned int totaldmg = 0;
	for (char i = 0; i < targets; i++) {
		int mapmobid = packet->getInt();
		packet->skipBytes(1); // Always 0x06
		packet->skipBytes(2); // Animation garbage, don't need for damage
		packet->skipBytes(1); // State, this changes depending on enemy buffs (super wdef up, etc.), might need later
		packet->skipBytes(8); // Positioning - first 4 = mob, second 4 = damage
		packet->skipBytes(2); // Distance, might be helpful for catching hacking
		Mob *mob = Maps::maps[map]->getMob(mapmobid);
		if (mob == 0)
			return;
		int mobid = mob->getMobID();
		for (char k = 0; k < hits; k++) {
			damage = packet->getInt();
			totaldmg = totaldmg + damage;
			mob->setHP(mob->getHP() - damage);
			mhp = mobinfo[mob->getMobID()].hp;
			displayHPBars(player, mob);
			if (mob->getHP() <= 0) {
				packet->skipBytes(4 * (hits - 1 - k));
				mob->die(player);
				break;
			}
		}
		packet->skipBytes(4); // 4 bytes of unknown purpose, new in .56
	}
	// packet->skipBytes(4); // Character positioning, end of packet, might eventually be useful for hacking detection
	if (skillid == 4101005) { // Drain
		int hpRecover = ((totaldmg * Skills::skills[4101005][player->getSkills()->getSkillLevel(4101005)].x)/100);
		if (hpRecover > mhp)
			hpRecover = mhp;
		if (hpRecover > player->getMHP()/2)
			hpRecover = player->getMHP()/2;
		if (player->getHP()+hpRecover > player->getMHP())
			player->setHP(player->getMHP());
		else
			player->setHP(player->getHP()+hpRecover);
	}
}

void Mobs::damageMobSpell(Player *player, ReadPacket *packet) {
	int map = player->getMap();
	MobsPacket::damageMobSpell(player, packet);
	packet->reset(2);
	packet->skipBytes(1);
	unsigned char tbyte = packet->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	int skillid = packet->getInt();
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		packet->skipBytes(4);
	int mpeater = 0;
	short mpeater_success;
	short mpeater_x;
	char mpeater_lv = 0;
	bool mpeated = false;
	if (player->getJob()/100 == 2) {
		mpeater = (player->getJob() / 10) * 100000;
		mpeater_lv = player->getSkills()->getSkillLevel(mpeater);
		mpeater_success = Skills::skills[mpeater][mpeater_lv].prop;
		mpeater_x = Skills::skills[mpeater][mpeater_lv].x;
	}
	packet->skipBytes(2); // Display, direction/animation
	packet->skipBytes(2); // Weapon subclass, casting speed
	packet->skipBytes(4); // Ticks
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for (char i = 0; i < targets; i++) {
		int mapmobid = packet->getInt();
		Mob *mob = Maps::maps[map]->getMob(mapmobid);
		if (mob == 0)
			return;
		int mobid = mob->getMobID();
		packet->skipBytes(3); // Useless
		packet->skipBytes(1); // State
		packet->skipBytes(8); // Useless
		packet->skipBytes(2); // Distance
		for (char k = 0; k < hits; k++) {
			int damage = packet->getInt();
			mob->setHP(mob->getHP() - damage);
			int cmp = -1;
			cmp = mob->getMP();
			int mmp = -1;
			mmp = mobinfo[mob->getMobID()].mp;
			if ((mpeater_lv > 0) && (!mpeated) && (damage != 0) && (cmp > 0) && (Randomizer::Instance()->randInt(99) < mpeater_success)) {
				// MP Eater
				mpeated = true;
				short mp = mmp * mpeater_x / 100;
				if (mp > cmp) mp = cmp;
				mob->setMP(cmp - mp);
				player->setMP(player->getMP() + mp);
				SkillsPacket::showSkillEffect(player, mpeater);
			}
			displayHPBars(player, mob);
			if (mob->getHP() <= 0) {
				packet->skipBytes(4 * (hits - 1 - k));
				mob->die(player);
				break;
			}
		}
		packet->skipBytes(4); // 4 bytes of unknown purpose, new in .56
	}
}

void Mobs::spawnMob(Player *player, int mobid, int amount) {
	for (int i = 0; i < amount; i++)
		spawnMobPos(player->getMap(), mobid, player->getPos());
}

void Mobs::spawnMobPos(int mapid, int mobid, Pos pos) {
	new Mob(mapid, mobid, pos);
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

	if (hpinfo.boss && hpinfo.hpcolor > 0) // Boss HP bars
		MobsPacket::showBossHP(player, hpinfo);
	else if (hpinfo.boss) // Miniboss HP bars
		MobsPacket::showMinibossHP(player, hpinfo.mobid, hpinfo.hp * 100 / hpinfo.mhp);
	else // Normal HP bars
		MobsPacket::showHP(player, hpinfo.mapmobid, hpinfo.hp * 100 / hpinfo.mhp);
}
