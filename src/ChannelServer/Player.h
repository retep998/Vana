/*
Copyright (C) 2008-2012 Vana Development Team

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
#pragma once

#include "AbstractConnection.h"
#include "MovableLife.h"
#include "noncopyable.hpp"
#include "Npc.h"
#include "PlayerActiveBuffs.h"
#include "PlayerBuddyList.h"
#include "PlayerInventory.h"
#include "PlayerMonsterBook.h"
#include "PlayerMounts.h"
#include "PlayerPets.h"
#include "PlayerQuests.h"
#include "PlayerSkills.h"
#include "PlayerStats.h"
#include "PlayerStorage.h"
#include "PlayerSummons.h"
#include "PlayerVariables.h"
#include "SkillDataProvider.h"
#include "TauswortheGenerator.h"
#include <ctime>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

using std::string;
using std::unique_ptr;
using std::unordered_set;
using std::vector;

class Instance;
class PacketReader;
class Party;
struct PortalInfo;

class Player : boost::noncopyable, public AbstractConnection, public MovableLife {
public:
	Player();
	~Player();

	void handleRequest(PacketReader &packet);

	void setSaveOnDc(bool save) { m_saveOnDc = save; }
	void setTrading(bool state) { m_tradeState = state; }
	void setChangingChannel(bool v) { m_changingChannel = v; }
	void setSkin(int8_t id);
	void setFallCounter(int8_t falls) { m_fallCounter = falls; }
	void setMapChair(int16_t s) { m_mapChair = s; }
	void setEyes(int32_t id);
	void setHair(int32_t id);
	void setMap(int32_t mapId, PortalInfo *portal = nullptr, bool instance = false);
	void setBuddyListSize(uint8_t size);
	void setConnectionTime(int64_t newtime) { m_onlineTime = newtime; }
	void setTradeId(int32_t id) { m_tradeId = id; }
	void setShop(int32_t shopId) { m_shop = shopId; }
	void setChair(int32_t chair) { m_chair = chair; }
	void setItemEffect(int32_t effect) { m_itemEffect = effect; }
	void setChalkboard(const string &msg) { m_chalkboard = msg; }
	void setSpecialSkill(const SpecialSkillInfo &info) { m_info = info; }
	void setNpc(Npc *npc) { m_npc.reset(npc); }
	void setParty(Party *party) { m_party = party; }
	void setInstance(Instance *instance) { m_instance = instance; }

	bool isGm() const { return m_gmLevel > 0; }
	bool isAdmin() const { return m_admin; }
	bool isChangingChannel() const { return m_changingChannel; }
	bool isTrading() const { return m_tradeState; }
	bool hasGmEquip() const;
	int8_t getWorldId() const { return m_worldId; }
	int8_t getGender() const { return m_gender; }
	int8_t getSkin() const { return m_skin; }
	int8_t getMappos() const { return m_mapPos; }
	int8_t getFallCounter() const { return m_fallCounter; }
	uint8_t getBuddyListSize() const { return m_buddylistSize; }
	uint8_t getPortalCount(bool add = false);
	int16_t getMapChair() const { return m_mapChair; }
	int32_t getId() const { return m_id; }
	int32_t getUserId() const { return m_userId; }
	int32_t getEyes() const { return m_eyes; }
	int32_t getHair() const { return m_hair; }
	int32_t getMap() const { return m_map; }
	int32_t getLastMap() const { return m_lastMap; }
	int32_t getShop() const { return m_shop; }
	int32_t getChair() const { return m_chair; }
	int32_t getItemEffect() const { return m_itemEffect; }
	int32_t getGmLevel() const { return m_gmLevel; }
	int32_t getSpecialSkill() const { return m_info.skillId; }
	int32_t getTradeId() const { return m_tradeId; }
	int64_t getConnectionTime() const { return m_onlineTime; }
	int64_t getConnectedTime() const { return time(nullptr) - m_onlineTime; }
	string getChalkboard() const { return m_chalkboard; }
	string getMedalName();
	string getName() const { return m_name; }
	SpecialSkillInfo getSpecialSkillInfo() const { return m_info; }

	Npc * getNpc() const { return m_npc.get(); }
	Party * getParty() const { return m_party; }
	Instance * getInstance() const { return m_instance; }
	PlayerActiveBuffs * getActiveBuffs() const { return m_activeBuffs.get(); }
	PlayerBuddyList * getBuddyList() const { return m_buddyList.get(); }
	PlayerInventory * getInventory() const { return m_inventory.get(); }
	PlayerMonsterBook * getMonsterBook() const { return m_monsterBook.get(); }
	PlayerMounts * getMounts() const { return m_mounts.get(); }
	PlayerPets * getPets() const { return m_pets.get(); }
	PlayerQuests * getQuests() const { return m_quests.get(); }
	PlayerSkills * getSkills() const { return m_skills.get(); }
	PlayerStats * getStats() const { return m_stats.get(); }
	PlayerStorage * getStorage() const { return m_storage.get(); }
	PlayerSummons * getSummons() const { return m_summons.get(); }
	PlayerVariables * getVariables() const { return m_variables.get(); }

	// For "onlyOnce" portals
	void addUsedPortal(int8_t portalId) { m_usedPortals.insert(portalId); }
	bool usedPortal(int8_t portalId) const { return m_usedPortals.find(portalId) != m_usedPortals.end(); }

	void changeChannel(int8_t channel);
	void saveAll(bool saveCooldowns = false);
	void setOnline(bool online);
	void setLevelDate();
	void acceptDeath(bool wheel);
	void initializeRng(PacketCreator &packet);
private:
	void playerConnect(PacketReader &packet);
	void changeKey(PacketReader &packet);
	void changeSkillMacros(PacketReader &packet);
	void saveStats();
	bool equippedUtility(int16_t slot, int32_t itemId) const;

	bool m_tradeState;
	bool m_saveOnDc;
	bool m_isConnect;
	bool m_changingChannel;
	bool m_admin;
	int8_t m_worldId;
	int8_t m_mapPos;
	int8_t m_gender;
	int8_t m_skin;
	int8_t m_fallCounter;
	uint8_t m_buddylistSize;
	uint8_t m_portalCount;
	int16_t m_mapChair;
	int32_t m_id;
	int32_t m_userId;
	int32_t m_eyes;
	int32_t m_hair;
	int32_t m_map;
	int32_t m_lastMap;
	int32_t m_shop;
	int32_t m_itemEffect;
	int32_t m_chair;
	int32_t m_gmLevel;
	int32_t m_tradeId;
	int64_t m_onlineTime;
	string m_chalkboard;
	string m_name;
	unordered_set<int8_t> m_usedPortals;
	SpecialSkillInfo m_info; // Hurricane/Pierce/Big Bang/Monster Magnet/etc.

	Instance *m_instance;
	Party *m_party;
	unique_ptr<Npc> m_npc;
	unique_ptr<PlayerActiveBuffs> m_activeBuffs;
	unique_ptr<PlayerBuddyList> m_buddyList;
	unique_ptr<PlayerInventory> m_inventory;
	unique_ptr<PlayerMonsterBook> m_monsterBook;
	unique_ptr<PlayerMounts> m_mounts;
	unique_ptr<PlayerPets> m_pets;
	unique_ptr<PlayerQuests> m_quests;
	unique_ptr<PlayerSkills> m_skills;
	unique_ptr<PlayerStats> m_stats;
	unique_ptr<PlayerStorage> m_storage;
	unique_ptr<PlayerSummons> m_summons;
	unique_ptr<PlayerVariables> m_variables;
	unique_ptr<TauswortheGenerator> m_randStream;
};

class PlayerFactory : public AbstractConnectionFactory {
public:
	AbstractConnection * createConnection() {
		return new Player();
	}
};