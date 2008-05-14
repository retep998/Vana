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
#include <math.h>
hash_map <int, MobInfo> Mobs::mobinfo;
hash_map <int, SpawnsInfo> Mobs::info;
hash_map <int, vector<Mob*>> Mobs::mobs;
int Mobs::mobscount=0x200;

int getDistance(Pos a, Pos b){
	return (int)sqrt((double)(pow((double)(a.x-b.x), 2.0) + pow((double)(a.y-b.y), 2.0)));
}

void Mob::setControl(Player* control){
	if(this->control != NULL)
		MobsPacket::endControlMob(this->control, this);

	this->control = control;
	if(control != NULL)
		MobsPacket::controlMob(control, this);
}

void Mobs::monsterControl(Player* player, unsigned char* packet, int size){
	int mobid = BufferUtilities::getInt(packet);
	Mob* mob = getMobByID(mobid, player->getMap());
	if(mob == NULL)
		return;	
	if(mob->getControl() == player){
	//if(mob->getControl() != player)
		//mob->setControl(player);
		Pos cpos;
		cpos.x = BufferUtilities::getShort(packet+size-4);
		cpos.y = BufferUtilities::getShort(packet+size-2);
		if(getDistance(cpos, mob->getPos()) > 300){
			player->addWarning();
		}
		mob->setPos(cpos);
		mob->setType(packet[size-12]);
		MobsPacket::moveMob(player, mob, Maps::info[player->getMap()].Players, packet, size);
	}
}

void Mobs::addMob(int id, MobInfo mob){
	mobinfo[id] = mob;
}

void Mobs::addSpawn(int id, SpawnsInfo spawns){
	info[id] = spawns;
}

void Mobs::monsterControlSkill(Player* player, unsigned char* packet){
	//TODO
}

void Mobs::checkSpawn(int mapid){
	for(unsigned int i=0; i<info[mapid].size(); i++){
		int check=0;
		for(unsigned int j=0; j<mobs[mapid].size(); j++){
			if(i == mobs[mapid][j]->getMapID()){
				check=1;
				break;
			}
		}
		if(!check){
			Mob* mob = new Mob();
			mobs[mapid].push_back(mob);
			Pos mobpos;
			mobpos.x = info[mapid][i].x;
			mobpos.y = info[mapid][i].cy;
			mob->setPos(mobpos);
			mob->setID(i+100);
			mob->setMobID(info[mapid][i].id);
			mob->setMapID(i);
			mob->setHP(mobinfo[info[mapid][i].id].hp);
			mob->setMP(mobinfo[info[mapid][i].id].mp);
			mob->setFH(info[mapid][i].fh);
			mob->setType(2);
			if(Maps::info[mapid].Players.size()>0)
				MobsPacket::spawnMob(Maps::info[mapid].Players[0], mob, Maps::info[mapid].Players, 1);
		}
		Mob* mob = NULL;
		for(unsigned int j=0; j<mobs[mapid].size(); j++)
			if(i == mobs[mapid][j]->getMapID()){
				mob = mobs[mapid][j];
				break;
			}
			
		if(mob == NULL){
			for(unsigned int j=0; j<mobs[mapid].size(); j++)
				if(i == mobs[mapid][j]->getMapID()){
					mob = mobs[mapid][j];
					break;
				}
			if(mob == NULL)
				continue;
		}
		if(Maps::info[mapid].Players.size() > 0 && mob->getControl()==0){
			int maxpos = distPos(mob->getPos(), Maps::info[mapid].Players[0]->getPos());
			int posi = 0;
			for(unsigned int k=0; k<Maps::info[mapid].Players.size(); k++){
				int curpos = distPos(mob->getPos(), Maps::info[mapid].Players[k]->getPos());
				if(curpos < maxpos){
					maxpos = curpos;
					posi = k;
				}
			}
			mob->setControl(Maps::info[mapid].Players[posi]);
		}
	}
}	

void Mobs::showMobs(Player* player){
	updateSpawn(player->getMap()); 
	for(unsigned int i=0; i<mobs[player->getMap()].size(); i++){
		MobsPacket::showMob(player, mobs[player->getMap()][i]);
	}
}

