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

hash_map<int, MobInfo> Mobs::mobinfo;
hash_map<int, SpawnsInfo> Mobs::info;
hash_map<int, queue<int>> Mobs::respawns;
hash_map<int, hash_map<int, Mob *>> Mobs::mobs;
hash_map<int, LoopingId *> Mobs::loopingIds;

void Mob::setControl(Player* control) {
	if (this == 0) return;
	if (this->control != NULL)
		MobsPacket::endControlMob(this->control, this);
	this->control = control;
	if (control != NULL)
		MobsPacket::controlMob(control, this);
}

void Mobs::monsterControl(Player *player, ReadPacket *packet) {
	int mobid = packet->getInt();

	Mob *mob = getMob(mobid, player->getMap());

	if (mob == NULL || mob->getControl() != player) {
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
	MobsPacket::moveMob(player, Maps::info[player->getMap()].Players, mobid, useskill, skill, packet->getBuffer(), packet->getBufferLength());
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
		Mob *mob = new Mob();
		int id = nextMobId(mapid);
		mob->setPos(info[mapid][i].x, info[mapid][i].cy);
		mob->setID(id);
		mob->setMobID(info[mapid][i].id);
		mob->setMapID(i);
		mob->setHP(mobinfo[info[mapid][i].id].hp);
		mob->setMP(mobinfo[info[mapid][i].id].mp);
		mob->setFH(info[mapid][i].fh);
		mob->setType(2);
		mobs[mapid][id] = mob;
		MobsPacket::spawnMob(Maps::info[mapid].Players, mob);
		updateSpawn(mapid, mob);
		respawns[mapid].pop();
	}
}

void Mobs::showMobs(Player *player) {
	updateSpawn(player->getMap());
	for (hash_map<int, Mob *>::iterator iter = Mobs::mobs[player->getMap()].begin(); iter != Mobs::mobs[player->getMap()].end(); iter++) {
		if (iter->second != NULL)
			MobsPacket::showMob(player, iter->second);
	}
}

void Mobs::updateSpawn(int mapid) {
	for (hash_map<int, Mob *>::iterator iter = Mobs::mobs[mapid].begin(); iter != Mobs::mobs[mapid].end(); iter++) {
		if (iter->second != NULL)
			updateSpawn(mapid, iter->second);
	}
}

void Mobs::updateSpawn(int mapid, Mob *mob) {
	if (Maps::info[mapid].Players.size() > 0 && mob->getControl() == 0) {
		int maxpos = mob->getPos() - Maps::info[mapid].Players[0]->getPos();
		int player = 0;
		for (unsigned int j = 0; j < Maps::info[mapid].Players.size(); j++) {
			int curpos = mob->getPos() - Maps::info[mapid].Players[j]->getPos();
			if (curpos < maxpos) {
				maxpos = curpos;
				player = j;
				break;
			}
			mob->setControl(Maps::info[mapid].Players[player]);
		}
	}
	else if (Maps::info[mapid].Players.size() == 0) {
		mob->setControl(NULL);
	}
}

void Mobs::dieMob(Player *player, Mob* mob) {
	if (mob == NULL) return;
	MobsPacket::dieMob(player, Maps::info[player->getMap()].Players, mob, mob->getID());

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

	if (mob->getMapID() > -1) // Add spawn point to respawns queue if mob was spawned by a spawn point.
		respawns[player->getMap()].push(mob->getMapID());

	player->quests->updateQuestMob(mob->getMobID());
	mobs[player->getMap()].erase(mob->getID());
	delete mob;
}

Mob * Mobs::getMob(int mobid, int map) {
	if (mobs[map].find(mobid) != mobs[map].end()) {
		return mobs[map][mobid];
	}
	return NULL;
}

