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
#include "BuddyList.h"
#include "ChannelServer.h"
#include "MovableLife.h"
#include "PlayerActiveBuffs.h"
#include "PlayerInventory.h"
#include "PlayerPets.h"
#include "PlayerQuests.h"
#include "PlayerSkills.h"
#include "PlayerStorage.h"
#include "Pos.h"
#include "Quests.h"
#include "Skills.h"
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>

using std::string;
using std::vector;

class NPC;
class ReadPacket;

struct SkillMapEnterActiveInfo;

class Player : public AbstractPlayer, public MovableLife {
public:
	Player() : isconnect(false), save_on_dc(true), shop(0), npc(0), chair(0), itemEffect(0), tradestate(0) { }

	~Player();

	void realHandleRequest(ReadPacket *packet);
	int32_t getId() const { return this->id; }
	int32_t getUserId() const { return this->userid; }
	char getWorldId() const { return this->world_id; }
	string getName() const { return this->name; }
	char getGender() const { return this->gender; }
	void setSkin(char id);
	char getSkin() const { return this->skin; }
	void setEyes(int32_t id);
	int32_t getEyes() const { return this->eyes; }
	void setHair(int32_t id);
	int32_t getHair() const { return this->hair; }
	void setLevel(unsigned char level);
	unsigned char getLevel() const { return this->level; }
	void setJob(int16_t job);
	int16_t getJob() const { return this->job; }
	void setStr(int16_t str);
	int16_t getStr() const { return this->str; }
	void setDex(int16_t dex);
	int16_t getDex() const { return this->dex; }
	void setInt(int16_t intt);
	int16_t getInt() const { return this->intt; }
	void setLuk(int16_t luk);
	int16_t getLuk() const { return this->luk; }
	void setHP(uint16_t hp, bool is = true);
	uint16_t getHP() { return this->hp; }
	void setMP(uint16_t mp, bool is = false);
	uint16_t getMP() { return this->mp; }
	void setMHP(uint16_t mhp);
	uint16_t getMHP() { return this->mhp; }
	void setRMHP(uint16_t rmhp);
	uint16_t getRMHP() { return this->rmhp; }
	void setMMP(uint16_t mmp);
	uint16_t getMMP() { return this->mmp; }
	void setRMMP(uint16_t rmmp);
	uint16_t getRMMP() { return this->rmmp; }
	void setHPMPAp(uint16_t ap) { hpmp_ap = ap; }
	uint16_t getHPMPAp() { return hpmp_ap; }
	void setAp(int16_t ap);
	int16_t getAp() const { return this->ap; }
	void setSp(int16_t sp);
	int16_t getSp() const { return this->sp; }
	void setFame(int16_t fame);
	int16_t getFame() const { return this->fame; }
	void setExp(int32_t exp);
	int32_t getExp() const { return this->exp; }
	void setMap(int32_t map) { this->map = map; }
	int32_t getMap() const { return this->map; }
	void setMappos(char pos) { this->mappos = pos; }
	char getMappos() const { return this->mappos; }
	void setShop(int32_t shopid) { shop = shopid; }
	int32_t getShop() const { return shop; }
	void setNPC(NPC *npc) { this->npc = npc; }
	void setChair(int32_t chair) { this->chair = chair; }
	int32_t getChair() const { return chair; }
	void setItemEffect(int32_t effect) { this->itemEffect = effect; }
	int32_t getItemEffect() const { return this->itemEffect; }
	NPC * getNPC() const { return npc; }
	bool isGM() const { return gm > 0; }
	int32_t getGMLevel() const { return gm; }
	void setSkill(SkillMapEnterActiveInfo skill) { this->skill = skill; }
	void setSaveOnDC(bool save) { save_on_dc = save; }
	SkillMapEnterActiveInfo getSkill() { return skill; }
	void setSpecialSkill(SpecialSkillInfo info) { this->info = info; }
	int32_t getSpecialSkill() const { return this->info.skillid; }
	SpecialSkillInfo getSpecialSkillInfo() { return this->info; }
	void setTrading(char newstate) { tradestate = newstate; }
	char isTrading() const { return tradestate; }
	void setTradeSendID(int32_t id) { this->tradesendid = id; }
	int32_t getTradeSendID() const { return tradesendid; }
	void setTradeRecvID(int32_t id) { this->traderecvid = id; }
	int32_t getTradeRecvID() const { return traderecvid; }
	bool hasGMEquip();

	BuddyList * getBuddyList() const { return buddyList.get(); }
	PlayerActiveBuffs * getActiveBuffs() const { return activeBuffs.get(); }
	PlayerInventory * getInventory() const { return inv.get(); }
	PlayerPets * getPets() const { return pets.get(); }
	PlayerQuests * getQuests() const { return quests.get(); }
	PlayerSkills * getSkills() const { return skills.get(); }
	PlayerStorage * getStorage() const { return storage.get(); }

	void deleteVariable(const string &name);
	void setVariable(const string &name, const string &val);
	string getVariable(const string &name);

	bool addWarning();
	void changeChannel(char channel);
	void saveStats();
	void saveVariables();
	void saveAll();
	void setOnline(bool online);
	void acceptDeath();
private:
	void playerConnect(ReadPacket *packet);
	void changeKey(ReadPacket *packet);
	void changeSkillMacros(ReadPacket *packet);

	int32_t id;
	int32_t userid;
	char world_id;
	bool isconnect;
	string name;
	char gender;
	char skin;
	int32_t eyes;
	int32_t hair;
	unsigned char level;
	int16_t job;
	int16_t fame;
	int16_t str;
	int16_t dex;
	int16_t intt;
	int16_t luk;
	uint16_t hp;
	uint16_t mhp;
	uint16_t rmhp;
	uint16_t mp;
	uint16_t mmp;
	uint16_t rmmp;
	uint16_t hpmp_ap;
	int16_t ap;
	int16_t sp;
	int32_t exp;
	int32_t map;
	char mappos;
	int32_t shop;
	int32_t itemEffect;
	int32_t chair;
	int32_t gm;
	char tradestate;
	int32_t tradesendid;
	int32_t traderecvid;
	NPC *npc;
	vector<int32_t> warnings;
	bool save_on_dc;
	unordered_map<string, string> variables;
	SkillMapEnterActiveInfo skill;
	SpecialSkillInfo info; // Hurricane/Pierce

	boost::scoped_ptr<BuddyList> buddyList;
	boost::scoped_ptr<PlayerActiveBuffs> activeBuffs;
	boost::scoped_ptr<PlayerInventory> inv;
	boost::scoped_ptr<PlayerPets> pets;
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
