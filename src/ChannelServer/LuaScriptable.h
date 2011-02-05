/*
Copyright (C) 2008-2011 Vana Development Team

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

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include "Types.h"
#include <string>

using std::string;

class Instance;
class Player;

class LuaScriptable {
public:
	LuaScriptable(const string &filename, int32_t playerid);
	virtual ~LuaScriptable();

	void initialize();
	virtual bool run();

	void setVariable(const string &name, int32_t val);
	void setVariable(const string &name, const string &val);
protected:
	virtual void handleError();
	void printError(const string &error);

	string filename;
	int32_t playerid;
	lua_State *luaVm;
};

namespace LuaExports {
	Player * getPlayer(lua_State *luaVm);
	Player * getPlayerDeduced(int parameter, lua_State *luaVm);
	Instance * getInstance(lua_State *luaVm);

	// Global exports

	// Miscellaneous
	int consoleOutput(lua_State *luaVm);
	int getRandomNumber(lua_State *luaVm);
	int log(lua_State *luaVm);
	int showWorldMessage(lua_State *luaVm);
	int showGlobalMessage(lua_State *luaVm);

	// Channel
	int deleteChannelVariable(lua_State *luaVm);
	int getChannel(lua_State *luaVm);
	int getChannelVariable(lua_State *luaVm);
	int isHorntailChannel(lua_State *luaVm);
	int isPapChannel(lua_State *luaVm);
	int isPianusChannel(lua_State *luaVm);
	int isPinkBeanChannel(lua_State *luaVm);
	int isZakumChannel(lua_State *luaVm);
	int setChannelVariable(lua_State *luaVm);
	int showChannelMessage(lua_State *luaVm);

	// Bosses
	int getHorntailChannels(lua_State *luaVm);
	int getMaxHorntailBattles(lua_State *luaVm);
	int getMaxPapBattles(lua_State *luaVm);
	int getMaxPianusBattles(lua_State *luaVm);
	int getMaxPinkBeanBattles(lua_State *luaVm);
	int getMaxZakumBattles(lua_State *luaVm);
	int getPapChannels(lua_State *luaVm);
	int getPianusChannels(lua_State *luaVm);
	int getPinkBeanChannels(lua_State *luaVm);
	int getZakumChannels(lua_State *luaVm);

	// NPC
	int isBusy(lua_State *luaVm);
	int removeNpc(lua_State *luaVm);
	int runNpc(lua_State *luaVm);
	int showShop(lua_State *luaVm);
	int spawnNpc(lua_State *luaVm);

	// Beauty
	int getAllFaces(lua_State *luaVm);
	int getAllHair(lua_State *luaVm);
	int getAllSkins(lua_State *luaVm);
	int getRandomFace(lua_State *luaVm);
	int getRandomHair(lua_State *luaVm);
	int getRandomSkin(lua_State *luaVm);
	int isValidFace(lua_State *luaVm);
	int isValidHair(lua_State *luaVm);
	int isValidSkin(lua_State *luaVm);

	// Buddy
	int addBuddySlots(lua_State *luaVm);
	int getBuddySlots(lua_State *luaVm);

	// Skill
	int addSkillLevel(lua_State *luaVm);
	int getSkillLevel(lua_State *luaVm);
	int getMaxSkillLevel(lua_State *luaVm);
	int setMaxSkillLevel(lua_State *luaVm);

	// Quest
	int getQuestData(lua_State *luaVm);
	int isQuestActive(lua_State *luaVm);
	int isQuestInactive(lua_State *luaVm);
	int isQuestCompleted(lua_State *luaVm);
	int setQuestData(lua_State *luaVm);

	// Inventory
	int addSlots(lua_State *luaVm);
	int addStorageSlots(lua_State *luaVm);
	int getEquippedItemInSlot(lua_State *luaVm);
	int getItemAmount(lua_State *luaVm);
	int getMesos(lua_State *luaVm);
	int getOpenSlots(lua_State *luaVm);
	int giveItem(lua_State *luaVm);
	int giveMesos(lua_State *luaVm);
	int hasOpenSlotsFor(lua_State *luaVm);
	int isEquippedItem(lua_State *luaVm);
	int useItem(lua_State *luaVm);

	// Player
	int changeMaplePoints(lua_State *luaVm);
	int changeNxCredit(lua_State *luaVm);
	int changeNxPrepaid(lua_State *luaVm);
	int deletePlayerVariable(lua_State *luaVm);
	int endMorph(lua_State *luaVm);
	int getAllianceId(lua_State *luaVm);
	int getAllianceRank(lua_State *luaVm);
	int getAp(lua_State *luaVm);
	int getDex(lua_State *luaVm);
	int getExp(lua_State *luaVm);
	int getEyes(lua_State *luaVm);
	int getFame(lua_State *luaVm);
	int getFh(lua_State *luaVm);
	int getGender(lua_State *luaVm);
	int getGmLevel(lua_State *luaVm);
	int getGuildId(lua_State *luaVm);
	int getGuildRank(lua_State *luaVm);
	int getHair(lua_State *luaVm);
	int getHp(lua_State *luaVm);
	int getHpMpAp(lua_State *luaVm);
	int getId(lua_State *luaVm);
	int getInt(lua_State *luaVm);
	int getJob(lua_State *luaVm);
	int getLevel(lua_State *luaVm);
	int getLuk(lua_State *luaVm);
	int getMap(lua_State *luaVm);
	int getMaplePoints(lua_State *luaVm);
	int getMaxHp(lua_State *luaVm);
	int getMaxMp(lua_State *luaVm);
	int getMp(lua_State *luaVm);
	int getName(lua_State *luaVm);
	int getNxCredit(lua_State *luaVm);
	int getNxPrepaid(lua_State *luaVm);
	int getPlayerVariable(lua_State *luaVm);
	int getPosX(lua_State *luaVm);
	int getPosY(lua_State *luaVm);
	int getRealMaxHp(lua_State *luaVm);
	int getRealMaxMp(lua_State *luaVm);
	int getSkin(lua_State *luaVm);
	int getSp(lua_State *luaVm);
	int getStr(lua_State *luaVm);
	int giveAp(lua_State *luaVm);
	int giveExp(lua_State *luaVm);
	int giveFame(lua_State *luaVm);
	int giveSp(lua_State *luaVm);
	int isActiveItem(lua_State *luaVm);
	int isActiveSkill(lua_State *luaVm);
	int isGm(lua_State *luaVm);
	int isOnline(lua_State *luaVm);
	int revertPlayer(lua_State *luaVm);
	int setAp(lua_State *luaVm);
	int setDex(lua_State *luaVm);
	int setExp(lua_State *luaVm);
	int setHp(lua_State *luaVm);
	int setInt(lua_State *luaVm);
	int setJob(lua_State *luaVm);
	int setLevel(lua_State *luaVm);
	int setLuk(lua_State *luaVm);
	int setMap(lua_State *luaVm);
	int setMaxHp(lua_State *luaVm);
	int setMaxMp(lua_State *luaVm);
	int setMp(lua_State *luaVm);
	int setPlayer(lua_State *luaVm);
	int setPlayerVariable(lua_State *luaVm);
	int setSp(lua_State *luaVm);
	int setStr(lua_State *luaVm);
	int setStyle(lua_State *luaVm);
	int showInstructionBubble(lua_State *luaVm);
	int showMessage(lua_State *luaVm);

	// Effects
	int playFieldSound(lua_State *luaVm);
	int playMinigameSound(lua_State *luaVm);
	int setMusic(lua_State *luaVm);
	int showMapEffect(lua_State *luaVm);
	int showMapEvent(lua_State *luaVm);

	// Map
	int clearDrops(lua_State *luaVm);
	int clearMobs(lua_State *luaVm);
	int countMobs(lua_State *luaVm);
	int getAllMapPlayerIds(lua_State *luaVm);
	int getNumPlayers(lua_State *luaVm);
	int getReactorState(lua_State *luaVm);
	int killMobs(lua_State *luaVm);
	int setMapSpawn(lua_State *luaVm);
	int setReactorState(lua_State *luaVm);
	int showMapMessage(lua_State *luaVm);
	int showMapTimer(lua_State *luaVm);
	int spawnMob(lua_State *luaVm);
	int spawnMobPos(lua_State *luaVm);

	// Mob
	int getMobFh(lua_State *luaVm);
	int getMobHp(lua_State *luaVm);
	int getMobMaxHp(lua_State *luaVm);
	int getMobMaxMp(lua_State *luaVm);
	int getMobMp(lua_State *luaVm);
	int getMobPosX(lua_State *luaVm);
	int getMobPosY(lua_State *luaVm);
	int getRealMobId(lua_State *luaVm);
	int killMob(lua_State *luaVm);
	int mobDropItem(lua_State *luaVm);

	// Time
	int getDate(lua_State *luaVm);
	int getDay(lua_State *luaVm);
	int getHour(lua_State *luaVm);
	int getMinute(lua_State *luaVm);
	int getMonth(lua_State *luaVm);
	int getNearestMinute(lua_State *luaVm);
	int getSecond(lua_State *luaVm);
	int getTime(lua_State *luaVm);
	int getTimeZoneOffset(lua_State *luaVm);
	int getWeek(lua_State *luaVm);
	int getYear(lua_State *luaVm);
	int getTickCount(lua_State *luaVm);
	int getSecondsSinceEpoch(lua_State *luaVm);
	int isDst(lua_State *luaVm);

	// Rates
	int getDropRate(lua_State *luaVm);
	int getExpRate(lua_State *luaVm);
	int getMesoRate(lua_State *luaVm);
	int getQuestExpRate(lua_State *luaVm);

	// Party
	int checkPartyFootholds(lua_State *luaVm);
	int getAllPartyPlayerIds(lua_State *luaVm);
	int getPartyCount(lua_State *luaVm);
	int getPartyId(lua_State *luaVm);
	int getPartyMapCount(lua_State *luaVm);
	int isPartyInLevelRange(lua_State *luaVm);
	int isPartyLeader(lua_State *luaVm);
	int verifyPartyFootholds(lua_State *luaVm);
	int warpParty(lua_State *luaVm);

	// Instance
	int addInstanceMap(lua_State *luaVm);
	int addInstanceParty(lua_State *luaVm);
	int addInstancePlayer(lua_State *luaVm);
	int addPlayerSignUp(lua_State *luaVm);
	int banInstancePlayer(lua_State *luaVm);
	int checkInstanceTimer(lua_State *luaVm);
	int createInstance(lua_State *luaVm);
	int deleteInstanceVariable(lua_State *luaVm);
	int getAllInstancePlayerIds(lua_State *luaVm);
	int getBannedInstancePlayerByIndex(lua_State *luaVm);
	int getBannedInstancePlayerCount(lua_State *luaVm);
	int getInstanceMax(lua_State *luaVm);
	int getInstancePlayerByIndex(lua_State *luaVm);
	int getInstancePlayerCount(lua_State *luaVm);
	int getInstancePlayerId(lua_State *luaVm);
	int getInstanceSignupCount(lua_State *luaVm);
	int getInstanceTime(lua_State *luaVm);
	int getInstanceVariable(lua_State *luaVm);
	int isBannedInstancePlayer(lua_State *luaVm);
	int isInstance(lua_State *luaVm);
	int isInstanceMap(lua_State *luaVm);
	int isInstancePersistent(lua_State *luaVm);
	int isPlayerSignedUp(lua_State *luaVm);
	int markForDelete(lua_State *luaVm);
	int moveAllPlayers(lua_State *luaVm);
	int passPlayersBetweenInstances(lua_State *luaVm);
	int removeAllInstancePlayers(lua_State *luaVm);
	int removeInstancePlayer(lua_State *luaVm);
	int removePlayerSignUp(lua_State *luaVm);
	int respawnInstanceMobs(lua_State *luaVm);
	int respawnInstanceReactors(lua_State *luaVm);
	int revertInstance(lua_State *luaVm);
	int setInstance(lua_State *luaVm);
	int setInstanceMax(lua_State *luaVm);
	int setInstancePersistence(lua_State *luaVm);
	int setInstanceReset(lua_State *luaVm);
	int setInstanceTime(lua_State *luaVm);
	int setInstanceVariable(lua_State *luaVm);
	int showInstanceTime(lua_State *luaVm);
	int startInstanceTimer(lua_State *luaVm);
	int stopAllInstanceTimers(lua_State *luaVm);
	int stopInstanceTimer(lua_State *luaVm);
	int unbanInstancePlayer(lua_State *luaVm);

	// Guild
	int addGuildPoint(lua_State *luaVm);
	int disbandGuild(lua_State *luaVm);
	int displayGuildRankBoard(lua_State *luaVm);
	int getGuildCapacity(lua_State *luaVm);
	int hasEmblem(lua_State *luaVm);
	int increaseGuildCapacity(lua_State *luaVm);
	int removeEmblem(lua_State *luaVm);
	int sendNewGuildWindow(lua_State *luaVm);
	int sendChangeGuildEmblem(lua_State *luaVm);

	// Alliance
	int createAlliance(lua_State *luaVm);
	int disbandAlliance(lua_State *luaVm);
	int getAllianceCapacity(lua_State *luaVm);
	int increaseAllianceCapacity(lua_State *luaVm);
};
