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
#include "Drops.h"
#include "Levels.h"
#include "Quests.h"
#include "Skills.h"
#include "Inventory.h"
#include "BufferUtilities.h"
#include "LoopingId.h"
hash_map<int, MobInfo> Mobs::mobinfo;
hash_map<int, SpawnsInfo> Mobs::info;
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

void Mobs::monsterControl(Player *player, unsigned char* packet, int size) {
	int mobid = BufferUtilities::getInt(packet);
	Mob *mob = getMob(mobid, player->getMap());
	if (mob == NULL)
		return;	
	if (mob->getControl() == player) {
		Pos cpos;
		cpos.x = BufferUtilities::getShort(packet+size-4);
		cpos.y = BufferUtilities::getShort(packet+size-2);
		if (cpos - mob->getPos() > 300) {
			if (player->addWarning()) return;
		}
		mob->setPos(cpos);
		mob->setType(packet[size-12]);
		MobsPacket::moveMob(player, mob, Maps::info[player->getMap()].Players, packet, size);
	}
}

void Mobs::addMob(int id, MobInfo mob) {
	mobinfo[id] = mob;
}

void Mobs::addSpawn(int id, SpawnsInfo spawns) {
	info[id] = spawns;
}

void Mobs::monsterControlSkill(Player *player, unsigned char* packet) {
	//TODO
}

void Mobs::checkSpawn(int mapid) {
	for (size_t i = 0; i < info[mapid].size(); i++) {
		// (Re-)spawn Mobs
		bool spawn = true;
		for (hash_map<int, Mob *>::iterator iter = mobs[mapid].begin(); iter != mobs[mapid].end(); iter++) {
			if (iter->second != NULL) {
				if (iter->second->getMapID() == i) {
					spawn = false;
				}
			}
		}
		if (spawn) {
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
			if (Maps::info[mapid].Players.size() > 0) {
				MobsPacket::spawnMob(Maps::info[mapid].Players[0], mob, Maps::info[mapid].Players, 1);
				updateSpawn(mapid, mob);
			}
		}
	}
}	

void Mobs::showMobs(Player *player) {
	updateSpawn(player->getMap()); 
	for (hash_map<int, Mob *>::iterator iter = Mobs::mobs[player->getMap()].begin(); iter != Mobs::mobs[player->getMap()].end(); iter++) {
		if (iter->second != 0)
			MobsPacket::showMob(player, iter->second);
	}
}

