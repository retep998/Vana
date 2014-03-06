/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "LuaEnvironment.hpp"
#include "Types.hpp"
#include <string>

class Instance;
class Player;

class LuaScriptable : public LuaEnvironment {
	NONCOPYABLE(LuaScriptable);
	NO_DEFAULT_CONSTRUCTOR(LuaScriptable);
protected:
	LuaScriptable(const string_t &filename, player_id_t playerId);
	LuaScriptable(const string_t &filename, player_id_t playerId, bool useThread);

	auto handleError(const string_t &filename, const string_t &error) -> void override;
	player_id_t m_playerId = -1;
private:
	auto initialize() -> void;
	auto setEnvironmentVariables() -> void;
};

namespace LuaExports {
	auto getEnvironment(lua_State *luaVm) -> LuaEnvironment &;
	auto getPlayer(lua_State *luaVm) -> Player *;
	auto getPlayerDeduced(int parameter, lua_State *luaVm) -> Player *;
	auto getInstance(lua_State *luaVm) -> Instance *;
	auto obtainSetVariablePair(lua_State *luaVm, LuaEnvironment &env) -> pair_t<string_t, string_t>;
	auto pushGetVariableData(lua_State *luaVm, const string_t &value, bool integralReturn) -> void;
	auto isBossChannel(lua_State *luaVm, const vector_t<channel_id_t> &channels) -> int;

	// Global exports

	// Miscellaneous
	auto consoleOutput(lua_State *luaVm) -> int;
	auto getRandomNumber(lua_State *luaVm) -> int;
	auto log(lua_State *luaVm) -> int;
	auto showGlobalMessage(lua_State *luaVm) -> int;
	auto showWorldMessage(lua_State *luaVm) -> int;
	auto testExport(lua_State *luaVm) -> int;

	// Channel
	auto deleteChannelVariable(lua_State *luaVm) -> int;
	auto getChannel(lua_State *luaVm) -> int;
	auto getChannelVariable(lua_State *luaVm) -> int;
	auto getWorld(lua_State *luaVm) -> int;
	auto isHorntailChannel(lua_State *luaVm) -> int;
	auto isPapChannel(lua_State *luaVm) -> int;
	auto isPianusChannel(lua_State *luaVm) -> int;
	auto isPinkBeanChannel(lua_State *luaVm) -> int;
	auto isZakumChannel(lua_State *luaVm) -> int;
	auto setChannelVariable(lua_State *luaVm) -> int;
	auto showChannelMessage(lua_State *luaVm) -> int;

	// Bosses
	auto getHorntailChannels(lua_State *luaVm) -> int;
	auto getMaxHorntailBattles(lua_State *luaVm) -> int;
	auto getMaxPapBattles(lua_State *luaVm) -> int;
	auto getMaxPianusBattles(lua_State *luaVm) -> int;
	auto getMaxPinkBeanBattles(lua_State *luaVm) -> int;
	auto getMaxZakumBattles(lua_State *luaVm) -> int;
	auto getPapChannels(lua_State *luaVm) -> int;
	auto getPianusChannels(lua_State *luaVm) -> int;
	auto getPinkBeanChannels(lua_State *luaVm) -> int;
	auto getZakumChannels(lua_State *luaVm) -> int;

	// NPC
	auto isBusy(lua_State *luaVm) -> int;
	auto removeNpc(lua_State *luaVm) -> int;
	auto runNpc(lua_State *luaVm) -> int;
	auto showShop(lua_State *luaVm) -> int;
	auto spawnNpc(lua_State *luaVm) -> int;

	// Beauty
	auto getAllFaces(lua_State *luaVm) -> int;
	auto getAllHair(lua_State *luaVm) -> int;
	auto getAllSkins(lua_State *luaVm) -> int;
	auto getRandomFace(lua_State *luaVm) -> int;
	auto getRandomHair(lua_State *luaVm) -> int;
	auto getRandomSkin(lua_State *luaVm) -> int;
	auto isValidFace(lua_State *luaVm) -> int;
	auto isValidHair(lua_State *luaVm) -> int;
	auto isValidSkin(lua_State *luaVm) -> int;

