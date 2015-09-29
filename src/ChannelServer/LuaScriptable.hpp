/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Common/LuaEnvironment.hpp"
#include "Common/Types.hpp"
#include <string>

namespace Vana {
	namespace ChannelServer {
		class Instance;
		class Player;

		namespace VariableType {
			enum Type : int32_t {
				Bool,
				String,
				Number,
				Integer,
			};
		}

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
			// TODO FIXME msvc
			// Remove this when MSVC supports static init
			static string_t sApiVersion/* = "1.0.0"*/;
		};

		namespace LuaExports {
			auto getEnvironment(lua_State *luaVm) -> LuaEnvironment &;
			auto getPlayer(lua_State *luaVm, LuaEnvironment &env) -> Player *;
			auto getPlayerDeduced(int parameter, lua_State *luaVm, LuaEnvironment &env) -> Player *;
			auto getInstance(lua_State *luaVm, LuaEnvironment &env) -> Instance *;
			auto obtainSetVariablePair(lua_State *luaVm, LuaEnvironment &env) -> pair_t<string_t, string_t>;
			auto pushGetVariableData(lua_State *luaVm, LuaEnvironment &env, const string_t &value, VariableType::Type returnType) -> lua_return_t;
			auto isBossChannel(lua_State *luaVm, const vector_t<channel_id_t> &channels) -> lua_return_t;

			// Global exports

			// Miscellaneous
			auto consoleOutput(lua_State *luaVm) -> lua_return_t;
			auto getRandomNumber(lua_State *luaVm) -> lua_return_t;
			auto log(lua_State *luaVm) -> lua_return_t;
			auto selectDiscrete(lua_State *luaVm) -> lua_return_t;
			auto showGlobalMessage(lua_State *luaVm) -> lua_return_t;
			auto showWorldMessage(lua_State *luaVm) -> lua_return_t;
			auto testExport(lua_State *luaVm) -> lua_return_t;

			// Channel
			auto deleteChannelVariable(lua_State *luaVm) -> lua_return_t;
			auto getChannel(lua_State *luaVm) -> lua_return_t;
			auto getChannelVariable(lua_State *luaVm) -> lua_return_t;
			auto getWorld(lua_State *luaVm) -> lua_return_t;
			auto setChannelVariable(lua_State *luaVm) -> lua_return_t;
			auto showChannelMessage(lua_State *luaVm) -> lua_return_t;

			// Bosses
			auto getHorntailChannels(lua_State *luaVm) -> lua_return_t;
			auto getMaxHorntailBattles(lua_State *luaVm) -> lua_return_t;
			auto getMaxPapulatusBattles(lua_State *luaVm) -> lua_return_t;
			auto getMaxPianusBattles(lua_State *luaVm) -> lua_return_t;
			auto getMaxPinkBeanBattles(lua_State *luaVm) -> lua_return_t;
			auto getMaxZakumBattles(lua_State *luaVm) -> lua_return_t;
			auto getPapulatusChannels(lua_State *luaVm) -> lua_return_t;
			auto getPianusChannels(lua_State *luaVm) -> lua_return_t;
			auto getPinkBeanChannels(lua_State *luaVm) -> lua_return_t;
			auto getZakumChannels(lua_State *luaVm) -> lua_return_t;
			auto isHorntailChannel(lua_State *luaVm) -> lua_return_t;
			auto isPapulatusChannel(lua_State *luaVm) -> lua_return_t;
			auto isPianusChannel(lua_State *luaVm) -> lua_return_t;
			auto isPinkBeanChannel(lua_State *luaVm) -> lua_return_t;
			auto isZakumChannel(lua_State *luaVm) -> lua_return_t;

			// NPC
			auto isBusy(lua_State *luaVm) -> lua_return_t;
			auto removeNpc(lua_State *luaVm) -> lua_return_t;
			auto runNpc(lua_State *luaVm) -> lua_return_t;
			auto showShop(lua_State *luaVm) -> lua_return_t;
			auto spawnNpc(lua_State *luaVm) -> lua_return_t;

			// Beauty
			auto getAllFaces(lua_State *luaVm) -> lua_return_t;
			auto getAllHairs(lua_State *luaVm) -> lua_return_t;
			auto getAllSkins(lua_State *luaVm) -> lua_return_t;
			auto getRandomFace(lua_State *luaVm) -> lua_return_t;
			auto getRandomHair(lua_State *luaVm) -> lua_return_t;
			auto getRandomSkin(lua_State *luaVm) -> lua_return_t;
			auto isValidFace(lua_State *luaVm) -> lua_return_t;
			auto isValidHair(lua_State *luaVm) -> lua_return_t;
			auto isValidSkin(lua_State *luaVm) -> lua_return_t;

