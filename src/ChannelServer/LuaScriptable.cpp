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
#include "LuaScriptable.hpp"
#include "BeautyDataProvider.hpp"
#include "ChannelServer.hpp"
#include "Drop.hpp"
#include "EffectPacket.hpp"
#include "EventDataProvider.hpp"
#include "GameLogicUtilities.hpp"
#include "Instance.hpp"
#include "Instances.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "MapPacket.hpp"
#include "Maps.hpp"
#include "Mob.hpp"
#include "Npc.hpp"
#include "NpcHandler.hpp"
#include "Party.hpp"
#include "PartyHandler.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayersPacket.hpp"
#include "Quests.hpp"
#include "Randomizer.hpp"
#include "Reactor.hpp"
#include "ScriptDataProvider.hpp"
#include "ShopDataProvider.hpp"
#include "StringUtilities.hpp"
#include "TimeUtilities.hpp"
#include <iostream>
#include <vector>

LuaScriptable::LuaScriptable(const string_t &filename, int32_t playerId) :
	m_filename(filename),
	m_playerId(playerId),
	luaVm(luaL_newstate())
{
	initialize();
}

LuaScriptable::~LuaScriptable() {
	lua_close(luaVm);
}

auto LuaScriptable::initialize() -> void {
	luaopen_base(luaVm);
	setVariable("system_playerId", m_playerId); // Pushing ID for reference from static functions
	setEnvironmentVariables();

	Player *player = LuaExports::getPlayer(luaVm);
	if (player != nullptr && player->getInstance() != nullptr) {
		setVariable("system_instanceName", player->getInstance()->getName());
	}

	// Miscellanous
	lua_register(luaVm, "consoleOutput", &LuaExports::consoleOutput);
	lua_register(luaVm, "getRandomNumber", &LuaExports::getRandomNumber);
	lua_register(luaVm, "log", &LuaExports::log);
	lua_register(luaVm, "showGlobalMessage", &LuaExports::showGlobalMessage);
	lua_register(luaVm, "showWorldMessage", &LuaExports::showWorldMessage);

	// Channel
	lua_register(luaVm, "deleteChannelVariable", &LuaExports::deleteChannelVariable);
	lua_register(luaVm, "getChannel", &LuaExports::getChannel);
	lua_register(luaVm, "getChannelVariable", &LuaExports::getChannelVariable);
	lua_register(luaVm, "isHorntailChannel", &LuaExports::isHorntailChannel);
	lua_register(luaVm, "isPapChannel", &LuaExports::isPapChannel);
	lua_register(luaVm, "isPianusChannel", &LuaExports::isPianusChannel);
	lua_register(luaVm, "isPinkBeanChannel", &LuaExports::isPinkBeanChannel);
	lua_register(luaVm, "isZakumChannel", &LuaExports::isZakumChannel);
	lua_register(luaVm, "setChannelVariable", &LuaExports::setChannelVariable);
	lua_register(luaVm, "showChannelMessage", &LuaExports::showChannelMessage);

	// Bosses
	lua_register(luaVm, "getHorntailChannels", &LuaExports::getHorntailChannels);
	lua_register(luaVm, "getMaxHorntailBattles", &LuaExports::getMaxHorntailBattles);
	lua_register(luaVm, "getMaxPapBattles", &LuaExports::getMaxPapBattles);
	lua_register(luaVm, "getMaxPianusBattles", &LuaExports::getMaxPianusBattles);
	lua_register(luaVm, "getMaxPinkBeanBattles", &LuaExports::getMaxPinkBeanBattles);
	lua_register(luaVm, "getMaxZakumBattles", &LuaExports::getMaxZakumBattles);
	lua_register(luaVm, "getPapChannels", &LuaExports::getPapChannels);
	lua_register(luaVm, "getPianusChannels", &LuaExports::getPianusChannels);
	lua_register(luaVm, "getPinkBeanChannels", &LuaExports::getPinkBeanChannels);
	lua_register(luaVm, "getZakumChannels", &LuaExports::getZakumChannels);

	// Npc
	lua_register(luaVm, "isBusy", &LuaExports::isBusy);
	lua_register(luaVm, "removeNpc", &LuaExports::removeNpc);
	lua_register(luaVm, "runNpc", &LuaExports::runNpc);
	lua_register(luaVm, "showShop", &LuaExports::showShop);
	lua_register(luaVm, "spawnNpc", &LuaExports::spawnNpc);

	// Beauty
	lua_register(luaVm, "getAllFaces", &LuaExports::getAllFaces);
	lua_register(luaVm, "getAllHair", &LuaExports::getAllHair);
	lua_register(luaVm, "getAllSkins", &LuaExports::getAllSkins);
	lua_register(luaVm, "getRandomFace", &LuaExports::getRandomFace);
	lua_register(luaVm, "getRandomHair", &LuaExports::getRandomHair);
	lua_register(luaVm, "getRandomSkin", &LuaExports::getRandomSkin);
	lua_register(luaVm, "isValidFace", &LuaExports::isValidFace);
	lua_register(luaVm, "isValidHair", &LuaExports::isValidHair);
	lua_register(luaVm, "isValidSkin", &LuaExports::isValidSkin);

	// Buddy
	lua_register(luaVm, "addBuddySlots", &LuaExports::addBuddySlots);
	lua_register(luaVm, "getBuddySlots", &LuaExports::getBuddySlots);

	// Skill
	lua_register(luaVm, "addSkillLevel", &LuaExports::addSkillLevel);
	lua_register(luaVm, "getSkillLevel", &LuaExports::getSkillLevel);
	lua_register(luaVm, "getMaxSkillLevel", &LuaExports::getMaxSkillLevel);
	lua_register(luaVm, "setMaxSkillLevel", &LuaExports::setMaxSkillLevel);

	// Quest
	lua_register(luaVm, "getQuestData", &LuaExports::getQuestData);
	lua_register(luaVm, "isQuestActive", &LuaExports::isQuestActive);
	lua_register(luaVm, "isQuestInactive", &LuaExports::isQuestInactive);
	lua_register(luaVm, "isQuestCompleted", &LuaExports::isQuestCompleted);
	lua_register(luaVm, "setQuestData", &LuaExports::setQuestData);

	// Inventory
	lua_register(luaVm, "addSlots", &LuaExports::addSlots);
	lua_register(luaVm, "addStorageSlots", &LuaExports::addStorageSlots);
	lua_register(luaVm, "destroyEquippedItem", &LuaExports::destroyEquippedItem);
	lua_register(luaVm, "getEquippedItemInSlot", &LuaExports::getEquippedItemInSlot);
	lua_register(luaVm, "getItemAmount", &LuaExports::getItemAmount);
	lua_register(luaVm, "getMesos", &LuaExports::getMesos);
	lua_register(luaVm, "getOpenSlots", &LuaExports::getOpenSlots);
	lua_register(luaVm, "giveItem", &LuaExports::giveItem);
	lua_register(luaVm, "giveMesos", &LuaExports::giveMesos);
	lua_register(luaVm, "hasOpenSlotsFor", &LuaExports::hasOpenSlotsFor);
	lua_register(luaVm, "isEquippedItem", &LuaExports::isEquippedItem);
	lua_register(luaVm, "useItem", &LuaExports::useItem);

	// Player
	lua_register(luaVm, "deletePlayerVariable", &LuaExports::deletePlayerVariable);
	lua_register(luaVm, "endMorph", &LuaExports::endMorph);
	lua_register(luaVm, "getAp", &LuaExports::getAp);
	lua_register(luaVm, "getDex", &LuaExports::getDex);
	lua_register(luaVm, "getExp", &LuaExports::getExp);
	lua_register(luaVm, "getEyes", &LuaExports::getEyes);
	lua_register(luaVm, "getFame", &LuaExports::getFame);
	lua_register(luaVm, "getFh", &LuaExports::getFh);
	lua_register(luaVm, "getGender", &LuaExports::getGender);
	lua_register(luaVm, "getGmLevel", &LuaExports::getGmLevel);
	lua_register(luaVm, "getHair", &LuaExports::getHair);
	lua_register(luaVm, "getHp", &LuaExports::getHp);
	lua_register(luaVm, "getHpMpAp", &LuaExports::getHpMpAp);
	lua_register(luaVm, "getId", &LuaExports::getId);
	lua_register(luaVm, "getInt", &LuaExports::getInt);
	lua_register(luaVm, "getJob", &LuaExports::getJob);
	lua_register(luaVm, "getLevel", &LuaExports::getLevel);
	lua_register(luaVm, "getLuk", &LuaExports::getLuk);
	lua_register(luaVm, "getMap", &LuaExports::getMap);
	lua_register(luaVm, "getMaxHp", &LuaExports::getMaxHp);
	lua_register(luaVm, "getMaxMp", &LuaExports::getMaxMp);
	lua_register(luaVm, "getMp", &LuaExports::getMp);
	lua_register(luaVm, "getName", &LuaExports::getName);
	lua_register(luaVm, "getPlayerVariable", &LuaExports::getPlayerVariable);
	lua_register(luaVm, "getPosX", &LuaExports::getPosX);
	lua_register(luaVm, "getPosY", &LuaExports::getPosY);
	lua_register(luaVm, "getRealMaxHp", &LuaExports::getRealMaxHp);
	lua_register(luaVm, "getRealMaxMp", &LuaExports::getRealMaxMp);
	lua_register(luaVm, "getSkin", &LuaExports::getSkin);
	lua_register(luaVm, "getSp", &LuaExports::getSp);
	lua_register(luaVm, "getStr", &LuaExports::getStr);
	lua_register(luaVm, "giveAp", &LuaExports::giveAp);
	lua_register(luaVm, "giveExp", &LuaExports::giveExp);
	lua_register(luaVm, "giveFame", &LuaExports::giveFame);
	lua_register(luaVm, "giveSp", &LuaExports::giveSp);
	lua_register(luaVm, "isActiveItem", &LuaExports::isActiveItem);
	lua_register(luaVm, "isActiveSkill", &LuaExports::isActiveSkill);
	lua_register(luaVm, "isGm", &LuaExports::isGm);
	lua_register(luaVm, "isOnline", &LuaExports::isOnline);
	lua_register(luaVm, "revertPlayer", &LuaExports::revertPlayer);
	lua_register(luaVm, "setAp", &LuaExports::setAp);
	lua_register(luaVm, "setDex", &LuaExports::setDex);
	lua_register(luaVm, "setExp", &LuaExports::setExp);
	lua_register(luaVm, "setHp", &LuaExports::setHp);
	lua_register(luaVm, "setInt", &LuaExports::setInt);
	lua_register(luaVm, "setJob", &LuaExports::setJob);
	lua_register(luaVm, "setLevel", &LuaExports::setLevel);
	lua_register(luaVm, "setLuk", &LuaExports::setLuk);
	lua_register(luaVm, "setMap", &LuaExports::setMap);
	lua_register(luaVm, "setMaxHp", &LuaExports::setMaxHp);
	lua_register(luaVm, "setMaxMp", &LuaExports::setMaxMp);
	lua_register(luaVm, "setMp", &LuaExports::setMp);
	lua_register(luaVm, "setPlayer", &LuaExports::setPlayer);
	lua_register(luaVm, "setPlayerVariable", &LuaExports::setPlayerVariable);
	lua_register(luaVm, "setSp", &LuaExports::setSp);
	lua_register(luaVm, "setStr", &LuaExports::setStr);
	lua_register(luaVm, "setStyle", &LuaExports::setStyle);
	lua_register(luaVm, "showInstructionBubble", &LuaExports::showInstructionBubble);
	lua_register(luaVm, "showMessage", &LuaExports::showMessage);

	// Effects
	lua_register(luaVm, "playFieldSound", &LuaExports::playFieldSound);
	lua_register(luaVm, "playMinigameSound", &LuaExports::playMinigameSound);
	lua_register(luaVm, "setMusic", &LuaExports::setMusic);
	lua_register(luaVm, "showMapEffect", &LuaExports::showMapEffect);
	lua_register(luaVm, "showMapEvent", &LuaExports::showMapEvent);

	// Map
	lua_register(luaVm, "clearDrops", &LuaExports::clearDrops);
	lua_register(luaVm, "clearMobs", &LuaExports::clearMobs);
	lua_register(luaVm, "countMobs", &LuaExports::countMobs);
	lua_register(luaVm, "getAllMapPlayerIds", &LuaExports::getAllMapPlayerIds);
	lua_register(luaVm, "getNumPlayers", &LuaExports::getNumPlayers);
	lua_register(luaVm, "getReactorState", &LuaExports::getReactorState);
	lua_register(luaVm, "killMobs", &LuaExports::killMobs);
	lua_register(luaVm, "setMapSpawn", &LuaExports::setMapSpawn);
	lua_register(luaVm, "setReactorState", &LuaExports::setReactorState);
	lua_register(luaVm, "showMapMessage", &LuaExports::showMapMessage);
	lua_register(luaVm, "showMapTimer", &LuaExports::showMapTimer);
	lua_register(luaVm, "spawnMob", &LuaExports::spawnMob);
	lua_register(luaVm, "spawnMobPos", &LuaExports::spawnMobPos);

	// Mob
	lua_register(luaVm, "getMobFh", &LuaExports::getMobFh);
	lua_register(luaVm, "getMobHp", &LuaExports::getMobHp);
	lua_register(luaVm, "getMobMaxHp", &LuaExports::getMobMaxHp);
	lua_register(luaVm, "getMobMaxMp", &LuaExports::getMobMaxMp);
	lua_register(luaVm, "getMobMp", &LuaExports::getMobMp);
	lua_register(luaVm, "getMobPosX", &LuaExports::getMobPosX);
	lua_register(luaVm, "getMobPosY", &LuaExports::getMobPosY);
	lua_register(luaVm, "getRealMobId", &LuaExports::getRealMobId);
	lua_register(luaVm, "killMob", &LuaExports::killMob);
	lua_register(luaVm, "mobDropItem", &LuaExports::mobDropItem);

	// Time
	lua_register(luaVm, "getDate", &LuaExports::getDate);
	lua_register(luaVm, "getDay", &LuaExports::getDay);
	lua_register(luaVm, "getHour", &LuaExports::getHour);
	lua_register(luaVm, "getMinute", &LuaExports::getMinute);
	lua_register(luaVm, "getMonth", &LuaExports::getMonth);
	lua_register(luaVm, "getNearestMinute", &LuaExports::getNearestMinute);
	lua_register(luaVm, "getSecond", &LuaExports::getSecond);
	lua_register(luaVm, "getTime", &LuaExports::getTime);
	lua_register(luaVm, "getTimeZoneOffset", &LuaExports::getTimeZoneOffset);
	lua_register(luaVm, "getWeek", &LuaExports::getWeek);
	lua_register(luaVm, "getYear", &LuaExports::getYear);
	lua_register(luaVm, "isDst", &LuaExports::isDst);

	// Rates
	lua_register(luaVm, "getDropRate", &LuaExports::getDropRate);
	lua_register(luaVm, "getExpRate", &LuaExports::getExpRate);
	lua_register(luaVm, "getMesoRate", &LuaExports::getMesoRate);
	lua_register(luaVm, "getQuestExpRate", &LuaExports::getQuestExpRate);

	// Party
	lua_register(luaVm, "checkPartyFootholds", &LuaExports::checkPartyFootholds);
	lua_register(luaVm, "getAllPartyPlayerIds", &LuaExports::getAllPartyPlayerIds);
	lua_register(luaVm, "getPartyCount", &LuaExports::getPartyCount);
	lua_register(luaVm, "getPartyId", &LuaExports::getPartyId);
	lua_register(luaVm, "getPartyMapCount", &LuaExports::getPartyMapCount);
	lua_register(luaVm, "isPartyInLevelRange", &LuaExports::isPartyInLevelRange);
	lua_register(luaVm, "isPartyLeader", &LuaExports::isPartyLeader);
	lua_register(luaVm, "verifyPartyFootholds", &LuaExports::verifyPartyFootholds);
	lua_register(luaVm, "warpParty", &LuaExports::warpParty);

	// Instance
	lua_register(luaVm, "addInstanceMap", &LuaExports::addInstanceMap);
	lua_register(luaVm, "addInstanceParty", &LuaExports::addInstanceParty);
	lua_register(luaVm, "addInstancePlayer", &LuaExports::addInstancePlayer);
	lua_register(luaVm, "addPlayerSignUp", &LuaExports::addPlayerSignUp);
	lua_register(luaVm, "banInstancePlayer", &LuaExports::banInstancePlayer);
	lua_register(luaVm, "checkInstanceTimer", &LuaExports::checkInstanceTimer);
	lua_register(luaVm, "createInstance", &LuaExports::createInstance);
	lua_register(luaVm, "deleteInstanceVariable", &LuaExports::deleteInstanceVariable);
	lua_register(luaVm, "getAllInstancePlayerIds", &LuaExports::getAllInstancePlayerIds);
	lua_register(luaVm, "getBannedInstancePlayerByIndex", &LuaExports::getBannedInstancePlayerByIndex);
	lua_register(luaVm, "getBannedInstancePlayerCount", &LuaExports::getBannedInstancePlayerCount);
	lua_register(luaVm, "getInstanceMax", &LuaExports::getInstanceMax);
	lua_register(luaVm, "getInstancePlayerByIndex", &LuaExports::getInstancePlayerByIndex);
	lua_register(luaVm, "getInstancePlayerCount", &LuaExports::getInstancePlayerCount);
	lua_register(luaVm, "getInstancePlayerId", &LuaExports::getInstancePlayerId);
	lua_register(luaVm, "getInstanceSignupCount", &LuaExports::getInstanceSignupCount);
	lua_register(luaVm, "getInstanceTime", &LuaExports::getInstanceTime);
	lua_register(luaVm, "getInstanceVariable", &LuaExports::getInstanceVariable);
	lua_register(luaVm, "isBannedInstancePlayer", &LuaExports::isBannedInstancePlayer);
	lua_register(luaVm, "isInstance", &LuaExports::isInstance);
	lua_register(luaVm, "isInstanceMap", &LuaExports::isInstanceMap);
	lua_register(luaVm, "isInstancePersistent", &LuaExports::isInstancePersistent);
	lua_register(luaVm, "isPlayerSignedUp", &LuaExports::isPlayerSignedUp);
	lua_register(luaVm, "markForDelete", &LuaExports::markForDelete);
	lua_register(luaVm, "moveAllPlayers", &LuaExports::moveAllPlayers);
	lua_register(luaVm, "passPlayersBetweenInstances", &LuaExports::passPlayersBetweenInstances);
	lua_register(luaVm, "removeAllInstancePlayers", &LuaExports::removeAllInstancePlayers);
	lua_register(luaVm, "removeInstancePlayer", &LuaExports::removeInstancePlayer);
	lua_register(luaVm, "removePlayerSignUp", &LuaExports::removePlayerSignUp);
	lua_register(luaVm, "respawnInstanceMobs", &LuaExports::respawnInstanceMobs);
	lua_register(luaVm, "respawnInstanceReactors", &LuaExports::respawnInstanceReactors);
	lua_register(luaVm, "revertInstance", &LuaExports::revertInstance);
	lua_register(luaVm, "setInstance", &LuaExports::setInstance);
	lua_register(luaVm, "setInstanceMax", &LuaExports::setInstanceMax);
	lua_register(luaVm, "setInstancePersistence", &LuaExports::setInstancePersistence);
	lua_register(luaVm, "setInstanceReset", &LuaExports::setInstanceReset);
	lua_register(luaVm, "setInstanceTime", &LuaExports::setInstanceTime);
	lua_register(luaVm, "setInstanceVariable", &LuaExports::setInstanceVariable);
	lua_register(luaVm, "showInstanceTime", &LuaExports::showInstanceTime);
	lua_register(luaVm, "startInstanceTimer", &LuaExports::startInstanceTimer);
	lua_register(luaVm, "stopAllInstanceTimers", &LuaExports::stopAllInstanceTimers);
	lua_register(luaVm, "stopInstanceTimer", &LuaExports::stopInstanceTimer);
	lua_register(luaVm, "unbanInstancePlayer", &LuaExports::unbanInstancePlayer);
}