	// Buddy
	auto addBuddySlots(lua_State *luaVm) -> int;
	auto getBuddySlots(lua_State *luaVm) -> int;

	// Skill
	auto addSkillLevel(lua_State *luaVm) -> int;
	auto getSkillLevel(lua_State *luaVm) -> int;
	auto getMaxSkillLevel(lua_State *luaVm) -> int;
	auto setMaxSkillLevel(lua_State *luaVm) -> int;

	// Quest
	auto getQuestData(lua_State *luaVm) -> int;
	auto isQuestActive(lua_State *luaVm) -> int;
	auto isQuestInactive(lua_State *luaVm) -> int;
	auto isQuestCompleted(lua_State *luaVm) -> int;
	auto setQuestData(lua_State *luaVm) -> int;

	// Inventory
	auto addSlots(lua_State *luaVm) -> int;
	auto addStorageSlots(lua_State *luaVm) -> int;
	auto destroyEquippedItem(lua_State *luaVm) -> int;
	auto getEquippedItemInSlot(lua_State *luaVm) -> int;
	auto getItemAmount(lua_State *luaVm) -> int;
	auto getMesos(lua_State *luaVm) -> int;
	auto getOpenSlots(lua_State *luaVm) -> int;
	auto giveItem(lua_State *luaVm) -> int;
	auto giveMesos(lua_State *luaVm) -> int;
	auto hasOpenSlotsFor(lua_State *luaVm) -> int;
	auto isEquippedItem(lua_State *luaVm) -> int;
	auto useItem(lua_State *luaVm) -> int;

	// Player
	auto deletePlayerVariable(lua_State *luaVm) -> int;
	auto endMorph(lua_State *luaVm) -> int;
	auto getAp(lua_State *luaVm) -> int;
	auto getDex(lua_State *luaVm) -> int;
	auto getExp(lua_State *luaVm) -> int;
	auto getEyes(lua_State *luaVm) -> int;
	auto getFame(lua_State *luaVm) -> int;
	auto getFh(lua_State *luaVm) -> int;
	auto getGender(lua_State *luaVm) -> int;
	auto getGmLevel(lua_State *luaVm) -> int;
	auto getHair(lua_State *luaVm) -> int;
	auto getHp(lua_State *luaVm) -> int;
	auto getHpMpAp(lua_State *luaVm) -> int;
	auto getId(lua_State *luaVm) -> int;
	auto getInt(lua_State *luaVm) -> int;
	auto getJob(lua_State *luaVm) -> int;
	auto getLevel(lua_State *luaVm) -> int;
	auto getLuk(lua_State *luaVm) -> int;
	auto getMap(lua_State *luaVm) -> int;
	auto getMaxHp(lua_State *luaVm) -> int;
	auto getMaxMp(lua_State *luaVm) -> int;
	auto getMp(lua_State *luaVm) -> int;
	auto getName(lua_State *luaVm) -> int;
	auto getPlayerVariable(lua_State *luaVm) -> int;
	auto getPosX(lua_State *luaVm) -> int;
	auto getPosY(lua_State *luaVm) -> int;
	auto getRealMaxHp(lua_State *luaVm) -> int;
	auto getRealMaxMp(lua_State *luaVm) -> int;
	auto getSkin(lua_State *luaVm) -> int;
	auto getSp(lua_State *luaVm) -> int;
	auto getStr(lua_State *luaVm) -> int;
	auto giveAp(lua_State *luaVm) -> int;
	auto giveExp(lua_State *luaVm) -> int;
	auto giveFame(lua_State *luaVm) -> int;
	auto giveSp(lua_State *luaVm) -> int;
	auto isActiveItem(lua_State *luaVm) -> int;
	auto isActiveSkill(lua_State *luaVm) -> int;
	auto isGm(lua_State *luaVm) -> int;
	auto isOnline(lua_State *luaVm) -> int;
	auto revertPlayer(lua_State *luaVm) -> int;
	auto setAp(lua_State *luaVm) -> int;
	auto setDex(lua_State *luaVm) -> int;
	auto setExp(lua_State *luaVm) -> int;
	auto setHp(lua_State *luaVm) -> int;
	auto setInt(lua_State *luaVm) -> int;
	auto setJob(lua_State *luaVm) -> int;
	auto setLevel(lua_State *luaVm) -> int;
	auto setLuk(lua_State *luaVm) -> int;
	auto setMap(lua_State *luaVm) -> int;
	auto setMaxHp(lua_State *luaVm) -> int;
	auto setMaxMp(lua_State *luaVm) -> int;
	auto setMp(lua_State *luaVm) -> int;
	auto setPlayer(lua_State *luaVm) -> int;
	auto setPlayerVariable(lua_State *luaVm) -> int;
	auto setSp(lua_State *luaVm) -> int;
	auto setStr(lua_State *luaVm) -> int;
	auto setStyle(lua_State *luaVm) -> int;
	auto showInstructionBubble(lua_State *luaVm) -> int;
	auto showMessage(lua_State *luaVm) -> int;