			// Buddy
			auto addBuddySlots(lua_State *luaVm) -> lua_return_t;
			auto getBuddySlots(lua_State *luaVm) -> lua_return_t;

			// Skill
			auto addSkillLevel(lua_State *luaVm) -> lua_return_t;
			auto getSkillLevel(lua_State *luaVm) -> lua_return_t;
			auto getMaxSkillLevel(lua_State *luaVm) -> lua_return_t;
			auto setMaxSkillLevel(lua_State *luaVm) -> lua_return_t;

			// Quest
			auto getQuestData(lua_State *luaVm) -> lua_return_t;
			auto isQuestActive(lua_State *luaVm) -> lua_return_t;
			auto isQuestInactive(lua_State *luaVm) -> lua_return_t;
			auto isQuestCompleted(lua_State *luaVm) -> lua_return_t;
			auto setQuestData(lua_State *luaVm) -> lua_return_t;

			// Inventory
			auto addSlots(lua_State *luaVm) -> lua_return_t;
			auto addStorageSlots(lua_State *luaVm) -> lua_return_t;
			auto destroyEquippedItem(lua_State *luaVm) -> lua_return_t;
			auto getEquippedItemInSlot(lua_State *luaVm) -> lua_return_t;
			auto getItemAmount(lua_State *luaVm) -> lua_return_t;
			auto getMaxStackSize(lua_State *luaVm) -> lua_return_t;
			auto getMesos(lua_State *luaVm) -> lua_return_t;
			auto getOpenSlots(lua_State *luaVm) -> lua_return_t;
			auto giveItem(lua_State *luaVm) -> lua_return_t;
			auto giveItemGachapon(lua_State *luaVm) -> lua_return_t;
			auto giveMesos(lua_State *luaVm) -> lua_return_t;
			auto hasOpenSlotsFor(lua_State *luaVm) -> lua_return_t;
			auto isEquippedItem(lua_State *luaVm) -> lua_return_t;
			auto isValidItem(lua_State *luaVm) -> lua_return_t;
			auto useItem(lua_State *luaVm) -> lua_return_t;

			// Player
			auto deletePlayerVariable(lua_State *luaVm) -> lua_return_t;
			auto endMorph(lua_State *luaVm) -> lua_return_t;
			auto getAp(lua_State *luaVm) -> lua_return_t;
			auto getDex(lua_State *luaVm) -> lua_return_t;
			auto getExp(lua_State *luaVm) -> lua_return_t;
			auto getFace(lua_State *luaVm) -> lua_return_t;
			auto getFame(lua_State *luaVm) -> lua_return_t;
			auto getFh(lua_State *luaVm) -> lua_return_t;
			auto getGender(lua_State *luaVm) -> lua_return_t;
			auto getGmLevel(lua_State *luaVm) -> lua_return_t;
			auto getHair(lua_State *luaVm) -> lua_return_t;
			auto getHp(lua_State *luaVm) -> lua_return_t;
			auto getHpMpAp(lua_State *luaVm) -> lua_return_t;
			auto getId(lua_State *luaVm) -> lua_return_t;
			auto getInt(lua_State *luaVm) -> lua_return_t;
			auto getJob(lua_State *luaVm) -> lua_return_t;
			auto getLevel(lua_State *luaVm) -> lua_return_t;
			auto getLuk(lua_State *luaVm) -> lua_return_t;
			auto getMap(lua_State *luaVm) -> lua_return_t;
			auto getMaxHp(lua_State *luaVm) -> lua_return_t;
			auto getMaxMp(lua_State *luaVm) -> lua_return_t;
			auto getMp(lua_State *luaVm) -> lua_return_t;
			auto getName(lua_State *luaVm) -> lua_return_t;
			auto getPlayerVariable(lua_State *luaVm) -> lua_return_t;
			auto getPosX(lua_State *luaVm) -> lua_return_t;
			auto getPosY(lua_State *luaVm) -> lua_return_t;
			auto getRealMaxHp(lua_State *luaVm) -> lua_return_t;
			auto getRealMaxMp(lua_State *luaVm) -> lua_return_t;
			auto getSkin(lua_State *luaVm) -> lua_return_t;
			auto getSp(lua_State *luaVm) -> lua_return_t;
			auto getStr(lua_State *luaVm) -> lua_return_t;
			auto giveAp(lua_State *luaVm) -> lua_return_t;
			auto giveExp(lua_State *luaVm) -> lua_return_t;
			auto giveFame(lua_State *luaVm) -> lua_return_t;
			auto giveSp(lua_State *luaVm) -> lua_return_t;
			auto isActiveItem(lua_State *luaVm) -> lua_return_t;
			auto isActiveSkill(lua_State *luaVm) -> lua_return_t;
			auto isGm(lua_State *luaVm) -> lua_return_t;
			auto isOnline(lua_State *luaVm) -> lua_return_t;
			auto revertPlayer(lua_State *luaVm) -> lua_return_t;
			auto setAp(lua_State *luaVm) -> lua_return_t;
			auto setDex(lua_State *luaVm) -> lua_return_t;
			auto setExp(lua_State *luaVm) -> lua_return_t;
			auto setHp(lua_State *luaVm) -> lua_return_t;
			auto setInt(lua_State *luaVm) -> lua_return_t;
			auto setJob(lua_State *luaVm) -> lua_return_t;
			auto setLevel(lua_State *luaVm) -> lua_return_t;
			auto setLuk(lua_State *luaVm) -> lua_return_t;
			auto setMap(lua_State *luaVm) -> lua_return_t;
			auto setMaxHp(lua_State *luaVm) -> lua_return_t;
			auto setMaxMp(lua_State *luaVm) -> lua_return_t;
			auto setMp(lua_State *luaVm) -> lua_return_t;
			auto setPlayer(lua_State *luaVm) -> lua_return_t;
			auto setPlayerVariable(lua_State *luaVm) -> lua_return_t;
			auto setSp(lua_State *luaVm) -> lua_return_t;
			auto setStr(lua_State *luaVm) -> lua_return_t;
			auto setStyle(lua_State *luaVm) -> lua_return_t;
			auto showInstructionBubble(lua_State *luaVm) -> lua_return_t;
			auto showMessage(lua_State *luaVm) -> lua_return_t;