auto LuaScriptable::setEnvironmentVariables() -> void {
	setVariable("env_blueMessage", PlayerPacket::NoticeTypes::Blue);
	setVariable("env_redMessage", PlayerPacket::NoticeTypes::Red);
	setVariable("env_noticeMessage", PlayerPacket::NoticeTypes::Notice);
	setVariable("env_boxMessage", PlayerPacket::NoticeTypes::Box);
	setVariable("env_mapleVersion", MapleVersion::Version);
	setVariable("env_mapleLocale", MapleVersion::Locale);
}

auto LuaScriptable::run() -> bool {
	if (luaL_dofile(luaVm, m_filename.c_str())) {
		// Error in lua script
		handleError();
		return false;
	}
	return true;
}

auto LuaScriptable::setVariable(const string_t &name, int32_t val) -> void {
	lua_pushinteger(luaVm, val);
	lua_setglobal(luaVm, name.c_str());
}

auto LuaScriptable::setVariable(const string_t &name, const string_t &val) -> void {
	lua_pushstring(luaVm, val.c_str());
	lua_setglobal(luaVm, name.c_str());
}

auto LuaScriptable::handleError() -> void {
	printError(lua_tostring(luaVm, -1));
}

auto LuaScriptable::printError(const string_t &error) -> void {
	Player *player = PlayerDataProvider::getInstance().getPlayer(m_playerId);

	ChannelServer::getInstance().log(LogTypes::ScriptLog, error);

	if (player == nullptr) {
		std::cerr << "Script error in " << m_filename << ": " << error << std::endl;
		return;
	}

	if (player->isGm()) {
		PlayerPacket::showMessage(player, error, PlayerPacket::NoticeTypes::Red);
	}
	else {
		PlayerPacket::showMessage(player, "There is a script error; please contact an administrator", PlayerPacket::NoticeTypes::Red);
	}
}