void Mobs::damageMobSpell(Player *player, unsigned char* packet) {
	MobsPacket::damageMobSpell(player, Maps::info[player->getMap()].Players, packet);
	int howmany = packet[1]/0x10;
	int hits = packet[1]%0x10;
	int map = player->getMap();
	int skillid = BufferUtilities::getInt(packet+2);
	short offset = 0;
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		offset = 4;
	int mpeater = 0;
	int mpeater_lv = 0;
	int mpeater_success;
	int mpeater_x;
	bool mpeated = false;
	if (player->getJob()/100 == 2) {
		mpeater = (player->getJob() / 10) * 100000;
		mpeater_lv = player->skills->getSkillLevel(mpeater);
		mpeater_success = Skills::skills[mpeater][mpeater_lv].prop;
		mpeater_x = Skills::skills[mpeater][mpeater_lv].x;
	}

	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for (int i=0; i<howmany; i++) {
		int mapmobid = BufferUtilities::getInt(packet+14+offset+i*(22+4*(hits-1)));
		Mob* mob = getMob(mapmobid, map);
		int mobid = mob->getMobID();
		for (int k=0; k<hits; k++) {
			int damage = BufferUtilities::getInt(packet+32+offset+i*(22+4*(hits-1))+k*4);
			if (mob == NULL)
				return;
			mob->setHP(mob->getHP()-damage);
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
				SkillsPacket::showMPEater(player, Maps::info[map].Players, mpeater);
			}
			MobHPInfoStruct hpinfo; 
			hpinfo.hp = mob->getHP();
			hpinfo.mhp = mobinfo[mobid].hp;
			hpinfo.boss = mobinfo[mobid].boss;
			hpinfo.hpcolor = mobinfo[mobid].hpcolor;
			hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
			hpinfo.mapmobid = mapmobid;
			hpinfo.mobid = mobid;
			displayHPBars(player, Maps::info[map].Players, hpinfo);
			if (mob->getHP() <= 0) {
				dieMob(player, mob);
				break;
			}
		}
	}
}

void Mobs::damageMob(Player *player, unsigned char* packet) {
	MobsPacket::damageMob(player, Maps::info[player->getMap()].Players, packet);
	int howmany = packet[1]/0x10;
	int hits = packet[1]%0x10;
	int map = player->getMap();
	int skillid = BufferUtilities::getInt(packet+2);
	short offset = 0;
	bool s4211006 = (skillid == 4211006);
	int totaldmg = 0;
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for (int i=0; i<howmany; i++) {
		int mapmobid = BufferUtilities::getInt(packet+14+offset+i*(22-s4211006+4*(hits-1)));
		Mob* mob = getMob(mapmobid, map);
		int mobid = mob->getMobID();
		for (int k=0; k<hits; k++) {
			int damage = BufferUtilities::getInt(packet+32+offset-s4211006+i*(22-s4211006+4*(hits-1))+k*4);
			totaldmg = totaldmg + damage;
			if (mob == NULL)
				return;
			if (mob->getPos() - player->getPos() > 300 && skillid == 0) {
				if (player->addWarning()) return;
			}
			mob->setHP(mob->getHP() - damage);
			MobHPInfoStruct hpinfo; 
			hpinfo.hp = mob->getHP();
			hpinfo.mhp = mobinfo[mobid].hp;
			hpinfo.boss = mobinfo[mobid].boss;
			hpinfo.hpcolor = mobinfo[mobid].hpcolor;
			hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
			hpinfo.mapmobid = mapmobid;
			hpinfo.mobid = mobid;
			displayHPBars(player, Maps::info[map].Players, hpinfo);
			if (mob->getHP() <= 0) {
				dieMob(player, mob);
				break;
			}
		}
	}
	if (s4211006) {
		unsigned char howmanyitems = packet[(14+howmany*(21+4*(hits-1)))+4];
		for (int i=0; i<howmanyitems; i++) {
			int objID = BufferUtilities::getInt(packet+(14+howmany*(21+4*(hits-1)))+5+(5*i));
			for (unsigned int i=0; i<Drops::drops[map].size(); i++) {
				if (Drops::drops[map][i]->getObjID() == objID) {
					DropsPacket::explodeDrop(Maps::info[map].Players, Drops::drops[map][i]);
					Drops::drops[map].erase(Drops::drops[map].begin()+i);
					break;
				}
			}
		}
	}
	if (skillid == 1111003 || skillid == 1111004 || skillid == 1111005 || skillid == 1111006) { // Combo finishing moves
		Skills::clearCombo(player);
	}
	else if (totaldmg > 0) { // Only add orbs for attacks that did damage
		Skills::addCombo(player, 1);
	}
}
void Mobs::damageMobRanged(Player *player, unsigned char* packet, int size) {
	int itemid = 0;
	int pos = BufferUtilities::getInt(packet+14);
	bool s4121006 = (packet[6] == 0x40 || packet[6] == 0x48); // Shadow Claw
	if (!s4121006) {
		for (int i = 0; i < player->inv->getItemNum(); i++) {
			if (player->inv->getItem(i)->pos == pos && player->inv->getItem(i)->inv == 2) {
				itemid = player->inv->getItem(i)->id;
				break;
			}
		}
	}
	else {
		itemid = BufferUtilities::getInt(packet+20);
	}
	int map = player->getMap();
	MobsPacket::damageMobRanged(player, Maps::info[map].Players, packet, itemid);

	int howmany = packet[1]/0x10;
	int hits = packet[1]%0x10;
	int skillid = BufferUtilities::getInt(packet+2);
	short offset = 0;
	if (skillid == 3121004 || skillid == 3221001) {
		offset += 4; // Hurricane/Pierce add 4 bytes after skill ID, value represents charge time (0 for Hurricane)
		if (skillid == 3121004) { // Only Hurricane constantly does damage
			if (player->getSpecialSkill() == 0) { // Display it if not displayed
				SpecialSkillInfo info;
				info.skillid = skillid;
				info.direction = packet[7+offset];
				info.w_speed = packet[9+offset];
				info.level = player->skills->getSkillLevel(info.skillid);
				player->setSpecialSkill(info);
				SkillsPacket::showSpecialSkill(player, Maps::info[map].Players, info);
			}	
		}
	}
	if (s4121006)
		offset += 4; // Shadow Claw's offset only matters for damage, adds a 4 byte star ID at the end of the normal packet before damage
	else {
		if (skillid == 4111005) // Avenger
			Inventory::takeItemSlot(player, pos, 2, 3*hits);
		else
			Inventory::takeItemSlot(player, pos, 2, hits);
	}
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	int damage, mhp;
	int totaldmg = 0;
	for (int i = 0; i < howmany; i++) {
		int mapmobid = BufferUtilities::getInt(packet+19+offset+i*(22+4*(hits-1)));
		Mob* mob = getMob(mapmobid, map);
		int mobid = mob->getMobID();
		for (int k = 0; k < hits; k++) {
			damage = BufferUtilities::getInt(packet+37+offset+i*(22+4*(hits-1))+k*4);
			totaldmg = totaldmg + damage;
			if (mob == NULL)
				return;
			mob->setHP(mob->getHP() - damage);
			MobHPInfoStruct hpinfo; 
			hpinfo.hp = mob->getHP();
			mhp = hpinfo.mhp = mobinfo[mobid].hp;
			hpinfo.boss = mobinfo[mobid].boss;
			hpinfo.hpcolor = mobinfo[mobid].hpcolor;
			hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
			hpinfo.mapmobid = mapmobid;
			hpinfo.mobid = mobid;
			displayHPBars(player, Maps::info[map].Players, hpinfo);
			if (mob->getHP() <= 0) {
				dieMob(player, mob);
				break;
			}
		}
	}	
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
	int map = player->getMap();
	int mobid = mob->getMobID();
	int mapmobid = mob->getID();
	if (mob == NULL)
		return;
	mob->setHP(mob->getHP() - damage);
	MobHPInfoStruct hpinfo;
	hpinfo.hp = mob->getHP();
	hpinfo.mhp = mobinfo[mobid].hp;
	hpinfo.boss = mobinfo[mobid].boss;
	hpinfo.hpcolor = mobinfo[mobid].hpcolor;
	hpinfo.hpbgcolor = mobinfo[mobid].hpbgcolor;
	hpinfo.mapmobid = mapmobid;
	hpinfo.mobid = mobid;
	displayHPBars(player, Maps::info[map].Players, hpinfo);
	if (mob->getHP() <= 0)
		dieMob(player, mob);
}