			// Effects
			auto playFieldSound(lua_State *luaVm) -> lua_return_t;
			auto playMinigameSound(lua_State *luaVm) -> lua_return_t;
			auto setMusic(lua_State *luaVm) -> lua_return_t;
			auto showMapEffect(lua_State *luaVm) -> lua_return_t;
			auto showMapEvent(lua_State *luaVm) -> lua_return_t;

			// Map
			auto clearDrops(lua_State *luaVm) -> lua_return_t;
			auto clearMobs(lua_State *luaVm) -> lua_return_t;
			auto countMobs(lua_State *luaVm) -> lua_return_t;
			auto setPortalState(lua_State *luaVm) -> lua_return_t;
			auto getAllMapPlayerIds(lua_State *luaVm) -> lua_return_t;
			auto getNumPlayers(lua_State *luaVm) -> lua_return_t;
			auto getReactorState(lua_State *luaVm) -> lua_return_t;
			auto killMobs(lua_State *luaVm) -> lua_return_t;
			auto setBoatDocked(lua_State *luaVm) -> lua_return_t;
			auto setMapSpawn(lua_State *luaVm) -> lua_return_t;
			auto setReactorState(lua_State *luaVm) -> lua_return_t;
			auto showMapMessage(lua_State *luaVm) -> lua_return_t;
			auto showMapTimer(lua_State *luaVm) -> lua_return_t;
			auto spawnMob(lua_State *luaVm) -> lua_return_t;
			auto spawnMobPos(lua_State *luaVm) -> lua_return_t;

			// Mob
			auto getMobFh(lua_State *luaVm) -> lua_return_t;
			auto getMobHp(lua_State *luaVm) -> lua_return_t;
			auto getMobMaxHp(lua_State *luaVm) -> lua_return_t;
			auto getMobMaxMp(lua_State *luaVm) -> lua_return_t;
			auto getMobMp(lua_State *luaVm) -> lua_return_t;
			auto getMobPosX(lua_State *luaVm) -> lua_return_t;
			auto getMobPosY(lua_State *luaVm) -> lua_return_t;
			auto getRealMobId(lua_State *luaVm) -> lua_return_t;
			auto killMob(lua_State *luaVm) -> lua_return_t;
			auto mobDropItem(lua_State *luaVm) -> lua_return_t;