// Lua Exports
auto LuaExports::getPlayer(lua_State *luaVm) -> Player * {
	lua_getglobal(luaVm, "system_playerId");
	Player *p = PlayerDataProvider::getInstance().getPlayer(lua_tointeger(luaVm, -1));
	lua_pop(luaVm, 1);
	return p;
}

auto LuaExports::getPlayerDeduced(int parameter, lua_State *luaVm) -> Player * {
	Player *player = nullptr;
	if (lua_type(luaVm, parameter) == LUA_TSTRING) {
		player = PlayerDataProvider::getInstance().getPlayer(lua_tostring(luaVm, parameter));
	}
	else {
		player = PlayerDataProvider::getInstance().getPlayer(lua_tointeger(luaVm, parameter));
	}
	return player;
}

auto LuaExports::getInstance(lua_State *luaVm) -> Instance * {
	lua_getglobal(luaVm, "system_instanceName");
	Instance *i = Instances::getInstance().getInstance(lua_tostring(luaVm, -1));
	lua_pop(luaVm, 1);
	return i;
}

auto LuaExports::obtainSetVariablePair(lua_State *luaVm) -> pair_t<string_t, string_t> {
	pair_t<string_t, string_t> ret;
	ret.first = lua_tostring(luaVm, 1);
	int valType = lua_type(luaVm, 2);
	switch (valType) {
		case LUA_TNIL: ret.second = "nil"; break;
		case LUA_TBOOLEAN: ret.second = lua_toboolean(luaVm, 2) ? "true" : "false"; break;
		case LUA_TNUMBER: ret.second = StringUtilities::lexical_cast<string_t>(lua_tointeger(luaVm, 2)); break;
		case LUA_TSTRING: ret.second = lua_tostring(luaVm, 2); break;
		default: throw std::exception("Invalid Lua datatype");
	}
	return ret;
}

auto LuaExports::pushGetVariableData(lua_State *luaVm, const string_t &value, bool integralReturn) -> void {
	if (value == "nil" || value == "") {
		lua_pushnil(luaVm);
	}
	else if (value == "true" || value == "false") {
		lua_pushboolean(luaVm, value == "true" ? 1 : 0);
	}
	else if (integralReturn) {
		lua_pushinteger(luaVm, StringUtilities::lexical_cast<int32_t>(value));
	}
	else {
		lua_pushstring(luaVm, value.c_str());
	}
}

// Miscellaneous
auto LuaExports::consoleOutput(lua_State *luaVm) -> int {
	std::cout << lua_tostring(luaVm, 1) << std::endl;
	return 0;
}

auto LuaExports::getRandomNumber(lua_State *luaVm) -> int {
	int32_t number = lua_tointeger(luaVm, -1);
	lua_pushinteger(luaVm, Randomizer::rand<int32_t>(number, 1));
	return 1;
}

auto LuaExports::log(lua_State *luaVm) -> int {
	ChannelServer::getInstance().log(LogTypes::ScriptLog, lua_tostring(luaVm, 1));
	return 0;
}

auto LuaExports::showGlobalMessage(lua_State *luaVm) -> int {
	const string_t &msg = lua_tostring(luaVm, -2);
	int8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessageGlobal(msg, type);
	return 0;
}

auto LuaExports::showWorldMessage(lua_State *luaVm) -> int {
	const string_t &msg = lua_tostring(luaVm, -2);
	int8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessageWorld(msg, type);
	return 0;
}

// Channel
auto LuaExports::deleteChannelVariable(lua_State *luaVm) -> int {
	const string_t &key = string_t(lua_tostring(luaVm, -1));
	EventDataProvider::getInstance().getVariables()->deleteVariable(key);
	return 0;
}

auto LuaExports::getChannel(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, ChannelServer::getInstance().getChannelId() + 1);
	return 1;
}

auto LuaExports::getChannelVariable(lua_State *luaVm) -> int {
	bool integral = false;
	if (lua_isboolean(luaVm, 2)) {
		integral = true;
	}
	const string_t &val = EventDataProvider::getInstance().getVariables()->getVariable(lua_tostring(luaVm, 1));
	pushGetVariableData(luaVm, val, integral);
	return 1;
}

auto LuaExports::isHorntailChannel(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, ChannelServer::getInstance().isHorntailChannel());
	return 1;
}

auto LuaExports::isPapChannel(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, ChannelServer::getInstance().isPapChannel());
	return 1;
}

auto LuaExports::isPianusChannel(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, ChannelServer::getInstance().isPianusChannel());
	return 1;
}

auto LuaExports::isPinkBeanChannel(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, ChannelServer::getInstance().isPinkBeanChannel());
	return 1;
}