void Mobs::spawnMob(Player *player, int mobid, int amount) {
	for (int i = 0; i < amount; i++)
		spawnMobPos(player->getMap(), mobid, player->getPos().x, player->getPos().y);
}

void Mobs::spawnMobPos(int mapid, int mobid, int xx, int yy) {
	Mob *mob = new Mob();
	int id = nextMobId(mapid);
	mob->setPos(xx, yy);
	mob->setID(id);
	mob->setMobID(mobid);
	mob->setMapID(-1);
	mob->setHP(mobinfo[mobid].hp);
	mob->setMP(mobinfo[mobid].mp);
	mob->setFH(0);
	mob->setType(2);
	mobs[mapid][id] = mob;
	updateSpawn(mapid, mob);
	MobsPacket::spawnMob(Maps::info[mapid].Players, mob);
}

inline int Mobs::nextMobId(int mapid) {
	if (loopingIds.find(mapid) == loopingIds.end()) {
		loopingIds[mapid] = new LoopingId(100);
	}
	return loopingIds[mapid]->next();
}

void Mobs::displayHPBars(Player* player, vector <Player*> players, const MobHPInfoStruct &mob) {
	if (mob.boss && mob.hpcolor > 0) // Boss HP bars
		MobsPacket::showBossHP(player, players, mob);
	else if (mob.boss) // Miniboss HP bars
		MobsPacket::showMinibossHP(player, players, mob.mobid, mob.hp * 100 / mob.mhp);
	else // Normal HP bars
		MobsPacket::showHP(player, mob.mapmobid, mob.hp * 100 / mob.mhp);
}
