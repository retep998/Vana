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

#include "AbstractPlayer.h"
#include "PacketHandler.h"
#include "PlayerInventory.h"
#include "Skills.h"
#include "Quests.h"
#include "ChannelServer.h"
#include "BuddyList.h"
#include "Pos.h"
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <string>

using std::string;
using std::vector;

class NPC;
class ReadPacket;

struct SkillMapEnterActiveInfo;

class Player : public AbstractPlayer {
public:
	Player() : isconnect(false), save_on_dc(true), shop(0), npc(0), chair(0), itemEffect(0) { }

	~Player();

	void realHandleRequest(ReadPacket *packet);
	void setPlayerid(int id) {
		this->id = id;
	}
	int getPlayerid() {
		return this->id;
	}
	string getName() {
		return this->name;
	}
	char getGender() {
		return this->gender;
	}
	void setSkin(char id);
	char getSkin() {
		return this->skin;
	}
	void setEyes(int id);
	int getEyes() {
		return this->eyes;
	}
	void setHair(int id);
	int getHair() {
		return this->hair;
	}
	void setLevel(unsigned char level);
	unsigned char getLevel() {
		return this->level;
	}
	void setJob(short job);
	short getJob() {
		return this->job;
	}
	void setStr(short str);
	short getStr() {
		return this->str;
	}
	void setDex(short dex);
	short getDex() {
		return this->dex;
	}
	void setInt(short intt);
	short getInt() {
		return this->intt;
	}
	void setLuk(short luk);
	short getLuk() {
		return this->luk;
	}
	void setHP(int hp, bool is=1);
	unsigned short getHP() {
		return this->hp;
	}
	void setMP(int mp, bool is=0);
	unsigned short getMP() {
		return this->mp;
	}
	void setMHP(int mhp);
	unsigned short getMHP() {
		return this->mhp;
	}
	void setRMHP(int rmhp);
	unsigned short getRMHP() {
		return this->rmhp;
	}
	void setMMP(int mmp);
	unsigned short getMMP() {
		return this->mmp;
	}
	void setRMMP(int rmmp);
	unsigned short getRMMP() {
		return this->rmmp;
	}
	void setHPMPAp(unsigned short ap) {
		hpmp_ap = ap;
	}
	unsigned short getHPMPAp() {
		return hpmp_ap;
	}
	short getAp() {
		return this->ap;
	}
	short getSp() {
		return this->sp;
	}
	void setAp(short ap);
	void setSp(short sp);
	void setFame(short fame);
	short getFame() {
		return this->fame;
	}
	void setExp(int exp);
	int getExp() {
		return this->exp;
	}
	void setMap(int map) {
		this->map = map;
	}
	int getMap() {
		return this->map;
	}
	void setMappos(char pos) {
		this->mappos = pos;
	}
	char getMappos() {
		return this->mappos;
	}
	Pos getPos() {
		return this->pos;
	}
	void setPos(Pos pos) {
		this->pos = pos;
	}
	void setType(char type) {
		this->type = type;
	}
	char getType() {
		return this->type;
	}
	void setShop(int shopid) {
		shop = shopid;
	}
	int getShop() {
		return shop;
	}
	void setNPC(NPC *npc) {
		this->npc = npc;
	}
	void setChair(int chair) {
		this->chair = chair;
	}
	int getChair() {
		return chair;
	}
	void setItemEffect(int effect) {
		this->itemEffect = effect;
	}
	int getItemEffect() {
		return this->itemEffect;
	}
	NPC * getNPC() {
		return npc;
	}
	bool isGM() {
		return gm > 0;
	}
	int getGMLevel() {
		return gm;
	}
	void setSkill(SkillMapEnterActiveInfo skill) {
		this->skill = skill;
	}
	void setCombo(int combo) { // Combo Attack
		this->combo = combo;
	}
	int getCombo() { // Combo Attack
		return combo;
	}
	void setSaveOnDC(bool save) {
		save_on_dc = save;
	}
	SkillMapEnterActiveInfo getSkill() {
		return skill;
	}
	void deleteVariable(const string &name) {
		if (variables.find(name) != variables.end())
			variables.erase(name);
	}
	void setVariable(const string &name, const string &val) {
		variables[name] = val;
	}
	string getVariable(const string &name) {
		return (variables.find(name) == variables.end()) ? "" : variables[name];
	}
	void setSpecialSkill(SpecialSkillInfo info) {
		this->info = info;
	}
	int getSpecialSkill() {
		return this->info.skillid;
	}
	SpecialSkillInfo getSpecialSkillInfo() {
		return this->info;
	}
	bool addWarning();
	void changeChannel(char channel);
	void saveSkills();
	void saveStats();
	void saveEquips();
	void saveItems();
	void saveVariables();
	void save();
	void setOnline(bool online);
	boost::scoped_ptr<PlayerInventory> inv;
	boost::scoped_ptr<PlayerSkills> skills;
	boost::scoped_ptr<PlayerQuests> quests;
	boost::scoped_ptr<BuddyList> buddyList;
private:
	void playerConnect(ReadPacket *packet);
	void changeKey(ReadPacket *packet);
	void changeSkillMacros(ReadPacket *packet);

	bool isconnect;
	int id;
	string name;
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
	unsigned short hpmp_ap;
	short ap;
	short sp;
	int exp;
	short fame;
	int map;
	char mappos;
	char type;
	int shop;
	int itemEffect;
	int chair;
	int gm;
	Pos pos;
	NPC *npc;
	vector <int> warnings;
	int combo; // Combo Attack
	bool save_on_dc;
	hash_map<string, string> variables;
	SkillMapEnterActiveInfo skill;
	SpecialSkillInfo info; // Hurricane/Pierce
};

class PlayerFactory : public AbstractPlayerFactory {
public:
	AbstractPlayer * createPlayer() {
		return new Player();
	}
};

#endif