auto LuaExports::isZakumChannel(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, ChannelServer::getInstance().isZakumChannel());
	return 1;
}

auto LuaExports::setChannelVariable(lua_State *luaVm) -> int {
	auto kvp = obtainSetVariablePair(luaVm);
	EventDataProvider::getInstance().getVariables()->setVariable(kvp.first, kvp.second);
	return 0;
}

auto LuaExports::showChannelMessage(lua_State *luaVm) -> int {
	const string_t &msg = lua_tostring(luaVm, -2);
	int8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessageChannel(msg, type);
	return 0;
}

// Bosses
auto LuaExports::getHorntailChannels(lua_State *luaVm) -> int {
	const vector_t<int8_t> &channels = ChannelServer::getInstance().getHorntailChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getMaxHorntailBattles(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, ChannelServer::getInstance().getHorntailAttempts());
	return 1;
}

auto LuaExports::getMaxPapBattles(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, ChannelServer::getInstance().getPapAttempts());
	return 1;
}

auto LuaExports::getMaxPianusBattles(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, ChannelServer::getInstance().getPianusAttempts());
	return 1;
}

auto LuaExports::getMaxPinkBeanBattles(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, ChannelServer::getInstance().getPinkBeanAttempts());
	return 1;
}

auto LuaExports::getMaxZakumBattles(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, ChannelServer::getInstance().getZakumAttempts());
	return 1;
}

auto LuaExports::getPapChannels(lua_State *luaVm) -> int {
	const vector_t<int8_t> &channels = ChannelServer::getInstance().getPapChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getPianusChannels(lua_State *luaVm) -> int {
	const vector_t<int8_t> &channels = ChannelServer::getInstance().getPianusChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getPinkBeanChannels(lua_State *luaVm) -> int {
	const vector_t<int8_t> &channels = ChannelServer::getInstance().getPinkBeanChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getZakumChannels(lua_State *luaVm) -> int {
	const vector_t<int8_t> &channels = ChannelServer::getInstance().getZakumChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

// Npc
auto LuaExports::isBusy(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getPlayer(luaVm)->getNpc() != nullptr);
	return 1;
}

auto LuaExports::removeNpc(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	uint32_t index = lua_tointeger(luaVm, 2);
	Maps::getMap(mapId)->removeNpc(index);
	return 0;
}

auto LuaExports::runNpc(lua_State *luaVm) -> int {
	int32_t npcId = lua_tointeger(luaVm, -1);
	string_t script;
	if (lua_type(luaVm, 2) == LUA_TSTRING) {
		// We already have our script name
		const string_t &specified = lua_tostring(luaVm, 2);
		script = "scripts/npcs/" + specified + ".lua";
	}
	else {
		script = ScriptDataProvider::getInstance().getScript(npcId, ScriptTypes::Npc);
	}
	Npc *npc = new Npc(npcId, getPlayer(luaVm), script);
	npc->run();
	return 0;
}

auto LuaExports::showShop(lua_State *luaVm) -> int {
	int32_t shopId = lua_tointeger(luaVm, -1);
	NpcHandler::showShop(getPlayer(luaVm), shopId);
	return 0;
}

auto LuaExports::spawnNpc(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t npcId = lua_tointeger(luaVm, 2);
	int16_t x = lua_tointeger(luaVm, 3);
	int16_t y = lua_tointeger(luaVm, 4);

	NpcSpawnInfo npc;
	npc.id = npcId;
	npc.foothold = 0;
	npc.pos = Pos(x, y);
	npc.rx0 = x - 50;
	npc.rx1 = x + 50;

	lua_pushinteger(luaVm, Maps::getMap(mapId)->addNpc(npc));
	return 1;
}

// Beauty
auto LuaExports::getAllFaces(lua_State *luaVm) -> int {
	const vector_t<int32_t> &ids = BeautyDataProvider::getInstance().getFaces(getPlayer(luaVm)->getGender());
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getAllHair(lua_State *luaVm) -> int {
	const vector_t<int32_t> &ids = BeautyDataProvider::getInstance().getHair(getPlayer(luaVm)->getGender());
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getAllSkins(lua_State *luaVm) -> int {
	const vector_t<int8_t> &ids = BeautyDataProvider::getInstance().getSkins();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); ++i) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getRandomFace(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, BeautyDataProvider::getInstance().getRandomFace(getPlayer(luaVm)->getGender()));
	return 1;
}

auto LuaExports::getRandomHair(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, BeautyDataProvider::getInstance().getRandomHair(getPlayer(luaVm)->getGender()));
	return 1;
}

auto LuaExports::getRandomSkin(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, BeautyDataProvider::getInstance().getRandomSkin());
	return 1;
}

auto LuaExports::isValidFace(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, BeautyDataProvider::getInstance().isValidFace(getPlayer(luaVm)->getGender(), lua_tointeger(luaVm, 1)));
	return 1;
}

auto LuaExports::isValidHair(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, BeautyDataProvider::getInstance().isValidHair(getPlayer(luaVm)->getGender(), lua_tointeger(luaVm, 1)));
	return 1;
}

auto LuaExports::isValidSkin(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, BeautyDataProvider::getInstance().isValidSkin(lua_tointeger(luaVm, 1)));
	return 1;
}

// Buddy
auto LuaExports::addBuddySlots(lua_State *luaVm) -> int {
	Player *p = getPlayer(luaVm);
	uint8_t csize = p->getBuddyListSize();
	int8_t mod = lua_tointeger(luaVm, 1);
	p->setBuddyListSize(csize + mod);
	return 0;
}

auto LuaExports::getBuddySlots(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getBuddyListSize());
	return 1;
}

// Skill
auto LuaExports::addSkillLevel(lua_State *luaVm) -> int {
	int32_t skillId = lua_tointeger(luaVm, 1);
	uint8_t level = lua_tointeger(luaVm, 2);

	if (lua_isnumber(luaVm, 3)) {
		// Optional argument of increasing a skill's max level
		getPlayer(luaVm)->getSkills()->setMaxSkillLevel(skillId, lua_tointeger(luaVm, 3));
	}

	getPlayer(luaVm)->getSkills()->addSkillLevel(skillId, level);
	return 0;
}

auto LuaExports::getSkillLevel(lua_State *luaVm) -> int {
	int32_t skillId = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getSkills()->getSkillLevel(skillId));
	return 1;
}

auto LuaExports::getMaxSkillLevel(lua_State *luaVm) -> int {
	int32_t skillId = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getSkills()->getMaxSkillLevel(skillId));
	return 1;
}

auto LuaExports::setMaxSkillLevel(lua_State *luaVm) -> int {
	int32_t skillId = lua_tointeger(luaVm, -2);
	uint8_t level = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getSkills()->setMaxSkillLevel(skillId, level);
	return 0;
}

// Quest
auto LuaExports::getQuestData(lua_State *luaVm) -> int {
	int16_t questId = lua_tointeger(luaVm, 1);
	lua_pushstring(luaVm, getPlayer(luaVm)->getQuests()->getQuestData(questId).c_str());
	return 1;
}

auto LuaExports::isQuestActive(lua_State *luaVm) -> int {
	int16_t questId = lua_tointeger(luaVm, -1);
	lua_pushboolean(luaVm, getPlayer(luaVm)->getQuests()->isQuestActive(questId));
	return 1;
}

auto LuaExports::isQuestInactive(lua_State *luaVm) -> int {
	int16_t questId = lua_tointeger(luaVm, -1);
	bool active = !(getPlayer(luaVm)->getQuests()->isQuestActive(questId) || getPlayer(luaVm)->getQuests()->isQuestComplete(questId));
	lua_pushboolean(luaVm, active);
	return 1;
}

auto LuaExports::isQuestCompleted(lua_State *luaVm) -> int {
	int16_t questId = lua_tointeger(luaVm, -1);
	lua_pushboolean(luaVm, getPlayer(luaVm)->getQuests()->isQuestComplete(questId));
	return 1;
}

auto LuaExports::setQuestData(lua_State *luaVm) -> int {
	int16_t questId = lua_tointeger(luaVm, 1);
	const string_t &data = lua_tostring(luaVm, 2);
	getPlayer(luaVm)->getQuests()->setQuestData(questId, data);
	return 0;
}

// Inventory
auto LuaExports::addSlots(lua_State *luaVm) -> int {
	int8_t inventory = lua_tointeger(luaVm, -2);
	int8_t rows = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getInventory()->addMaxSlots(inventory, rows);
	return 0;
}

auto LuaExports::addStorageSlots(lua_State *luaVm) -> int {
	int8_t slots = lua_tointeger(luaVm, 1);
	getPlayer(luaVm)->getStorage()->setSlots(getPlayer(luaVm)->getStorage()->getSlots() + slots);
	return 0;
}

