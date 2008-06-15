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
#include "BufferUtilities.h"
#include "LoopingId.h"
#include "Randomizer.h"
#include "ReadPacket.h"

hash_map <int, MobInfo> Mobs::mobinfo;
hash_map <int, SpawnsInfo> Mobs::info;
hash_map <int, queue<int>> Mobs::respawns;

void Mob::setControl(Player *control) {
	if (this == 0) return;
	if (this->control != 0)
		MobsPacket::endControlMob(this->control, this);
	this->control = control;
	if (control != 0)
		MobsPacket::controlMob(control, this);
}

void Mobs::monsterControl(Player *player, ReadPacket *packet) {
	int mobid = packet->getInt();

	Mob *mob = Maps::maps[player->getMap()]->getMob(mobid);

	if (mob == 0 || mob->getControl() != player) {
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
	MobsPacket::moveMob(player, Maps::maps[player->getMap()]->getPlayers(), mobid, useskill, skill, packet->getBuffer(), packet->getBufferLength());
}

void Mobs::addMob(int id, MobInfo mob) {
	mobinfo[id] = mob;
}

void Mobs::addSpawn(int id, SpawnsInfo spawns) {
	info[id] = spawns;
	for (size_t i = 0; i < info[id].size(); i++) // Queue up all spawn points for initial spawning
		respawns[id].push(i);
}

void Mobs::checkSpawn(int mapid) {
	// (Re-)spawn Mobs
	while (!respawns[mapid].empty()) {
		int i = respawns[mapid].front();
		respawns[mapid].pop();
		Mob *mob = new Mob();
		mob->setMobID(info[mapid][i].id);
		mob->setSpawnID(i);
		mob->setPos(info[mapid][i].x, info[mapid][i].cy);
		mob->setHP(mobinfo[mob->getMobID()].hp);
		mob->setMP(mobinfo[mob->getMobID()].mp);
		mob->setFH(info[mapid][i].fh);
		mob->setType(2);
		Maps::maps[mapid]->addMob(mob);
		MobsPacket::spawnMob(Maps::maps[mapid]->getPlayers(), mob);
		updateSpawn(mapid, mob);
	}
}

void Mobs::showMobs(Player *player) {
	updateSpawn(player->getMap());
	hash_map <int, Mob *> mobs = Maps::maps[player->getMap()]->getMobs();
	for (hash_map <int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			MobsPacket::showMob(player, iter->second);
	}
}

void Mobs::updateSpawn(int mapid) {
	hash_map <int, Mob *> mobs = Maps::maps[mapid]->getMobs();
	for (hash_map <int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			updateSpawn(mapid, iter->second);
	}
}

void Mobs::updateSpawn(int mapid, Mob *mob) {
	if (Maps::maps[mapid]->getPlayers().size() > 0 && mob->getControl() == 0) {
		int maxpos = mob->getPos() - Maps::maps[mapid]->getPlayers()[0]->getPos();
		int player = 0;
		for (unsigned int j = 0; j < Maps::maps[mapid]->getPlayers().size(); j++) {
			int curpos = mob->getPos() - Maps::maps[mapid]->getPlayers()[j]->getPos();
			if (curpos < maxpos) {
				maxpos = curpos;
				player = j;
				break;
			}
			mob->setControl(Maps::maps[mapid]->getPlayers()[player]);
		}
	}
	else if (Maps::maps[mapid]->getPlayers().size() == 0) {
		mob->setControl(0);
	}
}

void Mobs::dieMob(Player *player, Mob *mob) {
	if (mob == 0) return;
	MobsPacket::dieMob(player, Maps::maps[player->getMap()]->getPlayers(), mob, mob->getID());

	// Account for Holy Symbol
	int hsrate = 0;
	if (player->skills->getActiveSkillLevel(2311003)>0) {
		hsrate = Skills::skills[2311003][player->skills->getActiveSkillLevel(2311003)].x;
	}
	else if (player->skills->getActiveSkillLevel(5101002) > 0) {
		hsrate = Skills::skills[5101002][player->skills->getActiveSkillLevel(5101002)].x;
	}

	Levels::giveEXP(player, (mobinfo[mob->getMobID()].exp + ((mobinfo[mob->getMobID()].exp*hsrate)/100)) * ChannelServer::Instance()->getExprate());
	Drops::dropMob(player, mob);

	// Spawn mob(s) the mob is supposed to spawn when it dies
	for (unsigned int i = 0; i < mobinfo[mob->getMobID()].summon.size(); i++) {
		spawnMobPos(player->getMap(), mobinfo[mob->getMobID()].summon[i], mob->getPosX(), mob->getPosY()-1);
	}

	if (mob->getSpawnID() > -1) // Add spawn point to respawns queue if mob was spawned by a spawn point.
		respawns[player->getMap()].push(mob->getSpawnID());

	player->quests->updateQuestMob(mob->getMobID());
	Maps::maps[player->getMap()]->removeMob(mob);
	delete mob;
}

void Mobs::damageMobSpell(Player *player, ReadPacket *packet) {
	int map = player->getMap();
	MobsPacket::damageMobSpell(player, Maps::maps[map]->getPlayers(), packet);
	packet->reset(2);
	packet->skipBytes(1);
	unsigned char tbyte = packet->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	int skillid = packet->getInt();
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		packet->skipBytes(4);
	int mpeater = 0;
	int mpeater_lv = 0;
	short mpeater_success;
	short mpeater_x;
	bool mpeated = false;
	if (player->getJob()/100 == 2) {
		mpeater = (player->getJob() / 10) * 100000;
		mpeater_lv = player->skills->getSkillLevel(mpeater);
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
				SkillsPacket::showMPEater(player, Maps::maps[map]->getPlayers(), mpeater);
			}
			MobHPInfo hpinfo;
			hpinfo.hp = mob->getHP();
			hpinfo.mhp = mobinfo[mobid].hp;
			hpinfo.boss = mobinfo[mobid].boss;
			hpinfo.hpcolor = mobinfo[mobid].hpcolor;
			hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
			hpinfo.mapmobid = mapmobid;
			hpinfo.mobid = mobid;
			displayHPBars(player, Maps::maps[map]->getPlayers(), hpinfo);
			if (mob->getHP() <= 0) {
				packet->skipBytes(4*(hits-1-k));
				dieMob(player, mob);
				break;
			}
		}
	}
}

