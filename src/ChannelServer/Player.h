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

class Player : public AbstractPlayer, public MovableLife {
public:
	Player() : isconnect(false), save_on_dc(true), shop(0), npc(0), chair(0), itemEffect(0), tradestate(0) { }

	~Player();

	void realHandleRequest(ReadPacket *packet);

	void setSaveOnDC(bool save) { save_on_dc = save; }
	void setTrading(int8_t newstate) { tradestate = newstate; }
	void setSkin(int8_t id);
	void setMappos(int8_t pos) { this->mappos = pos; }
	void setLevel(uint8_t level);
	void setAp(int16_t ap);
	void setSp(int16_t sp);
	void setFame(int16_t fame);
	void setJob(int16_t job);
	void setStr(int16_t str);
	void setDex(int16_t dex);
	void setInt(int16_t intt);
	void setLuk(int16_t luk);
	void setHP(int16_t hp, bool is = true);
	void setMP(int16_t mp, bool is = false);
	void setMHP(int16_t mhp);
	void setRMHP(int16_t rmhp);
	void setMMP(int16_t mmp);
	void setRMMP(int16_t rmmp);
	void setHPMPAp(uint16_t ap) { hpmp_ap = ap; }
	void setEyes(int32_t id);
	void setHair(int32_t id);
	void setExp(int32_t exp);
	void setMap(int32_t map) { this->map = map; }
	void setTradeSendID(int32_t id) { this->tradesendid = id; }
	void setTradeRecvID(int32_t id) { this->traderecvid = id; }
	void setShop(int32_t shopid) { shop = shopid; }
	void setNPC(NPC *npc) { this->npc = npc; }
	void setChair(int32_t chair) { this->chair = chair; }
	void setItemEffect(int32_t effect) { this->itemEffect = effect; }
	void setSpecialSkill(SpecialSkillInfo info) { this->info = info; }

	int8_t getWorldId() const { return this->world_id; }
	int8_t getGender() const { return this->gender; }
	int8_t getSkin() const { return this->skin; }
	int8_t isTrading() const { return tradestate; }
	int8_t getMappos() const { return this->mappos; }
	uint8_t getLevel() const { return this->level; }
	int16_t getJob() const { return this->job; }
	int16_t getStr() const { return this->str; }
	int16_t getDex() const { return this->dex; }
	int16_t getInt() const { return this->intt; }
	int16_t getLuk() const { return this->luk; }
	int16_t getAp() const { return this->ap; }
	int16_t getSp() const { return this->sp; }
	int16_t getFame() const { return this->fame; }
	int16_t getHP() const { return this->hp; }
	int16_t getMP() const { return this->mp; }
	int16_t getMHP() const { return this->mhp; }
	int16_t getRMHP() const { return this->rmhp; }
	int16_t getMMP() const { return this->mmp; }
	int16_t getRMMP() const { return this->rmmp; }
	uint16_t getHPMPAp() const { return hpmp_ap; }
	int32_t getId() const { return this->id; }
	int32_t getUserId() const { return this->userid; }
	int32_t getEyes() const { return this->eyes; }
	int32_t getHair() const { return this->hair; }
	int32_t getExp() const { return this->exp; }
	int32_t getMap() const { return this->map; }
	int32_t getShop() const { return shop; }
	int32_t getChair() const { return chair; }
	int32_t getItemEffect() const { return this->itemEffect; }
	int32_t getGMLevel() const { return gm; }
	int32_t getSpecialSkill() const { return this->info.skillid; }
	int32_t getTradeSendID() const { return tradesendid; }
	int32_t getTradeRecvID() const { return traderecvid; }
	string getName() const { return this->name; }
	NPC * getNPC() const { return npc; }
	bool isGM() const { return gm > 0; }
	SpecialSkillInfo getSpecialSkillInfo() const { return this->info; }

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
	void changeChannel(int8_t channel);
	void set200Date();
	void saveStats();
	void saveVariables();
	void saveAll();
	void setOnline(bool online);
	void acceptDeath();
private:
	void playerConnect(ReadPacket *packet);
	void changeKey(ReadPacket *packet);
	void changeSkillMacros(ReadPacket *packet);

	int8_t world_id;
	int8_t mappos;
	int8_t tradestate;
	int8_t gender;
	int8_t skin;
	uint8_t level;
	int16_t job;
	int16_t fame;
	int16_t str;
	int16_t dex;
	int16_t intt;
	int16_t luk;
	int16_t ap;
	int16_t sp;
	int16_t hp;
	int16_t mhp;
	int16_t rmhp;
	int16_t mp;
	int16_t mmp;
	int16_t rmmp;
	uint16_t hpmp_ap;
	int32_t id;
	int32_t userid;
	int32_t eyes;
	int32_t hair;
	int32_t exp;
	int32_t map;
	int32_t shop;
	int32_t itemEffect;
	int32_t chair;
	int32_t gm;
	int32_t tradesendid;
	int32_t traderecvid;
	bool save_on_dc;
	bool isconnect;
	string name;
	unordered_map<string, string> variables;
	NPC *npc;
	vector<int32_t> warnings;
	SpecialSkillInfo info; // Hurricane/Pierce/Big Bang/Monster Magnet/etc.

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
