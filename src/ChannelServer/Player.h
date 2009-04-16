/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "ChannelServer.h"
#include "MovableLife.h"
#include "PlayerActiveBuffs.h"
#include "PlayerBuddyList.h"
#include "PlayerInventory.h"
#include "PlayerPets.h"
#include "PlayerQuests.h"
#include "PlayerSkills.h"
#include "PlayerStorage.h"
#include "PlayerSummons.h"
#include "Pos.h"
#include "Quests.h"
#include "Skills.h"
#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Instance;
class LuaScriptable;
class NPC;
class PacketReader;
class Party;

class Player : public AbstractPlayer, public MovableLife {
public:
	Player() : tradestate(0), shop(0), itemEffect(0), chair(0), party(0), save_on_dc(true), isconnect(false), npc(0), luascriptable(0), instance(0) { }

	~Player();

	void realHandleRequest(PacketReader &packet);

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
	void modifyHp(int16_t hp, bool is = true); // Bases its calculations on current HP/MP
	void modifyMp(int16_t mp, bool is = false);
	void damageHp(uint16_t dhp); // Calculations done based on the fact that damage can range from 0 to ~55k
	void damageMp(uint16_t dmp);
	void setHp(int16_t hp, bool is = true); // Only use setHp/Mp if you're going to do checking, they fall easily to datatype issues
	void setMp(int16_t mp, bool is = false); // For example, Power Elixir at 30k HP = 30k + 30k = 60k, but wait! That's > 32767, so it's negative
	void setMHp(int16_t mhp);
	void setMMp(int16_t mmp);
	void modifyRMHp(int16_t mod);
	void modifyRMMp(int16_t mod);
	void setRMHp(int16_t rmhp);
	void setRMMp(int16_t rmmp);
	void setHyperBody(int16_t modx, int16_t mody);
	void setHpMpAp(uint16_t ap) { hpmp_ap = ap; }
	void setEyes(int32_t id);
	void setHair(int32_t id);
	void setExp(int32_t exp);
	void setMap(int32_t map) { this->map = map; }
	void setBuddyListSize(uint8_t size);
	void setTradeSendId(int32_t id) { this->tradesendid = id; }
	void setTradeRecvId(int32_t id) { this->traderecvid = id; }
	void setShop(int32_t shopid) { shop = shopid; }
	void setNPC(NPC *npc) { this->npc = npc; }
	void setParty(Party *party) { this->party = party; }
	void setInstance(Instance *instance) { this->instance = instance; }
	void setLuaScriptable(LuaScriptable *lua) { luascriptable = lua; }
	void setChair(int32_t chair) { this->chair = chair; }
	void setItemEffect(int32_t effect) { this->itemEffect = effect; }
	void setSpecialSkill(SpecialSkillInfo info) { this->info = info; }

	int8_t getWorldId() const { return world_id; }
	int8_t getGender() const { return gender; }
	int8_t getSkin() const { return skin; }
	int8_t isTrading() const { return tradestate; }
	int8_t getMappos() const { return mappos; }
	uint8_t getLevel() const { return level; }
	uint8_t getBuddyListSize() const { return buddylist_size; }
	int16_t getJob() const { return job; }
	int16_t getStr() const { return str; }
	int16_t getDex() const { return dex; }
	int16_t getInt() const { return intt; }
	int16_t getLuk() const { return luk; }
	int16_t getAp() const { return ap; }
	int16_t getSp() const { return sp; }
	int16_t getFame() const { return fame; }
	int16_t getHp() const { return hp; }
	int16_t getMp() const { return mp; }
	int16_t getMHp() const { return mhp; }
	int16_t getRMHp() const { return rmhp; }
	int16_t getMMp() const { return mmp; }
	int16_t getRMMp() const { return rmmp; }
	uint16_t getHpMpAp() const { return hpmp_ap; }
	int32_t getId() const { return id; }
	int32_t getUserId() const { return userid; }
	int32_t getEyes() const { return eyes; }
	int32_t getHair() const { return hair; }
	int32_t getExp() const { return exp; }
	int32_t getMap() const { return map; }
	int32_t getShop() const { return shop; }
	int32_t getChair() const { return chair; }
	int32_t getItemEffect() const { return itemEffect; }
	int32_t getGmLevel() const { return gm; }
	int32_t getSpecialSkill() const { return info.skillid; }
	int32_t getTradeSendID() const { return tradesendid; }
	int32_t getTradeRecvID() const { return traderecvid; }
	string getName() const { return name; }
	NPC * getNPC() const { return npc; }
	Party * getParty() const { return party; }
	Instance * getInstance() const { return instance; }
	LuaScriptable * getLuaScriptable() const { return luascriptable; }
	bool isGm() const { return gm > 0; }
	SpecialSkillInfo getSpecialSkillInfo() const { return info; }

	bool hasGmEquip();

	PlayerActiveBuffs * getActiveBuffs() const { return activeBuffs.get(); }
	PlayerSummons * getSummons() const { return summons.get(); }
	PlayerBuddyList * getBuddyList() const { return buddyList.get(); }
	PlayerInventory * getInventory() const { return inv.get(); }
	PlayerPets * getPets() const { return pets.get(); }
	PlayerQuests * getQuests() const { return quests.get(); }
	PlayerSkills * getSkills() const { return skills.get(); }
	PlayerStorage * getStorage() const { return storage.get(); }

	void deleteVariable(const string &name);
	void setVariable(const string &name, const string &val);
	string getVariable(const string &name);

	void addCooldown(int32_t skillid, int16_t time);
	void removeCooldown(int32_t skillid);
	int16_t getCooldownSize() const { return static_cast<int16_t>(cooldowns.size()); }
	unordered_map<int32_t, int16_t> getCooldowns() const { return cooldowns; }

	bool addWarning();
	void changeChannel(int8_t channel);
	void saveStats();
	void saveVariables();
	void saveCooldowns();
	void saveAll(bool savecooldowns = false);
	void setOnline(bool online);
	void setLevelDate();
	void acceptDeath();
	void loseExp();
private:
	void playerConnect(PacketReader &packet);
	void changeKey(PacketReader &packet);
	void changeSkillMacros(PacketReader &packet);

	int8_t world_id;
	int8_t mappos;
	int8_t tradestate;
	int8_t gender;
	int8_t skin;
	uint8_t level;
	uint8_t buddylist_size;
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
	unordered_map<int32_t, int16_t> cooldowns;
	NPC *npc;
	Instance *instance;
	LuaScriptable *luascriptable;
	Party *party;
	vector<int32_t> warnings;
	SpecialSkillInfo info; // Hurricane/Pierce/Big Bang/Monster Magnet/etc.

	boost::scoped_ptr<PlayerActiveBuffs> activeBuffs;
	boost::scoped_ptr<PlayerSummons> summons;
	boost::scoped_ptr<PlayerBuddyList> buddyList;
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