	// Effects
	auto playFieldSound(lua_State *luaVm) -> int;
	auto playMinigameSound(lua_State *luaVm) -> int;
	auto setMusic(lua_State *luaVm) -> int;
	auto showMapEffect(lua_State *luaVm) -> int;
	auto showMapEvent(lua_State *luaVm) -> int;

	// Map
	auto clearDrops(lua_State *luaVm) -> int;
	auto clearMobs(lua_State *luaVm) -> int;
	auto countMobs(lua_State *luaVm) -> int;
	auto getAllMapPlayerIds(lua_State *luaVm) -> int;
	auto getNumPlayers(lua_State *luaVm) -> int;
	auto getReactorState(lua_State *luaVm) -> int;
	auto killMobs(lua_State *luaVm) -> int;
	auto setBoatDocked(lua_State *luaVm) -> int;
	auto setMapSpawn(lua_State *luaVm) -> int;
	auto setReactorState(lua_State *luaVm) -> int;
	auto showMapMessage(lua_State *luaVm) -> int;
	auto showMapTimer(lua_State *luaVm) -> int;
	auto spawnMob(lua_State *luaVm) -> int;
	auto spawnMobPos(lua_State *luaVm) -> int;

	// Mob
	auto getMobFh(lua_State *luaVm) -> int;
	auto getMobHp(lua_State *luaVm) -> int;
	auto getMobMaxHp(lua_State *luaVm) -> int;
	auto getMobMaxMp(lua_State *luaVm) -> int;
	auto getMobMp(lua_State *luaVm) -> int;
	auto getMobPosX(lua_State *luaVm) -> int;
	auto getMobPosY(lua_State *luaVm) -> int;
	auto getRealMobId(lua_State *luaVm) -> int;
	auto killMob(lua_State *luaVm) -> int;
	auto mobDropItem(lua_State *luaVm) -> int;

	// Time
	auto getDate(lua_State *luaVm) -> int;
	auto getDay(lua_State *luaVm) -> int;
	auto getHour(lua_State *luaVm) -> int;
	auto getMinute(lua_State *luaVm) -> int;
	auto getMonth(lua_State *luaVm) -> int;
	auto getNearestMinute(lua_State *luaVm) -> int;
	auto getSecond(lua_State *luaVm) -> int;
	auto getTime(lua_State *luaVm) -> int;
	auto getTimeZoneOffset(lua_State *luaVm) -> int;
	auto getWeek(lua_State *luaVm) -> int;
	auto getYear(lua_State *luaVm) -> int;
	auto isDst(lua_State *luaVm) -> int;