auto LuaExports::destroyEquippedItem(lua_State *luaVm) -> int {
	int32_t itemId = lua_tointeger(luaVm, 1);
	Player *player = getPlayer(luaVm);
	bool destroyed = player->getInventory()->isEquippedItem(itemId);
	if (destroyed) {
		player->getInventory()->destroyEquippedItem(itemId);
	}
	lua_pushboolean(luaVm, destroyed);
	return 1;
}

auto LuaExports::getEquippedItemInSlot(lua_State *luaVm) -> int {
	int16_t slot = lua_tointeger(luaVm, 1);
	lua_pushinteger(luaVm, getPlayer(luaVm)->getInventory()->getEquippedId(slot));
	return 1;
}

auto LuaExports::getItemAmount(lua_State *luaVm) -> int {
	int32_t itemId = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getInventory()->getItemAmount(itemId));
	return 1;
}

auto LuaExports::getMesos(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getInventory()->getMesos());
	return 1;
}

auto LuaExports::getOpenSlots(lua_State *luaVm) -> int {
	int8_t inv = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getInventory()->getOpenSlotsNum(inv));
	return 1;
}

auto LuaExports::giveItem(lua_State *luaVm) -> int {
	int32_t itemId = lua_tointeger(luaVm, 1);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 2)) {
		amount = lua_tointeger(luaVm, 2);
	}
	bool success = Quests::giveItem(getPlayer(luaVm), itemId, amount);
	lua_pushboolean(luaVm, success);
	return 1;
}

auto LuaExports::giveMesos(lua_State *luaVm) -> int {
	int32_t mesos = lua_tointeger(luaVm, -1);
	bool success = Quests::giveMesos(getPlayer(luaVm), mesos);
	lua_pushboolean(luaVm, success);
	return 1;
}

auto LuaExports::hasOpenSlotsFor(lua_State *luaVm) -> int {
	int32_t itemId = lua_tointeger(luaVm, 1);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 2)) {
		amount = lua_tointeger(luaVm, 2);
	}
	lua_pushboolean(luaVm, getPlayer(luaVm)->getInventory()->hasOpenSlotsFor(itemId, amount));
	return 1;
}

auto LuaExports::isEquippedItem(lua_State *luaVm) -> int {
	int32_t itemId = lua_tointeger(luaVm, 1);
	lua_pushboolean(luaVm, getPlayer(luaVm)->getInventory()->isEquippedItem(itemId));
	return 1;
}

auto LuaExports::useItem(lua_State *luaVm) -> int {
	int32_t itemId = lua_tointeger(luaVm, -1);
	Inventory::useItem(getPlayer(luaVm), itemId);
	return 0;
}

// Player
auto LuaExports::deletePlayerVariable(lua_State *luaVm) -> int {
	const string_t &key = string_t(lua_tostring(luaVm, -1));
	getPlayer(luaVm)->getVariables()->deleteVariable(key);
	return 0;
}

auto LuaExports::endMorph(lua_State *luaVm) -> int {
	getPlayer(luaVm)->getActiveBuffs()->endMorph();
	return 0;
}

auto LuaExports::getAp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getAp());
	return 1;
}

auto LuaExports::getDex(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getDex());
	return 1;
}

auto LuaExports::getExp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getExp());
	return 1;
}

auto LuaExports::getEyes(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getEyes());
	return 1;
}

auto LuaExports::getFame(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getFame());
	return 1;
}

auto LuaExports::getFh(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getFoothold());
	return 1;
}

auto LuaExports::getGender(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getGender());
	return 1;
}

auto LuaExports::getGmLevel(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getGmLevel());
	return 1;
}

auto LuaExports::getHair(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getHair());
	return 1;
}

auto LuaExports::getHp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getHp());
	return 1;
}

auto LuaExports::getHpMpAp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getHpMpAp());
	return 1;
}

auto LuaExports::getId(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getId());
	return 1;
}

auto LuaExports::getInt(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getInt());
	return 1;
}

auto LuaExports::getJob(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getJob());
	return 1;
}

auto LuaExports::getLevel(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getLevel());
	return 1;
}

auto LuaExports::getLuk(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getLuk());
	return 1;
}

auto LuaExports::getMap(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getMapId());
	return 1;
}

auto LuaExports::getMaxHp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxHp());
	return 1;
}

auto LuaExports::getMaxMp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxMp());
	return 1;
}

auto LuaExports::getMp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMp());
	return 1;
}

auto LuaExports::getName(lua_State *luaVm) -> int {
	lua_pushstring(luaVm, getPlayer(luaVm)->getName().c_str());
	return 1;
}

auto LuaExports::getPlayerVariable(lua_State *luaVm) -> int {
	bool integral = false;
	if (lua_isboolean(luaVm, 2)) {
		integral = true;
	}
	const string_t &val = getPlayer(luaVm)->getVariables()->getVariable(lua_tostring(luaVm, 1));
	pushGetVariableData(luaVm, val, integral);
	return 1;
}

auto LuaExports::getPosX(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getPos().x);
	return 1;
}

auto LuaExports::getPosY(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getPos().y);
	return 1;
}

auto LuaExports::getRealMaxHp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxHp(true));
	return 1;
}

auto LuaExports::getRealMaxMp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxMp(true));
	return 1;
}

auto LuaExports::getSkin(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getSkin());
	return 1;
}

auto LuaExports::getSp(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getSp());
	return 1;
}

auto LuaExports::getStr(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getStr());
	return 1;
}

auto LuaExports::giveAp(lua_State *luaVm) -> int {
	int16_t ap = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setAp(getPlayer(luaVm)->getStats()->getAp() + ap);
	return 0;
}

auto LuaExports::giveExp(lua_State *luaVm) -> int {
	uint32_t exp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->giveExp(exp * ChannelServer::getInstance().getQuestExpRate(), true);
	return 0;
}

auto LuaExports::giveFame(lua_State *luaVm) -> int {
	int32_t fame = lua_tointeger(luaVm, 1);
	Quests::giveFame(getPlayer(luaVm), fame);
	return 0;
}

auto LuaExports::giveSp(lua_State *luaVm) -> int {
	int16_t sp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setSp(getPlayer(luaVm)->getStats()->getSp() + sp);
	return 0;
}

auto LuaExports::isActiveItem(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getPlayer(luaVm)->getActiveBuffs()->getActiveSkillLevel(-1 * lua_tointeger(luaVm, -1)) > 0);
	return 1;
}

auto LuaExports::isActiveSkill(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getPlayer(luaVm)->getActiveBuffs()->getActiveSkillLevel(lua_tointeger(luaVm, -1)) > 0);
	return 1;
}

auto LuaExports::isGm(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getPlayer(luaVm)->isGm());
	return 1;
}

auto LuaExports::isOnline(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getPlayerDeduced(-1, luaVm) != nullptr);
	return 1;
}

auto LuaExports::revertPlayer(lua_State *luaVm) -> int {
	lua_getglobal(luaVm, "system_oldPlayerId");
	lua_setglobal(luaVm, "system_playerId");
	return 0;
}

auto LuaExports::setAp(lua_State *luaVm) -> int {
	int16_t ap = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setAp(ap);
	return 0;
}

auto LuaExports::setDex(lua_State *luaVm) -> int {
	int16_t dex = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setDex(dex);
	return 0;
}

auto LuaExports::setExp(lua_State *luaVm) -> int {
	int32_t exp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setExp(exp);
	return 0;
}

auto LuaExports::setHp(lua_State *luaVm) -> int {
	uint16_t hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setHp(hp);
	return 0;
}

auto LuaExports::setInt(lua_State *luaVm) -> int {
	int16_t intt = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setInt(intt);
	return 0;
}

auto LuaExports::setJob(lua_State *luaVm) -> int {
	int16_t job = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setJob(job);
	return 0;
}

auto LuaExports::setLevel(lua_State *luaVm) -> int {
	uint8_t level = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setLevel(level);
	return 0;
}

auto LuaExports::setLuk(lua_State *luaVm) -> int {
	int16_t luk = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setLuk(luk);
	return 0;
}

auto LuaExports::setMap(lua_State *luaVm) -> int {
	PortalInfo *portal = nullptr;

	int32_t mapId = lua_tointeger(luaVm, 1);

	if (lua_isstring(luaVm, 2)) {
		// Optional portal parameter
		const string_t &to = lua_tostring(luaVm, 2);
		portal = Maps::getMap(mapId)->getPortal(to);
	}

	if (Maps::getMap(mapId)) {
		getPlayer(luaVm)->setMap(mapId, portal);
	}
	return 0;
}

auto LuaExports::setMaxHp(lua_State *luaVm) -> int {
	uint16_t hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setMaxHp(hp);
	return 0;
}

auto LuaExports::setMaxMp(lua_State *luaVm) -> int {
	uint16_t mp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setMaxMp(mp);
	return 0;
}

auto LuaExports::setMp(lua_State *luaVm) -> int {
	uint16_t mp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setMp(mp);
	return 0;
}