void Mobs::updateSpawn(int mapid){
	for(unsigned int i=0; i<mobs[mapid].size(); i++){
		if(Maps::info[mapid].Players.size() > 0 && mobs[mapid][i]->getControl()==NULL){
			int maxpos = distPos(mobs[mapid][i]->getPos(), Maps::info[mapid].Players[0]->getPos());
			int posi = 0;
			for(unsigned int k=0; k<Maps::info[mapid].Players.size(); k++){
				int curpos = distPos(mobs[mapid][i]->getPos(), Maps::info[mapid].Players[k]->getPos());
				if(curpos < maxpos){
					maxpos = curpos;
					posi = k;
				}
			}
			mobs[mapid][i]->setControl(Maps::info[mapid].Players[posi]);
		}
		else if(Maps::info[mapid].Players.size() == 0)
			mobs[mapid][i]->setControl(NULL);
	}
}
void Mobs::dieMob(Player* player, Mob* mob){
	MobsPacket::dieMob(player, Maps::info[player->getMap()].Players, mob , mob->getID());

	// Account for Holy Symbol
	int hsrate = 0;
	if(player->skills->getActiveSkillLevel(2311003)>0){
		hsrate = Skills::skills[2311003][player->skills->getActiveSkillLevel(2311003)].x;
	}
	else if(player->skills->getActiveSkillLevel(5101002)>0){
		hsrate = Skills::skills[5101002][player->skills->getActiveSkillLevel(5101002)].x;
	}

	Levels::giveEXP(player, (mobinfo[mob->getMobID()].exp + ((mobinfo[mob->getMobID()].exp*hsrate)/100)) * 10);
	Drops::dropMob(player, mob);
	
	// Spawn mobs it's supposed to spawn when it dies
	vector<int>::iterator vi = mobinfo[mob->getMobID()].summon.begin();
	while (vi != mobinfo[mob->getMobID()].summon.end()) {
		int mobid = *vi++;
		Mob* mobb = new Mob();
		mobs[player->getMap()].push_back(mobb);
		mobb->setPos(mob->getPos());
		mobb->setID(mobscount++);
		mobb->setMobID(mobid);
		mobb->setMapID(-1);
		mobb->setHP(mobinfo[mobid].hp);
		mobb->setMP(mobinfo[mobid].mp);
		mobb->setFH(0);
		mobb->setControl(player);
		mobb->setType(2);
		for(unsigned int j=0; j<Maps::info[player->getMap()].Players.size(); j++)
			MobsPacket::showMob(Maps::info[player->getMap()].Players[j], mobb);
	}

	player->quests->updateQuestMob(mob->getMobID());
	for(unsigned int i=0; i<mobs[player->getMap()].size(); i++){
		if(mobs[player->getMap()][i] == mob){
			mobs[player->getMap()].erase(mobs[player->getMap()].begin()+i);
			break;
		}
	}
	delete mob;
}