	// Rates
	auto getDropRate(lua_State *luaVm) -> int;
	auto getExpRate(lua_State *luaVm) -> int;
	auto getMesoRate(lua_State *luaVm) -> int;
	auto getQuestExpRate(lua_State *luaVm) -> int;

	// Party
	auto checkPartyFootholds(lua_State *luaVm) -> int;
	auto getAllPartyPlayerIds(lua_State *luaVm) -> int;
	auto getPartyCount(lua_State *luaVm) -> int;
	auto getPartyId(lua_State *luaVm) -> int;
	auto getPartyMapCount(lua_State *luaVm) -> int;
	auto isPartyInLevelRange(lua_State *luaVm) -> int;
	auto isPartyLeader(lua_State *luaVm) -> int;
	auto verifyPartyFootholds(lua_State *luaVm) -> int;
	auto warpParty(lua_State *luaVm) -> int;

	// Instance
	auto addInstanceMap(lua_State *luaVm) -> int;
	auto addInstanceParty(lua_State *luaVm) -> int;
	auto addInstancePlayer(lua_State *luaVm) -> int;
	auto addPlayerSignUp(lua_State *luaVm) -> int;
	auto banInstancePlayer(lua_State *luaVm) -> int;
	auto checkInstanceTimer(lua_State *luaVm) -> int;
	auto createInstance(lua_State *luaVm) -> int;
	auto deleteInstanceVariable(lua_State *luaVm) -> int;
	auto getAllInstancePlayerIds(lua_State *luaVm) -> int;
	auto getBannedInstancePlayerByIndex(lua_State *luaVm) -> int;
	auto getBannedInstancePlayerCount(lua_State *luaVm) -> int;
	auto getInstanceMax(lua_State *luaVm) -> int;
	auto getInstancePlayerByIndex(lua_State *luaVm) -> int;
	auto getInstancePlayerCount(lua_State *luaVm) -> int;
	auto getInstancePlayerId(lua_State *luaVm) -> int;
	auto getInstanceSignupCount(lua_State *luaVm) -> int;
	auto getInstanceTime(lua_State *luaVm) -> int;
	auto getInstanceVariable(lua_State *luaVm) -> int;
	auto isBannedInstancePlayer(lua_State *luaVm) -> int;
	auto isInstance(lua_State *luaVm) -> int;
	auto isInstanceMap(lua_State *luaVm) -> int;
	auto isInstancePersistent(lua_State *luaVm) -> int;
	auto isPlayerSignedUp(lua_State *luaVm) -> int;
	auto markForDelete(lua_State *luaVm) -> int;
	auto moveAllPlayers(lua_State *luaVm) -> int;
	auto passPlayersBetweenInstances(lua_State *luaVm) -> int;
	auto removeAllInstancePlayers(lua_State *luaVm) -> int;
	auto removeInstancePlayer(lua_State *luaVm) -> int;
	auto removePlayerSignUp(lua_State *luaVm) -> int;
	auto respawnInstanceMobs(lua_State *luaVm) -> int;
	auto respawnInstanceReactors(lua_State *luaVm) -> int;
	auto revertInstance(lua_State *luaVm) -> int;
	auto setInstance(lua_State *luaVm) -> int;
	auto setInstanceMax(lua_State *luaVm) -> int;
	auto setInstancePersistence(lua_State *luaVm) -> int;
	auto setInstanceReset(lua_State *luaVm) -> int;
	auto setInstanceTime(lua_State *luaVm) -> int;
	auto setInstanceVariable(lua_State *luaVm) -> int;
	auto showInstanceTime(lua_State *luaVm) -> int;
	auto startInstanceTimer(lua_State *luaVm) -> int;
	auto stopAllInstanceTimers(lua_State *luaVm) -> int;
	auto stopInstanceTimer(lua_State *luaVm) -> int;
	auto unbanInstancePlayer(lua_State *luaVm) -> int;
}