void Mobs::damageMob(Player *player, ReadPacket *packet) {
	int map = player->getMap();
	MobsPacket::damageMob(player, Maps::maps[map]->getPlayers(), packet);
	packet->reset(2);
	packet->skipBytes(1); // Useless
	unsigned char tbyte = packet->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	int skillid = packet->getInt();
	// if (skillid == 1221011) {
	// Heaven's Hammer will require tons of special code, it only sends 0x01 as the damage for any hit
	// }
	packet->skipBytes(8); // In order: Display [1], Animation [1], Weapon subclass [1], Weapon speed [1], Tick count [4]
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
			totaldmg = totaldmg + damage;
			mob->setHP(mob->getHP() - damage);
			MobHPInfo hpinfo;
			hpinfo.hp = mob->getHP();
			hpinfo.mhp = mobinfo[mobid].hp;
			hpinfo.boss = mobinfo[mobid].boss;
			hpinfo.hpcolor = mobinfo[mobid].hpcolor;
			hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
			hpinfo.mapmobid = mapmobid;
			hpinfo.mobid = mobid;
			displayHPBars(player, Maps::maps[map]->getPlayers(), hpinfo);
			if (mob->getHP() <= 0) {
				packet->skipBytes(4*(hits-1-k));
				dieMob(player, mob);
				break;
			}
		}
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
					DropsPacket::explodeDrop(Maps::maps[map]->getPlayers(), drop);
					Maps::maps[map]->removeDrop(drop);
					delete drop;
				}
			}			
			break;
		}
		case 1111003: // Crusader finishers
		case 1111004:
		case 1111005:
		case 1111006:
			Skills::clearCombo(player);
			break;
		case 1111008: // Shout
		case 1311006: // Dragon Roar
		case 5001001: break; // Super Dragon Roar
		default:
			if (totaldmg > 0)
				Skills::addCombo(player);
			break;
	}
}