auto LuaExports::setPlayer(lua_State *luaVm) -> int {
	Player *player = getPlayerDeduced(-1, luaVm);
	if (player != nullptr) {
		lua_getglobal(luaVm, "system_playerId");
		lua_setglobal(luaVm, "system_oldPlayerId");

		lua_pushinteger(luaVm, player->getId());
		lua_setglobal(luaVm, "system_playerId");
	}
	lua_pushboolean(luaVm, player != nullptr);
	return 1;
}

auto LuaExports::setPlayerVariable(lua_State *luaVm) -> int {
	auto kvp = obtainSetVariablePair(luaVm);
	getPlayer(luaVm)->getVariables()->setVariable(kvp.first, kvp.second);
	return 0;
}

auto LuaExports::setSp(lua_State *luaVm) -> int {
	int16_t sp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setSp(sp);
	return 0;
}

auto LuaExports::setStr(lua_State *luaVm) -> int {
	int16_t str = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setStr(str);
	return 0;
}

auto LuaExports::setStyle(lua_State *luaVm) -> int {
	int32_t id = lua_tointeger(luaVm, -1);
	int32_t type = GameLogicUtilities::getItemType(id);
	if (type == 0) {
		getPlayer(luaVm)->setSkin(static_cast<int8_t>(id));
	}
	else if (type == 2) {
		getPlayer(luaVm)->setEyes(id);
	}
	else if (type == 3) {
		getPlayer(luaVm)->setHair(id);
	}
	InventoryPacket::updatePlayer(getPlayer(luaVm));
	return 0;
}

auto LuaExports::showInstructionBubble(lua_State *luaVm) -> int {
	const string_t &msg = lua_tostring(luaVm, 1);
	int16_t width = lua_tointeger(luaVm, 2);
	int16_t height = lua_tointeger(luaVm, 3);

	if (width == 0) {
		width = -1;
	}
	if (height == 0) {
		height = 5;
	}

	PlayerPacket::instructionBubble(getPlayer(luaVm), msg, width, height);
	return 0;
}

auto LuaExports::showMessage(lua_State *luaVm) -> int {
	const string_t &msg = lua_tostring(luaVm, -2);
	uint8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessage(getPlayer(luaVm), msg, type);
	return 0;
}

// Effects
auto LuaExports::playFieldSound(lua_State *luaVm) -> int {
	const string_t &val = lua_tostring(luaVm, 1);
	if (lua_isnumber(luaVm, 2)) {
		EffectPacket::sendFieldSound(lua_tointeger(luaVm, 2), val);
	}
	else {
		EffectPacket::sendFieldSound(getPlayer(luaVm), val);
	}
	return 0;
}

auto LuaExports::playMinigameSound(lua_State *luaVm) -> int {
	const string_t &val = lua_tostring(luaVm, 1);
	if (lua_isnumber(luaVm, 2)) {
		EffectPacket::sendMinigameSound(lua_tointeger(luaVm, 2), val);
	}
	else {
		EffectPacket::sendMinigameSound(getPlayer(luaVm), val);
	}
	return 0;
}

auto LuaExports::setMusic(lua_State *luaVm) -> int {
	int32_t mapId = -1;
	const string_t &music = lua_tostring(luaVm, 1);

	if (lua_isnumber(luaVm, 2)) {
		mapId = lua_tointeger(luaVm, 2);
	}
	else if (Player *player = getPlayer(luaVm)) {
		mapId = player->getMapId();
	}

	if (mapId != -1) {
		Maps::getMap(mapId)->setMusic(music);
	}
	return 0;
}

auto LuaExports::showMapEffect(lua_State *luaVm) -> int {
	const string_t &val = lua_tostring(luaVm, -1);
	EffectPacket::sendEffect(getPlayer(luaVm)->getMapId(), val);
	return 0;
}

auto LuaExports::showMapEvent(lua_State *luaVm) -> int {
	const string_t &val = lua_tostring(luaVm, -1);
	EffectPacket::sendEvent(getPlayer(luaVm)->getMapId(), val);
	return 0;
}

// Map
auto LuaExports::clearDrops(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	Maps::getMap(mapId)->clearDrops(false);
	return 0;
}

auto LuaExports::clearMobs(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	Maps::getMap(mapId)->killMobs(nullptr);
	return 0;
}

auto LuaExports::countMobs(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mobId = 0;
	if (lua_isnumber(luaVm, 2)) {
		mobId = lua_tointeger(luaVm, 2);
	}
	lua_pushinteger(luaVm, Maps::getMap(mapId)->countMobs(mobId));
	return 1;
}

auto LuaExports::getAllMapPlayerIds(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	Map *map = Maps::getMap(mapId);
	if (map != nullptr) {
		lua_newtable(luaVm);
		int top = lua_gettop(luaVm);
		for (uint32_t i = 0; i < map->getNumPlayers(); i++) {
			lua_pushinteger(luaVm, i + 1);
			lua_pushinteger(luaVm, map->getPlayer(i)->getId());
			lua_settable(luaVm, top);
		}
		return 1;
	}
	return 0;
}

auto LuaExports::getNumPlayers(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, -1);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getNumPlayers());
	return 1;
}

auto LuaExports::getReactorState(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, -2);
	int32_t reactorId = lua_tointeger(luaVm, -1);
	Map *map = Maps::getMap(mapId);
	for (uint32_t i = 0; i < map->getNumReactors(); i++) {
		if (map->getReactor(i)->getReactorId() == reactorId) {
			lua_pushinteger(luaVm, map->getReactor(i)->getState());
			return 1;
		}
	}
	lua_pushinteger(luaVm, 0);
	return 1;
}

auto LuaExports::killMobs(lua_State *luaVm) -> int {
	int32_t mobId = lua_tointeger(luaVm, 1);
	int32_t killed = getPlayer(luaVm)->getMap()->killMobs(nullptr, mobId);
	lua_pushinteger(luaVm, killed);
	return 1;
}

auto LuaExports::setMapSpawn(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t spawn = lua_tointeger(luaVm, 2);
	Maps::getMap(mapId)->setMobSpawning(spawn);
	return 0;
}

auto LuaExports::setReactorState(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, -3);
	int32_t reactorId = lua_tointeger(luaVm, -2);
	uint8_t state = lua_tointeger(luaVm, -1);
	for (size_t i = 0; i < Maps::getMap(mapId)->getNumReactors(); i++) {
		Reactor *reactor = Maps::getMap(mapId)->getReactor(i);
		if (reactor->getReactorId() == reactorId) {
			reactor->setState(state, true);
			break;
		}
	}
	return 0;
}

auto LuaExports::showMapMessage(lua_State *luaVm) -> int {
	const string_t &msg = lua_tostring(luaVm, -2);
	uint8_t type = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getMap()->showMessage(msg, type);
	return 0;
}

auto LuaExports::showMapTimer(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t time = lua_tointeger(luaVm, 2);
	Maps::getMap(mapId)->setMapTimer(seconds_t(time));
	return 0;
}

auto LuaExports::spawnMob(lua_State *luaVm) -> int {
	int32_t mobId = lua_tointeger(luaVm, -1);
	Player *player = getPlayer(luaVm);
	lua_pushinteger(luaVm, player->getMap()->spawnMob(mobId, player->getPos())->getMapMobId());
	return 1;
}

auto LuaExports::spawnMobPos(lua_State *luaVm) -> int {
	int32_t mobId = lua_tointeger(luaVm, 1);
	int16_t x = lua_tointeger(luaVm, 2);
	int16_t y = lua_tointeger(luaVm, 3);
	int16_t foothold = 0;
	if (lua_isnumber(luaVm, 4)) {
		foothold = lua_tointeger(luaVm, 4);
	}
	lua_pushinteger(luaVm, getPlayer(luaVm)->getMap()->spawnMob(mobId, Pos(x, y), foothold)->getMapMobId());
	return 1;
}

// Mob
auto LuaExports::getMobFh(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getFoothold());
	return 1;
}

auto LuaExports::getMobHp(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getHp());
	return 1;
}

auto LuaExports::getMobMaxHp(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMaxHp());
	return 1;
}

auto LuaExports::getMobMaxMp(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMaxMp());
	return 1;
}

auto LuaExports::getMobMp(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMp());
	return 1;
}

auto LuaExports::getMobPosX(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getPos().x);
	return 1;
}

auto LuaExports::getMobPosY(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getPos().y);
	return 1;
}

auto LuaExports::getRealMobId(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMobId());
	return 1;
}

auto LuaExports::killMob(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	auto mob = Maps::getMap(mapId)->getMob(mapMobId);
	if (mob != nullptr) {
		mob->kill();
	}
	return 0;
}

auto LuaExports::mobDropItem(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	int32_t mapMobId = lua_tointeger(luaVm, 2);
	int32_t itemId = lua_tointeger(luaVm, 3);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 4)) {
		amount = lua_tointeger(luaVm, 4);
	}
	auto mob = Maps::getMap(mapId)->getMob(mapMobId);
	if (mob != nullptr) {
		Item f(itemId, amount);
		Drop *drop = new Drop(mapId, f, mob->getPos(), 0);
		drop->setTime(0);
		drop->doDrop(mob->getPos());
	}
	return 0;
}

