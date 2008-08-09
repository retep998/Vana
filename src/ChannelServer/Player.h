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
#include "PlayerStorage.h"
#include "PlayerSkills.h"
#include "PlayerQuests.h"
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
	Player() : isconnect(false), save_on_dc(true), shop(0), npc(0), chair(0), itemEffect(0), tradestate(0) { }

	~Player();

	void realHandleRequest(ReadPacket *packet);
	int getPlayerid() const { return this->id; }
	string getName() const { return this->name; }
	char getGender() const { return this->gender; }
	void setSkin(char id);
	char getSkin() const { return this->skin; }
	void setEyes(int id);
	int getEyes() const { return this->eyes; }
	void setHair(int id);
	int getHair() const { return this->hair; }
	void setLevel(unsigned char level);
	unsigned char getLevel() const { return this->level; }
	void setJob(short job);
	short getJob() const { return this->job; }
	void setStr(short str);
	short getStr() const { return this->str; }
	void setDex(short dex);
	short getDex() const { return this->dex; }
	void setInt(short intt);
	short getInt() const { return this->intt; }
	void setLuk(short luk);
	short getLuk() const { return this->luk; }
	void setHP(int hp, bool is = true);
	unsigned short getHP() { return this->hp; }
	void setMP(int mp, bool is = false);
	unsigned short getMP() { return this->mp; }
	void setMHP(int mhp);
	unsigned short getMHP() { return this->mhp; }
	void setRMHP(int rmhp);
	unsigned short getRMHP() { return this->rmhp; }
	void setMMP(int mmp);
	unsigned short getMMP() { return this->mmp; }
	void setRMMP(int rmmp);
	unsigned short getRMMP() { return this->rmmp; }
	void setHPMPAp(unsigned short ap) { hpmp_ap = ap; }
	unsigned short getHPMPAp() { return hpmp_ap; }
	void setAp(short ap);
	short getAp() const { return this->ap; }
	void setSp(short sp);
	short getSp() const { return this->sp; }
	void setFame(short fame);
	short getFame() const { return this->fame; }
	void setExp(int exp);
	int getExp() const { return this->exp; }
	void setMap(int map) { this->map = map; }
	int getMap() const { return this->map; }
	void setMappos(char pos) { this->mappos = pos; }
	char getMappos() const { return this->mappos; }
	Pos getPos() const { return this->pos; }
	void setPos(Pos pos) { this->pos = pos; }
	void setType(char type) { this->type = type; }
	char getType() const { return this->type; }
	void setShop(int shopid) { shop = shopid; }
	int getShop() const { return shop; }
	void setNPC(NPC *npc) { this->npc = npc; }
	void setChair(int chair) { this->chair = chair; }
	int getChair() const { return chair; }
	void setItemEffect(int effect) { this->itemEffect = effect; }
	int getItemEffect() const { return this->itemEffect; }
	NPC * getNPC() const { return npc; }
	bool isGM() const { return gm > 0; }
	int getGMLevel() const { return gm; }
	void setSkill(SkillMapEnterActiveInfo skill) { this->skill = skill; }
	void setSaveOnDC(bool save) { save_on_dc = save; }
	SkillMapEnterActiveInfo getSkill() { return skill; }
	void setSpecialSkill(SpecialSkillInfo info) { this->info = info; }
	int getSpecialSkill() const { return this->info.skillid; }
	SpecialSkillInfo getSpecialSkillInfo() { return this->info; }
	void setTrading(char newstate) { tradestate = newstate; }
	char isTrading() const { return tradestate; }
	void setTradeSendID(int id) { this->tradesendid = id; }
	int getTradeSendID() const { return tradesendid; }
	void setTradeRecvID(int id) { this->traderecvid = id; }
	int getTradeRecvID() const { return traderecvid; }

	BuddyList * getBuddyList() const { return buddyList.get(); }
	PlayerInventory * getInventory() const { return inv.get(); }
	PlayerQuests * getQuests() const { return quests.get(); }
	PlayerSkills * getSkills() const { return skills.get(); }
	PlayerStorage * getStorage() const { return storage.get(); }

	void deleteVariable(const string &name);
	void setVariable(const string &name, const string &val);
	string getVariable(const string &name);

	bool addWarning();
	void changeChannel(char channel);
	void saveSkills();
	void saveStats();
	void saveInventory();
	void saveStorage();
	void saveVariables();
	void saveAll();
	void setOnline(bool online);
	void acceptDeath();
private:
	void playerConnect(ReadPacket *packet);
	void changeKey(ReadPacket *packet);
	void changeSkillMacros(ReadPacket *packet);

	int id;
	int userid;
	char world_id;
	bool isconnect;
	string name;
	char gender;
	char skin;
	int eyes;
	int hair;
	unsigned char level;
	short job;
	short fame;
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
	int map;
	char mappos;
	char type;
	int shop;
	int itemEffect;
	int chair;
	int gm;
	char tradestate;
	int tradesendid;
	int traderecvid;
	Pos pos;
	NPC *npc;
	vector <int> warnings;
	bool save_on_dc;
	hash_map<string, string> variables;
	SkillMapEnterActiveInfo skill;
	SpecialSkillInfo info; // Hurricane/Pierce

	boost::scoped_ptr<BuddyList> buddyList;
	boost::scoped_ptr<PlayerInventory> inv;
	boost::scoped_ptr<PlayerQuests> quests;
	boost::scoped_ptr<PlayerSkills> skills;
	boost::scoped_ptr<PlayerStorage> storage;
};

class PlayerFactory : public AbstractPlayerFactory {
public:
	AbstractPlayer * createPlayer() {
		return new Player();
	}
};

#endif
