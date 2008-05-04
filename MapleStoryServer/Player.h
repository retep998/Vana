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
#ifndef PLAYER_H
#define PLAYER_H

#include "Connection/AbstractPlayer.h"
#include "Connection/PacketHandler.h"
#include "PlayerInventory.h"
#include <string.h>
#include <math.h>
#include <vector>
#include "Skills.h"

using namespace std;

struct Pos {
	short x;
	short y;
};

class NPC;
class PlayerSkills;
class PlayerQuests;

int distPos(Pos pos1, Pos pos2);

struct SkillMapEnterActiveInfo;

class Player:public AbstractPlayer {
public:
	Player () {
		isconnect=0;
		shop=0;
		npc=NULL;
		chair=0;
		skill.types[0] = 0;
		skill.types[1] = 0;
		skill.types[2] = 0;
		skill.types[3] = 0;
		skill.types[4] = 0;
		skill.types[5] = 0;
		skill.types[6] = 0;
		skill.types[7] = 0;
		skill.val = 0;
		skill.isval = false;
	}

	~Player();

	void handleRequest(unsigned char* buf, int len);
	void sendPacket(unsigned char* buf, int len){ packetHandler->sendPacket(buf,len); }
	void setPlayerid(int id){
		this->id = id;
	}
	int getPlayerid(){
		return this->id;
	}
	char* getName(){
		return this->name;
	}
	char getGender(){
		return this->gender;
	}
	void setSkin(char id);
	char getSkin(){
		return this->skin;
	}
	void setEyes(int id);
	int getEyes(){
		return this->eyes;
	}
	void setHair(int id);
	int getHair(){
		return this->hair;
	}
	void setLevel(int level);
	unsigned char getLevel(){
		return this->level;
	}
	void setJob(short job);
	short getJob(){
		return this->job;
	}
	void setStr(short str){
		this->str=str;
	}
	short getStr(){
		return this->str;
	}
	void setDex(short dex){
		this->dex=dex;
	}
	short getDex(){
		return this->dex;
	}
	void setInt(short intt){
		this->intt=intt;
	}
	short getInt(){
		return this->intt;
	}
	void setLuk(short luk){
		this->luk=luk;
	}
	short getLuk(){
		return this->luk;
	}
	void setHP(int hp, bool is=1);
	unsigned short getHP(){
		return this->hp;
	}
	void setMP(int mp, bool is=0);
	unsigned short getMP(){
		return this->mp;
	}
	void setMHP(int mhp){
		this->mhp=mhp;
	}
	unsigned short getMHP(){
		return this->mhp;
	}
	void setRMHP(int rmhp){
		this->rmhp=rmhp;
	}
	unsigned short getRMHP(){
		return this->rmhp;
	}
	void setMMP(int mmp){
		this->mmp=mmp;
	}
	unsigned short getMMP(){
		return this->mmp;
	}
	void setRMMP(int rmmp){
		this->rmmp=rmmp;
	}
	unsigned short getRMMP(){
		return this->rmmp;
	}
	void setAp(short ap){
		this->ap=ap;
	}
	short getAp(){
		return this->ap;
	}
	void setSp(short sp);
	short getSp(){
		return this->sp;
	}
	void setFame(short fame){
		this->fame=fame;
	}
	short getFame(){
		return this->fame;
	}
	void setExp(int exp, bool is);
	int getExp(){
		return this->exp;
	}
	void setMap(int map){
		this->map=map;
	}
	int getMap(){
		return this->map;
	}
	void setOrigin(){
		this->origin=this->map;
	}
	int getOrigin(){
		return this->origin;
	}
	void setMappos(char pos){
		this->mappos = pos;
	}
	char getMappos(){
		return this->mappos;
	}
	Pos getPos(){
		return this->pos;
	}
	void setPos(Pos pos){
		this->pos = pos;
	}
	void setType(char type){
		this->type=type;
	}
	char getType(){
		return this->type;
	}
	void setShop(int shopid){
		shop=shopid;
	}
	int getShop(){
		return shop;
	}
	void setNPC(NPC* npc){
		this->npc = npc;
	}
	void setChair(int chair){
		this->chair = chair;
	}
	int getChair(){
		return chair;
	}
	NPC* getNPC(){
		return npc;
	}
	int isGM(){
		return gm;
	}
	void setSkill(SkillMapEnterActiveInfo skill){
		this->skill = skill;
	}
	SkillMapEnterActiveInfo getSkill(){
		return skill;
	}
	void addWarning();
	void saveSkills();
	void saveStats();
	void saveEquips();
	void saveItems();
	void save();
	void setOffline();
	PlayerInventory* inv;
	PlayerSkills* skills;
	PlayerQuests* quests;
private:	
	bool isconnect;
	int id;
	char name[15];
	char gender;
	char skin;
	int eyes;
	int hair;
	unsigned char level;
	short job;
	short str;
	short dex;
	short intt;
	short luk;
	unsigned short hp;
	unsigned short mhp;
	unsigned short rmhp;
	unsigned short mp;
	unsigned short mmp;
	unsigned short rmmp;
	short ap;
	short sp;
	int exp;
	short fame;
	int map;
	int origin;
	char mappos;
	char type;
	int keys[90];
	int shop;
	int chair;
	int gm;
	Pos pos;
	NPC* npc;
	vector <int> warnings;
	void getUserID(unsigned char* buf){
		isconnect=1;
		setPlayerid(buf[0] + buf[1]*0x100 + buf[2]*0x100*0x100 + buf[3]*0x100*0x100*0x100);
		playerConnect();
	}
	void playerConnect();
	void changeKey(unsigned char* packet);
	SkillMapEnterActiveInfo skill;
};

class PlayerFactory:public AbstractPlayerFactory {
public:
	AbstractPlayer* createPlayer() {
		return new Player();
	}
};

#endif