void Mobs::damageMobRanged(Player *player, ReadPacket *packet) {
	int map = player->getMap();
	MobsPacket::damageMobRanged(player, Maps::maps[map]->getPlayers(), packet);
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
	if (skillid == 3121004 || skillid == 3221001) {
		packet->skipBytes(4); // Charge time
		display = packet->getByte();
		if (skillid == 3121004 && player->getSpecialSkill() == 0) { // Only Hurricane constantly does damage and display it if not displayed
			SpecialSkillInfo info;
			info.skillid = skillid;
			info.direction = packet->getByte();
			packet->skipBytes(1); // Weapon subclass
			info.w_speed = packet->getByte();
			info.level = player->skills->getSkillLevel(info.skillid);
			player->setSpecialSkill(info);
			SkillsPacket::showSpecialSkill(player, Maps::maps[map]->getPlayers(), info);
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
	if (!(display == 0x40 || display == 0x48)) { // Shadow Claw doesn't take stars
		if (skillid == 4111005) // Avenger
			Inventory::takeItemSlot(player, pos, 2, 3*hits);
		else
			Inventory::takeItemSlot(player, pos, 2, hits);
	}
	else
		packet->skipBytes(4); // Star ID added by Shadow Claw
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
			MobHPInfo hpinfo;
			hpinfo.hp = mob->getHP();
			mhp = hpinfo.mhp = mobinfo[mobid].hp;
			hpinfo.boss = mobinfo[mobid].boss;
			hpinfo.hpcolor = mobinfo[mobid].hpcolor;
			hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
			hpinfo.mapmobid = mapmobid;
			hpinfo.mobid = mobid;
			displayHPBars(player, Maps::maps[map]->getPlayers(), hpinfo);
			if (mob->getHP() <= 0) {
				packet->skipBytes(4*(hits-1-k));
				dieMob(player, mob);
				break;
			}
		}
	}
	// packet->skipBytes(4); // Character positioning, end of packet, might eventually be useful for hacking detection
	if (skillid == 4101005) { // Drain
		int hpRecover = ((totaldmg * Skills::skills[4101005][player->skills->getSkillLevel(4101005)].x)/100);
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

void Mobs::damageMobPG(Player *player, int damage, Mob *mob) {
	if (mob == 0) return;

	int map = player->getMap();
	int mobid = mob->getMobID();
	int mapmobid = mob->getID();
	mob->setHP(mob->getHP() - damage);
	MobHPInfo hpinfo;
	hpinfo.hp = mob->getHP();
	hpinfo.mhp = mobinfo[mobid].hp;
	hpinfo.boss = mobinfo[mobid].boss;
	hpinfo.hpcolor = mobinfo[mobid].hpcolor;
	hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
	hpinfo.mapmobid = mapmobid;
	hpinfo.mobid = mobid;
	displayHPBars(player, Maps::maps[map]->getPlayers(), hpinfo);
	if (mob->getHP() <= 0)
		dieMob(player, mob);
}

void Mobs::spawnMob(Player *player, int mobid, int amount) {
	for (int i = 0; i < amount; i++)
		spawnMobPos(player->getMap(), mobid, player->getPos().x, player->getPos().y);
}

void Mobs::spawnMobPos(int mapid, int mobid, int xx, int yy) {
	Mob *mob = new Mob();
	mob->setMobID(mobid);
	mob->setSpawnID(-1);
	mob->setHP(mobinfo[mobid].hp);
	mob->setMP(mobinfo[mobid].mp);
	mob->setPos(xx, yy);
	mob->setFH(0);
	mob->setType(2);
	Maps::maps[mapid]->addMob(mob);
	MobsPacket::spawnMob(Maps::maps[mapid]->getPlayers(), mob);
	updateSpawn(mapid, mob);
}

void Mobs::displayHPBars(Player *player, vector <Player*> players, const MobHPInfo &mob) {
	if (mob.boss && mob.hpcolor > 0) // Boss HP bars
		MobsPacket::showBossHP(player, players, mob);
	else if (mob.boss) // Miniboss HP bars
		MobsPacket::showMinibossHP(player, players, mob.mobid, mob.hp * 100 / mob.mhp);
	else // Normal HP bars
		MobsPacket::showHP(player, mob.mapmobid, mob.hp * 100 / mob.mhp);
}