// Time
auto LuaExports::getDate(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, TimeUtilities::getDate());
	return 1;
}

auto LuaExports::getDay(lua_State *luaVm) -> int {
	bool isStringReturn = false;
	if (lua_isboolean(luaVm, -1)) {
		isStringReturn = (lua_toboolean(luaVm, -1) != 0);
	}
	if (isStringReturn) {
		lua_pushstring(luaVm, TimeUtilities::getDayString(false).c_str());
	}
	else {
		lua_pushinteger(luaVm, TimeUtilities::getDay());
	}
	return 1;
}

auto LuaExports::getHour(lua_State *luaVm) -> int {
	bool military = false;
	if (lua_isboolean(luaVm, 1)) {
		military = lua_toboolean(luaVm, 1) != 0;
	}
	lua_pushinteger(luaVm, TimeUtilities::getHour(military));
	return 1;
}

auto LuaExports::getMinute(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, TimeUtilities::getMinute());
	return 1;
}

auto LuaExports::getMonth(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, TimeUtilities::getMonth());
	return 1;
}

auto LuaExports::getNearestMinute(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, static_cast<int32_t>(TimeUtilities::getDistanceToNextMinuteMark(lua_tointeger(luaVm, 1)).count()));
	return 1;
}

auto LuaExports::getSecond(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, TimeUtilities::getSecond());
	return 1;
}

auto LuaExports::getTime(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, (lua_Integer) time(nullptr)); // Here's to hoping that lua_Integer is an 8-byte type on most platforms!
	return 1;
}

auto LuaExports::getTimeZoneOffset(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, TimeUtilities::getTimeZoneOffset());
	return 1;
}

auto LuaExports::getWeek(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, TimeUtilities::getWeek());
	return 1;
}

auto LuaExports::getYear(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, TimeUtilities::getYear(false));
	return 1;
}

auto LuaExports::isDst(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, TimeUtilities::isDst());
	return 1;
}

// Rates
auto LuaExports::getDropRate(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, ChannelServer::getInstance().getDropRate());
	return 1;
}

auto LuaExports::getExpRate(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, ChannelServer::getInstance().getMobExpRate());
	return 1;
}

auto LuaExports::getQuestExpRate(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, ChannelServer::getInstance().getQuestExpRate());
	return 1;
}

auto LuaExports::getMesoRate(lua_State *luaVm) -> int {
	lua_pushnumber(luaVm, ChannelServer::getInstance().getMobMesoRate());
	return 1;
}

// Party
auto LuaExports::checkPartyFootholds(lua_State *luaVm) -> int {
	int8_t numbermembers = lua_tointeger(luaVm, 1);
	Party *p = getPlayer(luaVm)->getParty();
	bool winner = false;
	if (p != nullptr) {
		vector_t<vector_t<int16_t>> footholds;
		lua_pushnil(luaVm);
		while (lua_next(luaVm, 2)) {
			vector_t<int16_t> arr;
			lua_pushnil(luaVm);
			while (lua_next(luaVm, -2)) {
				int16_t val = lua_tointeger(luaVm, -1);
				arr.push_back(val);
				lua_pop(luaVm, 1);
			}
			footholds.push_back(arr);
			lua_pop(luaVm, 1);
		}
		winner = p->checkFootholds(numbermembers, footholds);
	}
	lua_pushboolean(luaVm, winner);
	return 1;
}

auto LuaExports::getAllPartyPlayerIds(lua_State *luaVm) -> int {
	Party *p = getPlayer(luaVm)->getParty();
	if (p != nullptr) {
		const vector_t<int32_t> &ids = p->getAllPlayerIds();
		lua_newtable(luaVm);
		int top = lua_gettop(luaVm);
		for (size_t i = 0; i < ids.size(); i++) {
			lua_pushinteger(luaVm, i + 1);
			lua_pushinteger(luaVm, ids[i]);
			lua_settable(luaVm, top);
		}
		return 1;
	}
	return 0;
}

auto LuaExports::getPartyCount(lua_State *luaVm) -> int {
	int32_t mcount = 0;
	Party *p = getPlayer(luaVm)->getParty();
	if (p != nullptr) {
		mcount = p->getMembersCount();
	}
	lua_pushinteger(luaVm, mcount);
	return 1;
}

auto LuaExports::getPartyId(lua_State *luaVm) -> int {
	int32_t pid = 0;
	Party *p = getPlayer(luaVm)->getParty();
	if (p != nullptr) {
		pid = p->getId();
	}
	lua_pushinteger(luaVm, pid);
	return 1;
}

auto LuaExports::getPartyMapCount(lua_State *luaVm) -> int {
	Player *player = getPlayer(luaVm);
	Party *p = player->getParty();
	int8_t members = 0;
	if (p != nullptr) {
		int32_t mapId = lua_tointeger(luaVm, 1);
		members = p->getMemberCountOnMap(mapId);
	}
	lua_pushinteger(luaVm, members);
	return 1;
}

auto LuaExports::isPartyInLevelRange(lua_State *luaVm) -> int {
	Player *player = getPlayer(luaVm);
	Party *p = player->getParty();
	bool iswithin = false;
	if (p != nullptr) {
		uint8_t lowBound = lua_tointeger(luaVm, 1);
		uint8_t highBound = lua_tointeger(luaVm, 2);
		iswithin = p->isWithinLevelRange(lowBound, highBound);
	}
	lua_pushboolean(luaVm, iswithin);
	return 1;
}

auto LuaExports::isPartyLeader(lua_State *luaVm) -> int {
	Player *player = getPlayer(luaVm);
	Party *p = player->getParty();
	bool isleader = false;
	if (p != nullptr) {
		isleader = player == p->getLeader();
	}
	lua_pushboolean(luaVm, isleader);
	return 1;
}

auto LuaExports::verifyPartyFootholds(lua_State *luaVm) -> int {
	int8_t numbermembers = lua_tointeger(luaVm, 1);
	Party *p = getPlayer(luaVm)->getParty();
	bool winner = false;
	if (p != nullptr) {
		vector_t<vector_t<int16_t>> footholds;
		lua_pushnil(luaVm);
		while (lua_next(luaVm, 1)) {
			vector_t<int16_t> arr;
			lua_pushnil(luaVm);
			while (lua_next(luaVm, -2)) {
				int16_t fff = lua_tointeger(luaVm, -1);
				arr.push_back(fff);
				lua_pop(luaVm, 1);
			}
			footholds.push_back(arr);
			lua_pop(luaVm, 1);
		}
		winner = p->verifyFootholds(footholds);
	}
	lua_pushboolean(luaVm, winner);
	return 1;
}

auto LuaExports::warpParty(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	string_t to;
	if (lua_isstring(luaVm, 2)) {
		// Optional portal parameter
		string_t to = lua_tostring(luaVm, 2);
	}
	Player *player = getPlayer(luaVm);
	Party *p = player->getParty();
	if (p != nullptr) {
		p->warpAllMembers(mapId, to);
	}
	return 0;
}

// Instance
auto LuaExports::addInstanceMap(lua_State *luaVm) -> int {
	int32_t mapId = lua_tointeger(luaVm, 1);
	getInstance(luaVm)->addMap(mapId);
	return 0;
}

auto LuaExports::addInstanceParty(lua_State *luaVm) -> int {
	int32_t id = lua_tointeger(luaVm, -1);
	if (Party *p = PlayerDataProvider::getInstance().getParty(id)) {
		getInstance(luaVm)->addParty(p);
	}
	return 0;
}

auto LuaExports::addInstancePlayer(lua_State *luaVm) -> int {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->addPlayer(player);
	return 0;
}

auto LuaExports::addPlayerSignUp(lua_State *luaVm) -> int {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->addPlayerSignUp(player);
	return 0;
}

auto LuaExports::banInstancePlayer(lua_State *luaVm) -> int {
	if (Player *player = getPlayerDeduced(-1, luaVm)) {
		getInstance(luaVm)->setBanned(player->getName(), true);
	}
	return 0;
}

auto LuaExports::checkInstanceTimer(lua_State *luaVm) -> int {
	const string_t &name = lua_tostring(luaVm, 1);
	lua_pushinteger(luaVm, static_cast<int32_t>(getInstance(luaVm)->getTimerSecondsRemaining(name).count()));
	return 1;
}