Mob* Mobs::getMobByID(int mobid, int map){
	for(unsigned int i=0; i<mobs[map].size(); i++){
		if(mobs[map][i]->getID() == mobid){
			return mobs[map][i];
		}
	}
	return NULL;
}
void Mobs::damageMobSpell(Player* player, unsigned char* packet){
	MobsPacket::damageMobSpell(player, Maps::info[player->getMap()].Players, packet);
	int howmany = packet[1]/0x10;
	int hits = packet[1]%0x10;
	int map = player->getMap();
	int skillid = BufferUtilities::getInt(packet+2);
	if(skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for(int i=0; i<howmany; i++){
		int mobid = BufferUtilities::getInt(packet+14+i*(22+4*(hits-1)));
		Mob* mob = getMobByID(mobid, map);
		for(int k=0; k<hits; k++){
			int damage = BufferUtilities::getInt(packet+32+i*(22+4*(hits-1))+k*4);
			if(mob!=NULL){
				mob->setHP(mob->getHP()-damage);
				int mhp=-1;
				mhp = mobinfo[mob->getMobID()].hp;
				MobsPacket::showHP(player, mobid ,mob->getHP()*100/mhp);
				if(mob->getHP() <= 0){
					dieMob(player, mob);
					break;
				}
			}
		}
	}	
}

void Mobs::damageMob(Player* player, unsigned char* packet){
	MobsPacket::damageMob(player, Maps::info[player->getMap()].Players, packet);
	int howmany = packet[1]/0x10;
	int hits = packet[1]%0x10;
	int map = player->getMap();
	int skillid = BufferUtilities::getInt(packet+2);
	bool s4211006 = false;
	if(skillid == 4211006)
		s4211006 = true;
	int totaldmg = 0;
	if(skillid > 0)
		Skills::useAttackSkill(player, skillid);
	for(int i=0; i<howmany; i++){
        int mobid = BufferUtilities::getInt(packet+14+i*(22-s4211006+4*(hits-1)));
		Mob* mob = getMobByID(mobid, map);
		for(int k=0; k<hits; k++){
			int damage = BufferUtilities::getInt(packet+32-s4211006+i*(22-s4211006+4*(hits-1))+k*4);
			totaldmg = totaldmg + damage;
			if(mob!=NULL){
				if(getDistance(mob->getPos(), player->getPos()) > 300 && skillid == 0){
					player->addWarning();
				}
				mob->setHP(mob->getHP()-damage);
				int mhp=-1;
				mhp = mobinfo[mob->getMobID()].hp;
				MobsPacket::showHP(player, mobid ,mob->getHP()*100/mhp);
				if(mob->getHP() <= 0){
					dieMob(player, mob);
					break;
				}
			}
		}
	}
	if (s4211006){
        unsigned char howmanyitems = packet[(14+howmany*(21+4*(hits-1)))+4];
        for(int i=0; i<howmanyitems; i++){
            int objID = BufferUtilities::getInt(packet+(14+howmany*(21+4*(hits-1)))+5+(5*i));
            for(unsigned int i=0; i<Drops::drops[map].size(); i++){
                if(Drops::drops[map][i]->getObjID() == objID){
                    DropsPacket::explodeDrop(Maps::info[map].Players, Drops::drops[map][i]);
                    Drops::drops[map].erase(Drops::drops[map].begin()+i);
                    break;
                }
            }
        }
    }
	if (skillid == 1111003 || skillid == 1111004 || skillid == 1111005 || skillid == 1111006){ // Combo finishing moves
		Skills::clearCombo(player);
	}
	else if(totaldmg > 0){ // Only add orbs for attacks that did damage
		Skills::addCombo(player, 1);
	}
}
void Mobs::damageMobRanged(Player* player, unsigned char* packet, int size){
	int itemid=0;
	int pos=BufferUtilities::getInt(packet+14);
	for(int i=0; i<player->inv->getItemNum(); i++){
		if(player->inv->getItem(i)->pos == pos && player->inv->getItem(i)->inv == 2){
			itemid = player->inv->getItem(i)->id;
			break;
		}
	}
	MobsPacket::damageMobRanged(player, Maps::info[player->getMap()].Players, packet, itemid);
	int howmany = packet[1]/0x10;
	int hits = packet[1]%0x10;
	//Inventory::takeItemSlot(player, pos, 2, hits); // TODO
	int map = player->getMap();
	int skillid = BufferUtilities::getInt(packet+2);
	bool s3121004 = false;
	if(skillid == 3121004 || skillid == 3221001)
		s3121004 = true;
	if(skillid > 0)
		Skills::useAttackSkill(player, skillid);
	int damage, mhp;
	int totalDmg = 0;
	for(int i=0; i<howmany; i++){
		int mobid = BufferUtilities::getInt(packet+19+4*s3121004+i*(22+4*(hits-1)));
		Mob* mob = getMobByID(mobid, map);
		for(int k=0; k<hits; k++){
			damage = BufferUtilities::getInt(packet+37+4*s3121004+i*(22+4*(hits-1))+k*4);
			totalDmg += damage;
			if(mob!=NULL){
				mob->setHP(mob->getHP()-damage);
				mhp = mobinfo[mob->getMobID()].hp;
				MobsPacket::showHP(player, mobid ,mob->getHP()*100/mhp);
				if(mob->getHP() <= 0){
					dieMob(player, mob);
					break;
				}
			}
		}
	}	
	if(skillid == 4101005){ // Drain
		int hpRecover = ((totalDmg * Skills::skills[4101005][player->skills->getSkillLevel(4101005)].x)/100);
		if (hpRecover > mhp)
			hpRecover = mhp;
		if (hpRecover > player->getMHP()/2)
			hpRecover = player->getMHP()/2;
		if(player->getHP()+hpRecover > player->getMHP())
			player->setHP(player->getMHP());
		else
			player->setHP(player->getHP()+hpRecover);
	}
}

void Mobs::spawnMob(Player* player, int mobid){
	Mob* mob = new Mob();
	mobs[player->getMap()].push_back(mob);
	mob->setPos(player->getPos());
	mob->setID(mobscount++);
	mob->setMobID(mobid);
	mob->setMapID(-1);
	mob->setHP(mobinfo[mobid].hp);
	mob->setMP(mobinfo[mobid].mp);
	mob->setFH(0);
	mob->setType(2);
	mob->setControl(player);
	for(unsigned int j=0; j<Maps::info[player->getMap()].Players.size(); j++)
		MobsPacket::showMob(Maps::info[player->getMap()].Players[j], mob);
}

void Mobs::spawnMobPos(Player* player, int mobid, int xx, int yy){
    Mob* mob = new Mob();
    mobs[player->getMap()].push_back(mob);
    Pos spawnPos;
    spawnPos.x = xx;
    spawnPos.y = yy;
    mob->setPos(spawnPos);
    mob->setID(mobscount++);
    mob->setMobID(mobid);
    mob->setMapID(-1);
    mob->setHP(mobinfo[mobid].hp);
    mob->setMP(mobinfo[mobid].mp);
    mob->setFH(0);
    mob->setControl(player);
    mob->setType(2);
	for(unsigned int j=0; j<Maps::info[player->getMap()].Players.size(); j++)
		MobsPacket::showMob(Maps::info[player->getMap()].Players[j], mob);
}