void Mobs::updateSpawn(int mapid) {
	for (hash_map<int, Mob *>::iterator iter = Mobs::mobs[mapid].begin(); iter != Mobs::mobs[mapid].end(); iter++) {
		if (iter->second != 0)
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
	MobsPacket::dieMob(player, Maps::info[player->getMap()].Players, mob, mob->getID());

	// Account for Holy Symbol
	int hsrate = 0;
	if (player->skills->getActiveSkillLevel(2311003)>0) {
		hsrate = Skills::skills[2311003][player->skills->getActiveSkillLevel(2311003)].x;
	}
	else if (player->skills->getActiveSkillLevel(5101002)>0) {
		hsrate = Skills::skills[5101002][player->skills->getActiveSkillLevel(5101002)].x;
	}

	Levels::giveEXP(player, (mobinfo[mob->getMobID()].exp + ((mobinfo[mob->getMobID()].exp*hsrate)/100)) * 10);
	Drops::dropMob(player, mob);
	
	// Spawn mob(s) the mob is supposed to spawn when it dies
	for (unsigned int i = 0; i < mobinfo[mob->getMobID()].summon.size(); i++) {
		spawnMobPos(player->getMap(), mobinfo[mob->getMobID()].summon[i], mob->getPosX(), mob->getPosY()-1);
	}

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
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for (int i=0; i<howmany; i++) {
		int mobid = BufferUtilities::getInt(packet+14+i*(22+4*(hits-1)));
		Mob* mob = getMob(mobid, map);
		for (int k=0; k<hits; k++) {
			int damage = BufferUtilities::getInt(packet+32+i*(22+4*(hits-1))+k*4);
			if (mob!=NULL) {
				mob->setHP(mob->getHP()-damage);
				int mhp=-1;
				mhp = mobinfo[mob->getMobID()].hp;
				// HP Bars
				if (mobinfo[mob->getMobID()].boss && mobinfo[mob->getMobID()].hpcolor > 0) // Boss HP bars
					MobsPacket::showBossHP(player, Maps::info[map].Players, mob->getMobID(), mob->getHP(), mhp, mobinfo[mob->getMobID()].hpcolor, mobinfo[mob->getMobID()].hpbgcolor);
				else if (mobinfo[mob->getMobID()].boss) // Miniboss HP bars
					MobsPacket::showMinibossHP(player, Maps::info[map].Players, mobid, mob->getHP()*100/mhp);
				else // Normal HP bars
					MobsPacket::showHP(player, mobid, mob->getHP()*100/mhp);
				if (mob->getHP() <= 0) {
					dieMob(player, mob);
					break;
				}
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
	bool s4211006 = (skillid == 4211006);
	int totaldmg = 0;
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for (int i=0; i<howmany; i++) {
        int mobid = BufferUtilities::getInt(packet+14+i*(22-s4211006+4*(hits-1)));
		Mob* mob = getMob(mobid, map);
		for (int k=0; k<hits; k++) {
			int damage = BufferUtilities::getInt(packet+32-s4211006+i*(22-s4211006+4*(hits-1))+k*4);
			totaldmg = totaldmg + damage;
			if (mob!=NULL) {
				if (mob->getPos() - player->getPos() > 300 && skillid == 0) {
					if (player->addWarning()) return;
				}
				mob->setHP(mob->getHP()-damage);
				int mhp=-1;
				mhp = mobinfo[mob->getMobID()].hp;
				// HP Bars
				if (mobinfo[mob->getMobID()].boss && mobinfo[mob->getMobID()].hpcolor > 0) // Boss HP bars
					MobsPacket::showBossHP(player, Maps::info[map].Players, mob->getMobID(), mob->getHP(), mhp, mobinfo[mob->getMobID()].hpcolor, mobinfo[mob->getMobID()].hpbgcolor);
				else if (mobinfo[mob->getMobID()].boss) // Miniboss HP bars
					MobsPacket::showMinibossHP(player, Maps::info[map].Players, mobid, mob->getHP()*100/mhp);
				else // Normal HP bars
					MobsPacket::showHP(player, mobid, mob->getHP()*100/mhp);
				if (mob->getHP() <= 0) {
					dieMob(player, mob);
					break;
				}
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
	for (int i = 0; i < player->inv->getItemNum(); i++) {
		if (player->inv->getItem(i)->pos == pos && player->inv->getItem(i)->inv == 2) {
			itemid = player->inv->getItem(i)->id;
			break;
		}
	}
	int map = player->getMap();
	MobsPacket::damageMobRanged(player, Maps::info[map].Players, packet, itemid);

	int howmany = packet[1]/0x10;
	int hits = packet[1]%0x10;
	int skillid = BufferUtilities::getInt(packet+2);

	bool s3121004 = (skillid == 3121004 || skillid == 3221001); // Hurricane/Pierce
	bool s4121006 = (player->skills->getActiveSkillLevel(4121006) > 0); // Shadow Claw

	if (!s4121006) {
		if (skillid == 4111005) // Avenger
			Inventory::takeItemSlot(player, pos, 2, 3*hits);
		else
			Inventory::takeItemSlot(player, pos, 2, hits);
	}
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	int damage, mhp;
	int totalDmg = 0;
	for (int i = 0; i < howmany; i++) {
		int mobid = BufferUtilities::getInt(packet+19+4*(s4121006+s3121004)+i*(22+4*(hits-1)));
		Mob* mob = getMob(mobid, map);
		for (int k = 0; k < hits; k++) {
			damage = BufferUtilities::getInt(packet+37+4*(s4121006+s3121004)+i*(22+4*(hits-1))+k*4);
			totalDmg += damage;
			if (mob != NULL) {
				mob->setHP(mob->getHP()-damage);
				mhp = mobinfo[mob->getMobID()].hp;
				// HP Bars
				if (mobinfo[mob->getMobID()].boss && mobinfo[mob->getMobID()].hpcolor > 0) // Boss HP bars
					MobsPacket::showBossHP(player, Maps::info[map].Players, mob->getMobID(), mob->getHP(), mhp, mobinfo[mob->getMobID()].hpcolor, mobinfo[mob->getMobID()].hpbgcolor);
				else if (mobinfo[mob->getMobID()].boss) // Miniboss HP bars
					MobsPacket::showMinibossHP(player, Maps::info[map].Players, mobid, mob->getHP()*100/mhp);
				else // Normal HP bars
					MobsPacket::showHP(player, mobid, mob->getHP()*100/mhp);
				if (mob->getHP() <= 0) {
					dieMob(player, mob);
					break;
				}
			}
		}
	}	
	if (skillid == 4101005) { // Drain
		int hpRecover = ((totalDmg * Skills::skills[4101005][player->skills->getSkillLevel(4101005)].x)/100);
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
	for (unsigned int j=0; j<Maps::info[mapid].Players.size(); j++)
		MobsPacket::showMob(Maps::info[mapid].Players[j], mob);
}

inline int Mobs::nextMobId(int mapid) {
	if (loopingIds.find(mapid) == loopingIds.end()) {
		loopingIds[mapid] = new LoopingId(100);
	}
	return loopingIds[mapid]->next();
}