			// Time
			auto getDate(lua_State *luaVm) -> lua_return_t;
			auto getDay(lua_State *luaVm) -> lua_return_t;
			auto getHour(lua_State *luaVm) -> lua_return_t;
			auto getMinute(lua_State *luaVm) -> lua_return_t;
			auto getMonth(lua_State *luaVm) -> lua_return_t;
			auto getNearestMinute(lua_State *luaVm) -> lua_return_t;
			auto getSecond(lua_State *luaVm) -> lua_return_t;
			auto getTime(lua_State *luaVm) -> lua_return_t;
			auto getTimeZoneOffset(lua_State *luaVm) -> lua_return_t;
			auto getWeek(lua_State *luaVm) -> lua_return_t;
			auto getYear(lua_State *luaVm) -> lua_return_t;
			auto isDst(lua_State *luaVm) -> lua_return_t;

			// Rates
			auto getDropMeso(lua_State *luaVm) -> lua_return_t;
			auto getDropRate(lua_State *luaVm) -> lua_return_t;
			auto getExpRate(lua_State *luaVm) -> lua_return_t;
			auto getGlobalDropMeso(lua_State *luaVm) -> lua_return_t;
			auto getGlobalDropRate(lua_State *luaVm) -> lua_return_t;
			auto getQuestExpRate(lua_State *luaVm) -> lua_return_t;

			// Party
			auto checkPartyFootholds(lua_State *luaVm) -> lua_return_t;
			auto getAllPartyPlayerIds(lua_State *luaVm) -> lua_return_t;
			auto getPartyCount(lua_State *luaVm) -> lua_return_t;
			auto getPartyId(lua_State *luaVm) -> lua_return_t;
			auto getPartyMapCount(lua_State *luaVm) -> lua_return_t;
			auto isPartyInLevelRange(lua_State *luaVm) -> lua_return_t;
			auto isPartyLeader(lua_State *luaVm) -> lua_return_t;
			auto verifyPartyFootholds(lua_State *luaVm) -> lua_return_t;
			auto warpParty(lua_State *luaVm) -> lua_return_t;

			// Instance
			auto addInstanceMap(lua_State *luaVm) -> lua_return_t;
			auto addInstanceParty(lua_State *luaVm) -> lua_return_t;
			auto addInstancePlayer(lua_State *luaVm) -> lua_return_t;
			auto checkInstanceTimer(lua_State *luaVm) -> lua_return_t;
			auto createInstance(lua_State *luaVm) -> lua_return_t;
			auto deleteInstanceVariable(lua_State *luaVm) -> lua_return_t;
			auto getAllInstancePlayerIds(lua_State *luaVm) -> lua_return_t;
			auto getInstancePlayerCount(lua_State *luaVm) -> lua_return_t;
			auto getInstancePlayerId(lua_State *luaVm) -> lua_return_t;
			auto getInstanceVariable(lua_State *luaVm) -> lua_return_t;
			auto isInstance(lua_State *luaVm) -> lua_return_t;
			auto isInstanceMap(lua_State *luaVm) -> lua_return_t;
			auto isInstancePersistent(lua_State *luaVm) -> lua_return_t;
			auto markForDelete(lua_State *luaVm) -> lua_return_t;
			auto moveAllPlayers(lua_State *luaVm) -> lua_return_t;
			auto passPlayersBetweenInstances(lua_State *luaVm) -> lua_return_t;
			auto removeAllInstancePlayers(lua_State *luaVm) -> lua_return_t;
			auto removeInstancePlayer(lua_State *luaVm) -> lua_return_t;
			auto respawnInstanceMobs(lua_State *luaVm) -> lua_return_t;
			auto respawnInstanceReactors(lua_State *luaVm) -> lua_return_t;
			auto revertInstance(lua_State *luaVm) -> lua_return_t;
			auto setInstance(lua_State *luaVm) -> lua_return_t;
			auto setInstancePersistence(lua_State *luaVm) -> lua_return_t;
			auto setInstanceReset(lua_State *luaVm) -> lua_return_t;
			auto setInstanceVariable(lua_State *luaVm) -> lua_return_t;
			auto showInstanceTime(lua_State *luaVm) -> lua_return_t;
			auto startInstanceFutureTimer(lua_State *luaVm) -> lua_return_t;
			auto startInstanceSecondOfHourTimer(lua_State *luaVm) -> lua_return_t;
			auto stopAllInstanceTimers(lua_State *luaVm) -> lua_return_t;
			auto stopInstanceTimer(lua_State *luaVm) -> lua_return_t;
		}
	}
}