auto LuaExports::createInstance(lua_State *luaVm) -> int {
	const string_t &name = lua_tostring(luaVm, 1);
	int32_t time = lua_tointeger(luaVm, 2);
	bool showTimer = lua_toboolean(luaVm, 3) != 0;
	int32_t persistent = 0;
	int32_t map = 0;
	int32_t id = 0;
	Player *player = getPlayer(luaVm);
	if (lua_isnumber(luaVm, 4)) {
		persistent = lua_tointeger(luaVm, 4);
	}
	if (player != nullptr) {
		map = player->getMapId();
		id = player->getId();
	}
	Instance *instance = new Instance(name, map, id, seconds_t(time), seconds_t(persistent), showTimer);
	Instances::getInstance().addInstance(instance);
	instance->sendMessage(BeginInstance);

	if (instance->showTimer()) {
		instance->showTimer(true, true);
	}
	lua_pushstring(luaVm, name.c_str());
	lua_setglobal(luaVm, "system_instanceName");
	return 0;
}

auto LuaExports::deleteInstanceVariable(lua_State *luaVm) -> int {
	getInstance(luaVm)->getVariables()->deleteVariable(lua_tostring(luaVm, 1));
	return 0;
}

auto LuaExports::getAllInstancePlayerIds(lua_State *luaVm) -> int {
	const vector_t<int32_t> &ids = getInstance(luaVm)->getAllPlayerIds();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

auto LuaExports::getBannedInstancePlayerByIndex(lua_State *luaVm) -> int {
	lua_pushstring(luaVm, getInstance(luaVm)->getBannedPlayerByIndex(lua_tointeger(luaVm, -1)).c_str());
	return 1;
}

auto LuaExports::getBannedInstancePlayerCount(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getInstance(luaVm)->getBannedPlayerNum());
	return 1;
}

auto LuaExports::getInstanceMax(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getInstance(luaVm)->getMaxPlayers());
	return 1;
}

auto LuaExports::getInstancePlayerByIndex(lua_State *luaVm) -> int {
	lua_pushstring(luaVm, getInstance(luaVm)->getPlayerByIndex(lua_tointeger(luaVm, -1)).c_str());
	return 1;
}

auto LuaExports::getInstancePlayerCount(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getInstance(luaVm)->getPlayerNum());
	return 1;
}

auto LuaExports::getInstancePlayerId(lua_State *luaVm) -> int {
	Player *player = getPlayerDeduced(-1, luaVm);
	lua_pushinteger(luaVm, player->getId());
	return 1;
}

auto LuaExports::getInstanceSignupCount(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getInstance(luaVm)->getPlayerSignupNum());
	return 1;
}

auto LuaExports::getInstanceTime(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, static_cast<int32_t>(getInstance(luaVm)->checkInstanceTimer().count()));
	return 1;
}

auto LuaExports::getInstanceVariable(lua_State *luaVm) -> int {
	bool integral = false;
	if (lua_isboolean(luaVm, 2)) {
		integral = true;
	}
	const string_t &val = getInstance(luaVm)->getVariables()->getVariable(lua_tostring(luaVm, 1));
	pushGetVariableData(luaVm, val, integral);
	return 1;
}

auto LuaExports::isBannedInstancePlayer(lua_State *luaVm) -> int {
	Player *player = getPlayerDeduced(-1, luaVm);
	lua_pushboolean(luaVm, getInstance(luaVm)->isBanned(player->getName()));
	return 1;
}

auto LuaExports::isInstance(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, Instances::getInstance().isInstance(lua_tostring(luaVm, 1)));
	return 1;
}

auto LuaExports::isInstanceMap(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getInstance(luaVm)->isInstanceMap(lua_tointeger(luaVm, -1)));
	return 1;
}

auto LuaExports::isInstancePersistent(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getInstance(luaVm)->getPersistence().count() != 0);
	return 1;
}

auto LuaExports::isPlayerSignedUp(lua_State *luaVm) -> int {
	lua_pushboolean(luaVm, getInstance(luaVm)->isPlayerSignedUp(lua_tostring(luaVm, -1)));
	return 1;
}

auto LuaExports::markForDelete(lua_State *luaVm) -> int {
	getInstance(luaVm)->markForDelete();
	return 0;
}

auto LuaExports::moveAllPlayers(lua_State *luaVm) -> int {
	PortalInfo *portal = nullptr;

	int32_t mapId = lua_tointeger(luaVm, 1);

	if (lua_isstring(luaVm, 2)) {
		// Optional portal parameter
		const string_t &to = lua_tostring(luaVm, 2);
		portal = Maps::getMap(mapId)->getPortal(to);
	}

	getInstance(luaVm)->moveAllPlayers(mapId, true, portal);
	return 0;
}

auto LuaExports::passPlayersBetweenInstances(lua_State *luaVm) -> int {
	PortalInfo *portal = nullptr;

	int32_t mapId = lua_tointeger(luaVm, 1);

	if (lua_isstring(luaVm, 2)) {
		// Optional portal parameter
		const string_t &to = lua_tostring(luaVm, 2);
		portal = Maps::getMap(mapId)->getPortal(to);
	}

	getInstance(luaVm)->moveAllPlayers(mapId, false, portal);
	return 0;
}

auto LuaExports::removeAllInstancePlayers(lua_State *luaVm) -> int {
	getInstance(luaVm)->removeAllPlayers();
	return 0;
}

auto LuaExports::removeInstancePlayer(lua_State *luaVm) -> int {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->removePlayer(player);
	return 0;
}

auto LuaExports::removePlayerSignUp(lua_State *luaVm) -> int {
	getInstance(luaVm)->removePlayerSignUp(lua_tostring(luaVm, -1));
	return 0;
}

auto LuaExports::respawnInstanceMobs(lua_State *luaVm) -> int {
	int32_t mapId = Maps::NoMap;
	if (lua_isnumber(luaVm, 1)) {
		mapId = lua_tointeger(luaVm, 1);
	}
	getInstance(luaVm)->respawnMobs(mapId);
	return 0;
}

auto LuaExports::respawnInstanceReactors(lua_State *luaVm) -> int {
	int32_t mapId = Maps::NoMap;
	if (lua_isnumber(luaVm, 1)) {
		mapId = lua_tointeger(luaVm, 1);
	}
	getInstance(luaVm)->respawnReactors(mapId);
	return 0;
}

auto LuaExports::revertInstance(lua_State *luaVm) -> int {
	lua_getglobal(luaVm, "system_oldInstanceName");
	lua_setglobal(luaVm, "system_instanceName");
	return 0;
}

auto LuaExports::setInstance(lua_State *luaVm) -> int {
	Instance *instance = Instances::getInstance().getInstance(lua_tostring(luaVm, -1));
	if (instance != nullptr) {
		lua_getglobal(luaVm, "system_instanceName");
		lua_setglobal(luaVm, "system_oldInstanceName");

		lua_pushstring(luaVm, instance->getName().c_str());
		lua_setglobal(luaVm, "system_instanceName");
	}
	lua_pushboolean(luaVm, instance != nullptr);
	return 1;
}

auto LuaExports::setInstanceMax(lua_State *luaVm) -> int {
	getInstance(luaVm)->setMaxPlayers(lua_tointeger(luaVm, 1));
	return 0;
}

auto LuaExports::setInstancePersistence(lua_State *luaVm) -> int {
	seconds_t persistence = seconds_t(lua_tointeger(luaVm, 1));
	getInstance(luaVm)->setPersistence(persistence);
	return 0;
}

auto LuaExports::setInstanceReset(lua_State *luaVm) -> int {
	bool reset = lua_toboolean(luaVm, 1) != 0;
	getInstance(luaVm)->setResetAtEnd(reset);
	return 0;
}

auto LuaExports::setInstanceTime(lua_State *luaVm) -> int {
	seconds_t time = seconds_t(lua_tointeger(luaVm, 1));
	getInstance(luaVm)->setInstanceTimer(time);
	return 0;
}

auto LuaExports::setInstanceVariable(lua_State *luaVm) -> int {
	auto kvp = obtainSetVariablePair(luaVm);
	getInstance(luaVm)->getVariables()->setVariable(kvp.first, kvp.second);
	return 0;
}

auto LuaExports::showInstanceTime(lua_State *luaVm) -> int {
	getInstance(luaVm)->showTimer(lua_toboolean(luaVm, 1) != 0);
	return 0;
}

auto LuaExports::startInstanceTimer(lua_State *luaVm) -> int {
	const string_t &name = lua_tostring(luaVm, 1);
	TimerAction t;
	t.time = lua_tointeger(luaVm, 2);
	if (lua_isnumber(luaVm, 3)) {
		t.persistent = lua_tointeger(luaVm, 3);
	}
	t.counterId = getInstance(luaVm)->getCounterId();
	lua_pushboolean(luaVm, getInstance(luaVm)->addTimer(name, t));
	return 1;
}

auto LuaExports::stopAllInstanceTimers(lua_State *luaVm) -> int {
	getInstance(luaVm)->removeAllTimers();
	return 0;
}

auto LuaExports::stopInstanceTimer(lua_State *luaVm) -> int {
	const string_t &name = lua_tostring(luaVm, 1);
	getInstance(luaVm)->removeTimer(name);
	return 0;
}

auto LuaExports::unbanInstancePlayer(lua_State *luaVm) -> int {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->setBanned(player->getName(), false);
	return 0;
}