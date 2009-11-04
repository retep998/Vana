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
	Instance * getInstance(lua_State *luaVm);

	// Global exports

	// Miscellaneous
	int getRandomNumber(lua_State *luaVm);
	int consoleOutput(lua_State *luaVm);

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
	int removeNPC(lua_State *luaVm);
	int runNPC(lua_State *luaVm);
	int showShop(lua_State *luaVm);
	int spawnNPC(lua_State *luaVm);

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
	int deletePlayerVariable(lua_State *luaVm);
	int endMorph(lua_State *luaVm);
	int getAllianceId(lua_State *luaVm);
	int getAllianceRank(lua_State *luaVm);
	int getAP(lua_State *luaVm);
	int getDEX(lua_State *luaVm);
	int getEXP(lua_State *luaVm);
	int getEyes(lua_State *luaVm);
	int getFame(lua_State *luaVm);
	int getFH(lua_State *luaVm);
	int getGender(lua_State *luaVm);
	int getGMLevel(lua_State *luaVm);
	int getGuildId(lua_State *luaVm);
	int getGuildRank(lua_State *luaVm);
	int getHair(lua_State *luaVm);
	int getHP(lua_State *luaVm);
	int getHPMPAP(lua_State *luaVm);
	int getID(lua_State *luaVm);
	int getINT(lua_State *luaVm);
	int getJob(lua_State *luaVm);
	int getLevel(lua_State *luaVm);
	int getLUK(lua_State *luaVm);
	int getMap(lua_State *luaVm);
	int getMHP(lua_State *luaVm);
	int getMMP(lua_State *luaVm);
	int getMP(lua_State *luaVm);
	int getName(lua_State *luaVm);
	int getPlayerVariable(lua_State *luaVm);
	int getPosX(lua_State *luaVm);
	int getPosY(lua_State *luaVm);
	int getRMHP(lua_State *luaVm);
	int getRMMP(lua_State *luaVm);
	int getSkin(lua_State *luaVm);
	int getSP(lua_State *luaVm);
	int getSTR(lua_State *luaVm);
	int giveAP(lua_State *luaVm);
	int giveEXP(lua_State *luaVm);
	int giveFame(lua_State *luaVm);
	int giveSP(lua_State *luaVm);
	int isActiveItem(lua_State *luaVm);
	int isActiveSkill(lua_State *luaVm);
	int isGM(lua_State *luaVm);
	int isOnline(lua_State *luaVm);
	int playSoundPlayer(lua_State *luaVm);
	int revertPlayer(lua_State *luaVm);
	int setAP(lua_State *luaVm);
	int setDEX(lua_State *luaVm);
	int setEXP(lua_State *luaVm);
	int setHP(lua_State *luaVm);
	int setINT(lua_State *luaVm);
	int setJob(lua_State *luaVm);
	int setLevel(lua_State *luaVm);
	int setLUK(lua_State *luaVm);
	int setMap(lua_State *luaVm);
	int setMHP(lua_State *luaVm);
	int setMMP(lua_State *luaVm);
	int setMP(lua_State *luaVm);
	int setPlayer(lua_State *luaVm);
	int setPlayerVariable(lua_State *luaVm);
	int setRMHP(lua_State *luaVm);
	int setRMMP(lua_State *luaVm);
	int setSP(lua_State *luaVm);
	int setSTR(lua_State *luaVm);
	int setStyle(lua_State *luaVm);
	int showInstructionBubble(lua_State *luaVm);
	int showMessage(lua_State *luaVm);

	// Map
	int clearDrops(lua_State *luaVm);
	int clearMobs(lua_State *luaVm);
	int countMobs(lua_State *luaVm);
	int getAllMapPlayerIDs(lua_State *luaVm);
	int getNumPlayers(lua_State *luaVm);
	int getReactorState(lua_State *luaVm);
	int killMobs(lua_State *luaVm);
	int playSoundMap(lua_State *luaVm);
	int setMapSpawn(lua_State *luaVm);
	int setMusic(lua_State *luaVm);
	int setReactorState(lua_State *luaVm);
	int showMapEffect(lua_State *luaVm);
	int showMapEvent(lua_State *luaVm);
	int showMapMessage(lua_State *luaVm);
	int showMapTimer(lua_State *luaVm);
	int spawnMob(lua_State *luaVm);
	int spawnMobPos(lua_State *luaVm);

	// Mob
	int getMobFH(lua_State *luaVm);
	int getMobHP(lua_State *luaVm);
	int getMobMHP(lua_State *luaVm);
	int getMobMMP(lua_State *luaVm);
	int getMobMP(lua_State *luaVm);
	int getMobPosX(lua_State *luaVm);
	int getMobPosY(lua_State *luaVm);
	int getRealMobID(lua_State *luaVm);
	int killMob(lua_State *luaVm);
	int mobDropItem(lua_State *luaVm);

	// Time
	int getDate(lua_State *luaVm);
	int getDay(lua_State *luaVm);
	int getDST(lua_State *luaVm);
	int getHour(lua_State *luaVm);
	int getMinute(lua_State *luaVm);
	int getMonth(lua_State *luaVm);
	int getNearestMinute(lua_State *luaVm);
	int getSecond(lua_State *luaVm);
	int getTime(lua_State *luaVm);
	int getTimeZoneOffset(lua_State *luaVm);
	int getWeek(lua_State *luaVm);
	int getYear(lua_State *luaVm);

	// Rates
	int getDropRate(lua_State *luaVm);
	int getEXPRate(lua_State *luaVm);
	int getMesoRate(lua_State *luaVm);
	int getQuestEXPRate(lua_State *luaVm);

	// Party
	int checkPartyFootholds(lua_State *luaVm);
	int getAllPartyPlayerIDs(lua_State *luaVm);
	int getPartyCount(lua_State *luaVm);
	int getPartyID(lua_State *luaVm);
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
	int getAllInstancePlayerIDs(lua_State *luaVm);
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
