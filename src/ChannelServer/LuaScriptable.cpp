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
#include "LuaScriptable.h"
#include "AlliancePacket.h"
#include "BeautyDataProvider.h"
#include "ChannelServer.h"
#include "Drop.h"
#include "EffectPacket.h"
#include "EventDataProvider.h"
#include "GuildPacket.h"
#include "Instance.h"
#include "Instances.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mob.h"
#include "Npc.h"
#include "NpcHandler.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Quests.h"
#include "Randomizer.h"
#include "Reactor.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include "TimeUtilities.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>

using std::vector;

LuaScriptable::LuaScriptable(const string &filename, int32_t playerid) :
filename(filename),
playerid(playerid),
luaVm(lua_open())
{
	initialize();
}

LuaScriptable::~LuaScriptable() {
	lua_close(luaVm);
}

void LuaScriptable::initialize() {
	luaopen_base(luaVm);
	setVariable("_playerid", playerid); // Pushing ID for reference from static functions
	setVariable("m_blue", PlayerPacket::NoticeTypes::Blue);
	setVariable("m_red", PlayerPacket::NoticeTypes::Red);
	setVariable("m_notice", PlayerPacket::NoticeTypes::Notice);
	setVariable("m_box", PlayerPacket::NoticeTypes::Box);

	Player *player = LuaExports::getPlayer(luaVm);
	if (player != nullptr && player->getInstance() != nullptr)
		setVariable("_instancename", player->getInstance()->getName());

	// Miscellanous
	lua_register(luaVm, "consoleOutput", &LuaExports::consoleOutput);
	lua_register(luaVm, "getRandomNumber", &LuaExports::getRandomNumber);
	lua_register(luaVm, "log", &LuaExports::log);
	lua_register(luaVm, "showWorldMessage", &LuaExports::showWorldMessage);
	lua_register(luaVm, "showGlobalMessage", &LuaExports::showGlobalMessage);

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

	// NPC
	lua_register(luaVm, "isBusy", &LuaExports::isBusy);
	lua_register(luaVm, "removeNPC", &LuaExports::removeNpc);
	lua_register(luaVm, "runNPC", &LuaExports::runNpc);
	lua_register(luaVm, "showShop", &LuaExports::showShop);
	lua_register(luaVm, "spawnNPC", &LuaExports::spawnNpc);

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
	lua_register(luaVm, "changeMaplePoints", &LuaExports::changeMaplePoints);
	lua_register(luaVm, "changeNxCredit", &LuaExports::changeNxCredit);
	lua_register(luaVm, "changeNxPrepaid", &LuaExports::changeNxPrepaid);
	lua_register(luaVm, "deletePlayerVariable", &LuaExports::deletePlayerVariable);
	lua_register(luaVm, "endMorph", &LuaExports::endMorph);
	lua_register(luaVm, "getAllianceID", &LuaExports::getAllianceId);
	lua_register(luaVm, "getAllianceRank", &LuaExports::getAllianceRank);
	lua_register(luaVm, "getAP", &LuaExports::getAp);
	lua_register(luaVm, "getDEX", &LuaExports::getDex);
	lua_register(luaVm, "getEXP", &LuaExports::getExp);
	lua_register(luaVm, "getEyes", &LuaExports::getEyes);
	lua_register(luaVm, "getFame", &LuaExports::getFame);
	lua_register(luaVm, "getFH", &LuaExports::getFh);
	lua_register(luaVm, "getGender", &LuaExports::getGender);
	lua_register(luaVm, "getGMLevel", &LuaExports::getGmLevel);
	lua_register(luaVm, "getGuildID", &LuaExports::getGuildId);
	lua_register(luaVm, "getGuildRank", &LuaExports::getGuildRank);
	lua_register(luaVm, "getHair", &LuaExports::getHair);
	lua_register(luaVm, "getHP", &LuaExports::getHp);
	lua_register(luaVm, "getHPMPAP", &LuaExports::getHpMpAp);
	lua_register(luaVm, "getID", &LuaExports::getId);
	lua_register(luaVm, "getINT", &LuaExports::getInt);
	lua_register(luaVm, "getJob", &LuaExports::getJob);
	lua_register(luaVm, "getLevel", &LuaExports::getLevel);
	lua_register(luaVm, "getLUK", &LuaExports::getLuk);
	lua_register(luaVm, "getMap", &LuaExports::getMap);
	lua_register(luaVm, "getMaplePoints", &LuaExports::getMaplePoints);
	lua_register(luaVm, "getMaxHP", &LuaExports::getMaxHp);
	lua_register(luaVm, "getMaxMP", &LuaExports::getMaxMp);
	lua_register(luaVm, "getMP", &LuaExports::getMp);
	lua_register(luaVm, "getName", &LuaExports::getName);
	lua_register(luaVm, "getNxCredit", &LuaExports::getNxCredit);
	lua_register(luaVm, "getNxPrepaid", &LuaExports::getNxPrepaid);
	lua_register(luaVm, "getPlayerVariable", &LuaExports::getPlayerVariable);
	lua_register(luaVm, "getPosX", &LuaExports::getPosX);
	lua_register(luaVm, "getPosY", &LuaExports::getPosY);
	lua_register(luaVm, "getRealMaxHP", &LuaExports::getRealMaxHp);
	lua_register(luaVm, "getRealMaxMP", &LuaExports::getRealMaxMp);
	lua_register(luaVm, "getSkin", &LuaExports::getSkin);
	lua_register(luaVm, "getSP", &LuaExports::getSp);
	lua_register(luaVm, "getSTR", &LuaExports::getStr);
	lua_register(luaVm, "giveAP", &LuaExports::giveAp);
	lua_register(luaVm, "giveEXP", &LuaExports::giveExp);
	lua_register(luaVm, "giveFame", &LuaExports::giveFame);
	lua_register(luaVm, "giveSP", &LuaExports::giveSp);
	lua_register(luaVm, "isActiveItem", &LuaExports::isActiveItem);
	lua_register(luaVm, "isActiveSkill", &LuaExports::isActiveSkill);
	lua_register(luaVm, "isGM", &LuaExports::isGm);
	lua_register(luaVm, "isOnline", &LuaExports::isOnline);
	lua_register(luaVm, "revertPlayer", &LuaExports::revertPlayer);
	lua_register(luaVm, "setAP", &LuaExports::setAp);
	lua_register(luaVm, "setDEX", &LuaExports::setDex);
	lua_register(luaVm, "setEXP", &LuaExports::setExp);
	lua_register(luaVm, "setHP", &LuaExports::setHp);
	lua_register(luaVm, "setINT", &LuaExports::setInt);
	lua_register(luaVm, "setJob", &LuaExports::setJob);
	lua_register(luaVm, "setLevel", &LuaExports::setLevel);
	lua_register(luaVm, "setLUK", &LuaExports::setLuk);
	lua_register(luaVm, "setMap", &LuaExports::setMap);
	lua_register(luaVm, "setMaxHP", &LuaExports::setMaxHp);
	lua_register(luaVm, "setMaxMP", &LuaExports::setMaxMp);
	lua_register(luaVm, "setMP", &LuaExports::setMp);
	lua_register(luaVm, "setPlayer", &LuaExports::setPlayer);
	lua_register(luaVm, "setPlayerVariable", &LuaExports::setPlayerVariable);
	lua_register(luaVm, "setSP", &LuaExports::setSp);
	lua_register(luaVm, "setSTR", &LuaExports::setStr);
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
	lua_register(luaVm, "getAllMapPlayerIDs", &LuaExports::getAllMapPlayerIds);
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
	lua_register(luaVm, "getMobFH", &LuaExports::getMobFh);
	lua_register(luaVm, "getMobHP", &LuaExports::getMobHp);
	lua_register(luaVm, "getMobMaxHP", &LuaExports::getMobMaxHp);
	lua_register(luaVm, "getMobMaxMP", &LuaExports::getMobMaxMp);
	lua_register(luaVm, "getMobMP", &LuaExports::getMobMp);
	lua_register(luaVm, "getMobPosX", &LuaExports::getMobPosX);
	lua_register(luaVm, "getMobPosY", &LuaExports::getMobPosY);
	lua_register(luaVm, "getRealMobID", &LuaExports::getRealMobId);
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
	lua_register(luaVm, "getTickCount", &LuaExports::getTickCount);
	lua_register(luaVm, "getSecondsSinceEpoch", &LuaExports::getSecondsSinceEpoch);
	lua_register(luaVm, "isDST", &LuaExports::isDst);

	// Rates
	lua_register(luaVm, "getDropRate", &LuaExports::getDropRate);
	lua_register(luaVm, "getEXPRate", &LuaExports::getExpRate);
	lua_register(luaVm, "getMesoRate", &LuaExports::getMesoRate);
	lua_register(luaVm, "getQuestEXPRate", &LuaExports::getQuestExpRate);

	// Party
	lua_register(luaVm, "checkPartyFootholds", &LuaExports::checkPartyFootholds);
	lua_register(luaVm, "getAllPartyPlayerIDs", &LuaExports::getAllPartyPlayerIds);
	lua_register(luaVm, "getPartyCount", &LuaExports::getPartyCount);
	lua_register(luaVm, "getPartyID", &LuaExports::getPartyId);
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
	lua_register(luaVm, "getAllInstancePlayerIDs", &LuaExports::getAllInstancePlayerIds);
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

	// Guild
	lua_register(luaVm, "addGuildPoint", &LuaExports::addGuildPoint);
	lua_register(luaVm, "disbandGuild", &LuaExports::disbandGuild);
	lua_register(luaVm, "displayGuildRankBoard", &LuaExports::displayGuildRankBoard);
	lua_register(luaVm, "getGuildCapacity", &LuaExports::getGuildCapacity);
	lua_register(luaVm, "hasEmblem", &LuaExports::hasEmblem);
	lua_register(luaVm, "removeEmblem", &LuaExports::removeEmblem);
	lua_register(luaVm, "sendChangeGuildEmblem", &LuaExports::sendChangeGuildEmblem);
	lua_register(luaVm, "sendIncreaseCapacity", &LuaExports::increaseGuildCapacity);
	lua_register(luaVm, "sendNewGuildWindow", &LuaExports::sendNewGuildWindow);

	// Alliance
	lua_register(luaVm, "createAlliance", &LuaExports::createAlliance);
	lua_register(luaVm, "disbandAlliance", &LuaExports::disbandAlliance);
	lua_register(luaVm, "getAllianceCapacity", &LuaExports::getAllianceCapacity);
	lua_register(luaVm, "increaseAllianceCapacity", &LuaExports::increaseAllianceCapacity);
}

bool LuaScriptable::run() {
	if (luaL_dofile(luaVm, filename.c_str())) {
		// Error in lua script
		handleError();
		return false;
	}
	return true;
}

void LuaScriptable::setVariable(const string &name, int32_t val) {
	lua_pushinteger(luaVm, val);
	lua_setglobal(luaVm, name.c_str());
}

void LuaScriptable::setVariable(const string &name, const string &val) {
	lua_pushstring(luaVm, val.c_str());
	lua_setglobal(luaVm, name.c_str());
}

void LuaScriptable::handleError() {
	printError(lua_tostring(luaVm, -1));
}

void LuaScriptable::printError(const string &error) {
	std::cout << error << std::endl;

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);

	if (player == nullptr) {
		std::cout << "Script error in " << filename << ": " << error << std::endl;
		return;
	}

	if (player->isGm()) {
		PlayerPacket::showMessage(player, error, PlayerPacket::NoticeTypes::Blue);
	}
	else {
		PlayerPacket::showMessage(player, "There is an error in the script '" + filename +"'", PlayerPacket::NoticeTypes::Blue);
	}
}

// Lua Exports
Player * LuaExports::getPlayer(lua_State *luaVm) {
	lua_getglobal(luaVm, "_playerid");
	return PlayerDataProvider::Instance()->getPlayer(lua_tointeger(luaVm, -1));
}

Player * LuaExports::getPlayerDeduced(int parameter, lua_State *luaVm) {
	Player *player = nullptr;
	if (lua_type(luaVm, parameter) == LUA_TSTRING) {
		player = PlayerDataProvider::Instance()->getPlayer(lua_tostring(luaVm, parameter));
	}
	else {
		player = PlayerDataProvider::Instance()->getPlayer(lua_tointeger(luaVm, parameter));
	}
	return player;
}

Instance * LuaExports::getInstance(lua_State *luaVm) {
	lua_getglobal(luaVm, "_instancename");
	return Instances::InstancePtr()->getInstance(lua_tostring(luaVm, -1));
}

// Miscellaneous
int LuaExports::consoleOutput(lua_State *luaVm) {
	std::cout << lua_tostring(luaVm, 1) << std::endl;
	return 0;
}

int LuaExports::getRandomNumber(lua_State *luaVm) {
	int32_t number = lua_tointeger(luaVm, -1);
	lua_pushinteger(luaVm, Randomizer::Instance()->randInt(number, 1));
	return 1;
}

int LuaExports::log(lua_State *luaVm) {
	ChannelServer::Instance()->log(LogTypes::ScriptLog, lua_tostring(luaVm, 1));
	return 0;
}

int LuaExports::showWorldMessage(lua_State *luaVm) {
	string msg = lua_tostring(luaVm, -2);
	int8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessageWorld(msg, type);
	return 0;
}

int LuaExports::showGlobalMessage(lua_State *luaVm) {
	string msg = lua_tostring(luaVm, -2);
	int8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessageGlobal(msg, type);
	return 0;
}

// Channel
int LuaExports::deleteChannelVariable(lua_State *luaVm) {
	string key = string(lua_tostring(luaVm, -1));
	EventDataProvider::InstancePtr()->getVariables()->deleteVariable(key);
	return 0;
}

int LuaExports::getChannel(lua_State *luaVm) {
	lua_pushnumber(luaVm, ChannelServer::Instance()->getChannel() + 1);
	return 1;
}

int LuaExports::getChannelVariable(lua_State *luaVm) {
	bool integral = false;
	if (lua_isboolean(luaVm, 2)) {
		integral = true;
	}
	string val = EventDataProvider::InstancePtr()->getVariables()->getVariable(lua_tostring(luaVm, 1));
	if (integral) {
		if (val == "") {
			lua_pushnil(luaVm);
		}
		else {
			lua_pushinteger(luaVm, boost::lexical_cast<int32_t>(val));
		}
	}
	else {
		lua_pushstring(luaVm, val.c_str());
	}
	return 1;
}

int LuaExports::isHorntailChannel(lua_State *luaVm) {
	lua_pushboolean(luaVm, ChannelServer::Instance()->isHorntailChannel());
	return 1;
}

int LuaExports::isPapChannel(lua_State *luaVm) {
	lua_pushboolean(luaVm, ChannelServer::Instance()->isPapChannel());
	return 1;
}

int LuaExports::isPianusChannel(lua_State *luaVm) {
	lua_pushboolean(luaVm, ChannelServer::Instance()->isPianusChannel());
	return 1;
}

int LuaExports::isPinkBeanChannel(lua_State *luaVm) {
	lua_pushboolean(luaVm, ChannelServer::Instance()->isPinkBeanChannel());
	return 1;
}

int LuaExports::isZakumChannel(lua_State *luaVm) {
	lua_pushboolean(luaVm, ChannelServer::Instance()->isZakumChannel());
	return 1;
}

int LuaExports::setChannelVariable(lua_State *luaVm) {
	string value = string(lua_tostring(luaVm, -1));
	string key = string(lua_tostring(luaVm, -2));
	EventDataProvider::InstancePtr()->getVariables()->setVariable(key, value);
	return 0;
}

int LuaExports::showChannelMessage(lua_State *luaVm) {
	string msg = lua_tostring(luaVm, -2);
	int8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessageChannel(msg, type);
	return 0;
}

// Bosses
int LuaExports::getHorntailChannels(lua_State *luaVm) {
	vector<int8_t> channels = ChannelServer::Instance()->getHorntailChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getMaxHorntailBattles(lua_State *luaVm) {
	lua_pushinteger(luaVm, ChannelServer::Instance()->getHorntailAttempts());
	return 1;
}

int LuaExports::getMaxPapBattles(lua_State *luaVm) {
	lua_pushinteger(luaVm, ChannelServer::Instance()->getPapAttempts());
	return 1;
}

int LuaExports::getMaxPianusBattles(lua_State *luaVm) {
	lua_pushinteger(luaVm, ChannelServer::Instance()->getPianusAttempts());
	return 1;
}

int LuaExports::getMaxPinkBeanBattles(lua_State *luaVm) {
	lua_pushinteger(luaVm, ChannelServer::Instance()->getPinkBeanAttempts());
	return 1;
}

int LuaExports::getMaxZakumBattles(lua_State *luaVm) {
	lua_pushinteger(luaVm, ChannelServer::Instance()->getZakumAttempts());
	return 1;
}

int LuaExports::getPapChannels(lua_State *luaVm) {
	vector<int8_t> channels = ChannelServer::Instance()->getPapChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getPianusChannels(lua_State *luaVm) {
	vector<int8_t> channels = ChannelServer::Instance()->getPianusChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getPinkBeanChannels(lua_State *luaVm) {
	vector<int8_t> channels = ChannelServer::Instance()->getPinkBeanChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getZakumChannels(lua_State *luaVm) {
	vector<int8_t> channels = ChannelServer::Instance()->getZakumChannels();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < channels.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, channels[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

// NPC
int LuaExports::isBusy(lua_State *luaVm) {
	lua_pushboolean(luaVm, getPlayer(luaVm)->getNpc() != nullptr);
	return 1;
}

int LuaExports::removeNpc(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	uint32_t index = lua_tointeger(luaVm, 2);
	Maps::getMap(mapid)->removeNpc(index);
	return 0;
}

int LuaExports::runNpc(lua_State *luaVm) {
	int32_t npcid = lua_tointeger(luaVm, -1);
	string script;
	if (lua_type(luaVm, 2) == LUA_TSTRING) { // We already have our script name
		string specified = lua_tostring(luaVm, 2);
		script = "scripts/npcs/" + specified + ".lua";
	}
	else {
		script = ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	Npc *npc = new Npc(npcid, getPlayer(luaVm), script);
	npc->run();
	return 0;
}

int LuaExports::showShop(lua_State *luaVm) {
	int32_t shopid = lua_tointeger(luaVm, -1);
	NpcHandler::showShop(getPlayer(luaVm), shopid);
	return 0;
}

int LuaExports::spawnNpc(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t npcid = lua_tointeger(luaVm, 2);
	int16_t x = lua_tointeger(luaVm, 3);
	int16_t y = lua_tointeger(luaVm, 4);

	NpcSpawnInfo npc;
	npc.id = npcid;
	npc.foothold = 0;
	npc.pos = Pos(x, y);
	npc.rx0 = x - 50;
	npc.rx1 = x + 50;

	lua_pushinteger(luaVm, Maps::getMap(mapid)->addNpc(npc));
	return 1;
}

// Beauty
int LuaExports::getAllFaces(lua_State *luaVm) {
	vector<int32_t> ids = BeautyDataProvider::Instance()->getFaces(getPlayer(luaVm)->getGender());
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getAllHair(lua_State *luaVm) {
	vector<int32_t> ids = BeautyDataProvider::Instance()->getHair(getPlayer(luaVm)->getGender());
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getAllSkins(lua_State *luaVm) {
	vector<int8_t> ids = BeautyDataProvider::Instance()->getSkins();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getRandomFace(lua_State *luaVm) {
	lua_pushinteger(luaVm, BeautyDataProvider::Instance()->getRandomFace(getPlayer(luaVm)->getGender()));
	return 1;
}

int LuaExports::getRandomHair(lua_State *luaVm) {
	lua_pushinteger(luaVm, BeautyDataProvider::Instance()->getRandomHair(getPlayer(luaVm)->getGender()));
	return 1;
}

int LuaExports::getRandomSkin(lua_State *luaVm) {
	lua_pushinteger(luaVm, BeautyDataProvider::Instance()->getRandomSkin());
	return 1;
}

int LuaExports::isValidFace(lua_State *luaVm) {
	lua_pushboolean(luaVm, BeautyDataProvider::Instance()->isValidFace(getPlayer(luaVm)->getGender(), lua_tointeger(luaVm, 1)));
	return 1;
}

int LuaExports::isValidHair(lua_State *luaVm) {
	lua_pushboolean(luaVm, BeautyDataProvider::Instance()->isValidHair(getPlayer(luaVm)->getGender(), lua_tointeger(luaVm, 1)));
	return 1;
}

int LuaExports::isValidSkin(lua_State *luaVm) {
	lua_pushboolean(luaVm, BeautyDataProvider::Instance()->isValidSkin(lua_tointeger(luaVm, 1)));
	return 1;
}

// Buddy
int LuaExports::addBuddySlots(lua_State *luaVm) {
	Player *p = getPlayer(luaVm);
	uint8_t csize = p->getBuddyListSize();
	int8_t mod = lua_tointeger(luaVm, 1);
	p->setBuddyListSize(csize + mod);
	return 0;
}

int LuaExports::getBuddySlots(lua_State *luaVm) {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getBuddyListSize());
	return 1;
}

// Skill
int LuaExports::addSkillLevel(lua_State *luaVm) {
	int32_t skillid = lua_tointeger(luaVm, 1);
	uint8_t level = lua_tointeger(luaVm, 2);

	if (lua_isnumber(luaVm, 3)) { // Optional argument of increasing a skill's max level
		getPlayer(luaVm)->getSkills()->setMaxSkillLevel(skillid, lua_tointeger(luaVm, 3));
	}

	getPlayer(luaVm)->getSkills()->addSkillLevel(skillid, level);
	return 0;
}

int LuaExports::getSkillLevel(lua_State *luaVm) {
	int32_t skillid = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getSkills()->getSkillLevel(skillid));
	return 1;
}

int LuaExports::getMaxSkillLevel(lua_State *luaVm) {
	int32_t skillid = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getSkills()->getMaxSkillLevel(skillid));
	return 1;
}

int LuaExports::setMaxSkillLevel(lua_State *luaVm) {
	int32_t skillid = lua_tointeger(luaVm, -2);
	uint8_t level = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getSkills()->setMaxSkillLevel(skillid, level);
	return 0;
}

// Quest
int LuaExports::getQuestData(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, 1);
	lua_pushstring(luaVm, getPlayer(luaVm)->getQuests()->getQuestData(questid).c_str());
	return 1;
}

int LuaExports::isQuestActive(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	lua_pushboolean(luaVm, getPlayer(luaVm)->getQuests()->isQuestActive(questid));
	return 1;
}

int LuaExports::isQuestInactive(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	bool active = !(getPlayer(luaVm)->getQuests()->isQuestActive(questid) || getPlayer(luaVm)->getQuests()->isQuestComplete(questid));
	lua_pushboolean(luaVm, active);
	return 1;
}

int LuaExports::isQuestCompleted(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	lua_pushboolean(luaVm, getPlayer(luaVm)->getQuests()->isQuestComplete(questid));
	return 1;
}

int LuaExports::setQuestData(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, 1);
	string data = lua_tostring(luaVm, 2);
	getPlayer(luaVm)->getQuests()->setQuestData(questid, data);
	return 0;
}

// Inventory
int LuaExports::addSlots(lua_State *luaVm) {
	int8_t inventory = lua_tointeger(luaVm, -2);
	int8_t rows = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getInventory()->addMaxSlots(inventory, rows);
	return 0;
}

int LuaExports::addStorageSlots(lua_State *luaVm) {
	int8_t slots = lua_tointeger(luaVm, 1);
	getPlayer(luaVm)->getStorage()->setSlots(getPlayer(luaVm)->getStorage()->getSlots() + slots);
	return 0;
}

int LuaExports::getEquippedItemInSlot(lua_State *luaVm) {
	int16_t slot = lua_tointeger(luaVm, 1);
	lua_pushinteger(luaVm, getPlayer(luaVm)->getInventory()->getEquippedId(slot));
	return 1;
}

int LuaExports::getItemAmount(lua_State *luaVm) {
	int32_t itemid = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getInventory()->getItemAmount(itemid));
	return 1;
}

int LuaExports::getMesos(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getInventory()->getMesos());
	return 1;
}

int LuaExports::getOpenSlots(lua_State *luaVm) {
	int8_t inv = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->getInventory()->getOpenSlotsNum(inv));
	return 1;
}

int LuaExports::giveItem(lua_State *luaVm) {
	int32_t itemid = lua_tointeger(luaVm, 1);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 2)) {
		amount = lua_tointeger(luaVm, 2);
	}
	bool success = Quests::giveItem(getPlayer(luaVm), itemid, amount);
	lua_pushboolean(luaVm, success);
	return 1;
}

int LuaExports::giveMesos(lua_State *luaVm) {
	int32_t mesos = lua_tointeger(luaVm, -1);
	bool success = Quests::giveMesos(getPlayer(luaVm), mesos);
	lua_pushboolean(luaVm, success);
	return 1;
}

int LuaExports::hasOpenSlotsFor(lua_State *luaVm) {
	int32_t itemid = lua_tointeger(luaVm, 1);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 2))
		amount = lua_tointeger(luaVm, 2);
	lua_pushboolean(luaVm, getPlayer(luaVm)->getInventory()->hasOpenSlotsFor(itemid, amount));
	return 1;
}

int LuaExports::isEquippedItem(lua_State *luaVm) {
	int32_t itemid = lua_tointeger(luaVm, 1);
	lua_pushboolean(luaVm, getPlayer(luaVm)->getInventory()->isEquippedItem(itemid));
	return 1;
}

int LuaExports::useItem(lua_State *luaVm) {
	int32_t itemid = lua_tointeger(luaVm, -1);
	Inventory::useItem(getPlayer(luaVm), itemid);
	return 0;
}

// Player
int LuaExports::changeMaplePoints(lua_State *luaVm) {
	int32_t value = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStorage()->changeMaplePoints(value);
	return 0;
}

int LuaExports::changeNxCredit(lua_State *luaVm) {
	int32_t value = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStorage()->changeNxCredit(value);
	return 0;
}

int LuaExports::changeNxPrepaid(lua_State *luaVm) {
	int32_t value = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStorage()->changeNxPrepaid(value);
	return 0;
}

int LuaExports::deletePlayerVariable(lua_State *luaVm) {
	string key = string(lua_tostring(luaVm, -1));
	getPlayer(luaVm)->getVariables()->deleteVariable(key);
	return 0;
}

int LuaExports::endMorph(lua_State *luaVm) {
	getPlayer(luaVm)->getActiveBuffs()->endMorph();
	return 0;
}

int LuaExports::getAllianceId(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getAllianceId());
	return 1;
}

int LuaExports::getAllianceRank(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getAllianceRank());
	return 1;
}

int LuaExports::getAp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getAp());
	return 1;
}

int LuaExports::getDex(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getDex());
	return 1;
}

int LuaExports::getExp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getExp());
	return 1;
}

int LuaExports::getEyes(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getEyes());
	return 1;
}

int LuaExports::getFame(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getFame());
	return 1;
}

int LuaExports::getFh(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getFh());
	return 1;
}

int LuaExports::getGender(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getGender());
	return 1;
}

int LuaExports::getGmLevel(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getGmLevel());
	return 1;
}

int LuaExports::getHair(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getHair());
	return 1;
}

int LuaExports::getHp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getHp());
	return 1;
}

int LuaExports::getHpMpAp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getHpMpAp());
	return 1;
}

int LuaExports::getId(lua_State *luaVm) {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getId());
	return 1;
}

int LuaExports::getInt(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getInt());
	return 1;
}

int LuaExports::getJob(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getJob());
	return 1;
}

int LuaExports::getLevel(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getLevel());
	return 1;
}

int LuaExports::getLuk(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getLuk());
	return 1;
}

int LuaExports::getGuildId(lua_State *luaVm) {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getGuildId());
	return 1;
}

int LuaExports::getGuildRank(lua_State *luaVm) {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getGuildRank());
	return 1;
}

int LuaExports::getMap(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getMap());
	return 1;
}

int LuaExports::getMaplePoints(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStorage()->getMaplePoints());
	return 1;
}

int LuaExports::getMaxHp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxHp());
	return 1;
}

int LuaExports::getMaxMp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxMp());
	return 1;
}

int LuaExports::getMp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMp());
	return 1;
}

int LuaExports::getName(lua_State *luaVm) {
	lua_pushstring(luaVm, getPlayer(luaVm)->getName().c_str());
	return 1;
}

int LuaExports::getPlayerVariable(lua_State *luaVm) {
	bool integral = false;
	if (lua_isboolean(luaVm, 2)) {
		integral = true;
	}
	string val = getPlayer(luaVm)->getVariables()->getVariable(lua_tostring(luaVm, 1));
	if (integral) {
		if (val == "") {
			lua_pushnil(luaVm);
		}
		else {
			lua_pushinteger(luaVm, boost::lexical_cast<int32_t>(val));
		}
	}
	else {
		lua_pushstring(luaVm, val.c_str());
	}
	return 1;
}

int LuaExports::getNxCredit(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStorage()->getNxCredit());
	return 1;
}

int LuaExports::getNxPrepaid(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStorage()->getNxPrepaid());
	return 1;
}

int LuaExports::getPosX(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getPos().x);
	return 1;
}

int LuaExports::getPosY(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getPos().y);
	return 1;
}

int LuaExports::getRealMaxHp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxHp(true));
	return 1;
}

int LuaExports::getRealMaxMp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getMaxMp(true));
	return 1;
}

int LuaExports::getSkin(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getSkin());
	return 1;
}

int LuaExports::getSp(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getSp());
	return 1;
}

int LuaExports::getStr(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStats()->getStr());
	return 1;
}

int LuaExports::giveAp(lua_State *luaVm) {
	int16_t ap = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setAp(getPlayer(luaVm)->getStats()->getAp() + ap);
	return 0;
}

int LuaExports::giveExp(lua_State *luaVm) {
	int32_t exp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->giveExp(exp, true);
	return 0;
}

int LuaExports::giveFame(lua_State *luaVm) {
	int32_t fame = lua_tointeger(luaVm, 1);
	Quests::giveFame(getPlayer(luaVm), fame);
	return 0;
}

int LuaExports::giveSp(lua_State *luaVm) {
	int16_t sp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setSp(getPlayer(luaVm)->getStats()->getSp() + sp);
	return 0;
}

int LuaExports::isActiveItem(lua_State *luaVm) {
	lua_pushboolean(luaVm, getPlayer(luaVm)->getActiveBuffs()->getActiveSkillLevel(-1 * lua_tointeger(luaVm, -1)) > 0);
	return 1;
}

int LuaExports::isActiveSkill(lua_State *luaVm) {
	lua_pushboolean(luaVm, getPlayer(luaVm)->getActiveBuffs()->getActiveSkillLevel(lua_tointeger(luaVm, -1)) > 0);
	return 1;
}

int LuaExports::isGm(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->isGm());
	return 1;
}

int LuaExports::isOnline(lua_State *luaVm) {
	lua_pushboolean(luaVm, getPlayerDeduced(-1, luaVm) != nullptr);
	return 1;
}

int LuaExports::revertPlayer(lua_State *luaVm) {
	lua_getglobal(luaVm, "oldplayerid");
	lua_setglobal(luaVm, "playerid");
	return 0;
}

int LuaExports::setAp(lua_State *luaVm) {
	int16_t ap = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setAp(ap);
	return 0;
}

int LuaExports::setDex(lua_State *luaVm) {
	int16_t dex = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setDex(dex);
	return 0;
}

int LuaExports::setExp(lua_State *luaVm) {
	int32_t exp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setExp(exp);
	return 0;
}

int LuaExports::setHp(lua_State *luaVm) {
	uint16_t hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setHp(hp);
	return 0;
}

int LuaExports::setInt(lua_State *luaVm) {
	int16_t intt = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setInt(intt);
	return 0;
}

int LuaExports::setJob(lua_State *luaVm) {
	int16_t job = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setJob(job);
	return 0;
}

int LuaExports::setLevel(lua_State *luaVm) {
	uint8_t level = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setLevel(level);
	return 0;
}

int LuaExports::setLuk(lua_State *luaVm) {
	int16_t luk = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setLuk(luk);
	return 0;
}

int LuaExports::setMap(lua_State *luaVm) {
	PortalInfo *portal = nullptr;

	int32_t mapid = lua_tointeger(luaVm, 1);

	if (lua_isstring(luaVm, 2)) { // Optional portal parameter
		string to = lua_tostring(luaVm, 2);
		portal = Maps::getMap(mapid)->getPortal(to);
	}

	if (Maps::getMap(mapid))
		getPlayer(luaVm)->setMap(mapid, portal);
	return 0;
}

int LuaExports::setMaxHp(lua_State *luaVm) {
	uint16_t hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setMaxHp(hp);
	return 0;
}

int LuaExports::setMaxMp(lua_State *luaVm) {
	uint16_t mp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setMaxMp(mp);
	return 0;
}

int LuaExports::setMp(lua_State *luaVm) {
	uint16_t mp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setMp(mp);
	return 0;
}

int LuaExports::setPlayer(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	if (player != nullptr) {
		lua_getglobal(luaVm, "playerid");
		lua_setglobal(luaVm, "oldplayerid");

		lua_pushinteger(luaVm, player->getId());
		lua_setglobal(luaVm, "playerid");
	}
	lua_pushboolean(luaVm, player != nullptr);
	return 1;
}

int LuaExports::setPlayerVariable(lua_State *luaVm) {
	string value = string(lua_tostring(luaVm, -1));
	string key = string(lua_tostring(luaVm, -2));
	getPlayer(luaVm)->getVariables()->setVariable(key, value);
	return 0;
}

int LuaExports::setSp(lua_State *luaVm) {
	int16_t sp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setSp(sp);
	return 0;
}

int LuaExports::setStr(lua_State *luaVm) {
	int16_t str = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getStats()->setStr(str);
	return 0;
}

int LuaExports::setStyle(lua_State *luaVm) {
	int32_t id = lua_tointeger(luaVm, -1);
	int32_t type = id / 10000;
	if (type == 0) {
		getPlayer(luaVm)->setSkin((int8_t)id);
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

int LuaExports::showInstructionBubble(lua_State *luaVm) {
	string msg = lua_tostring(luaVm, 1);
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

int LuaExports::showMessage(lua_State *luaVm) {
	string msg = lua_tostring(luaVm, -2);
	uint8_t type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessage(getPlayer(luaVm), msg, type);
	return 0;
}

// Effects
int LuaExports::playFieldSound(lua_State *luaVm) {
	string val = lua_tostring(luaVm, 1);
	if (lua_isnumber(luaVm, 2)) {
		EffectPacket::sendFieldSound(lua_tointeger(luaVm, 2), val);
	}
	else {
		EffectPacket::sendFieldSound(getPlayer(luaVm), val);
	}
	return 0;
}

int LuaExports::playMinigameSound(lua_State *luaVm) {
	string val = lua_tostring(luaVm, 1);
	if (lua_isnumber(luaVm, 2)) {
		EffectPacket::sendMinigameSound(lua_tointeger(luaVm, 2), val);
	}
	else {
		EffectPacket::sendMinigameSound(getPlayer(luaVm), val);
	}
	return 0;
}

int LuaExports::setMusic(lua_State *luaVm) {
	int32_t mapid = -1;
	string music = lua_tostring(luaVm, 1);

	if (lua_isnumber(luaVm, 2)) {
		mapid = lua_tointeger(luaVm, 2);
	}
	else if (Player *player = getPlayer(luaVm)) {
		mapid = player->getMap();
	}

	if (mapid != -1) {
		Maps::getMap(mapid)->setMusic(music);
	}
	return 0;
}

int LuaExports::showMapEffect(lua_State *luaVm) {
	string val = lua_tostring(luaVm, -1);
	EffectPacket::sendEffect(getPlayer(luaVm)->getMap(), val);
	return 0;
}

int LuaExports::showMapEvent(lua_State *luaVm) {
	string val = lua_tostring(luaVm, -1);
	EffectPacket::sendEvent(getPlayer(luaVm)->getMap(), val);
	return 0;
}

// Map
int LuaExports::clearDrops(lua_State *luaVm) {
	int32_t mapid = -1;
	if (lua_type(luaVm, 1) == LUA_TNUMBER) {
		mapid = lua_tointeger(luaVm, 1);
	}
	else if (Player *player = getPlayer(luaVm)) {
		mapid = player->getMap();
	}

	if (mapid != -1) {
		Maps::getMap(mapid)->clearDrops(false);
	}
	return 0;
}

int LuaExports::clearMobs(lua_State *luaVm) {
	int32_t mapid = 0;
	if (lua_type(luaVm, 1) == LUA_TNUMBER) {
		mapid = lua_tointeger(luaVm, 1);
	}
	else {
		mapid = getPlayer(luaVm)->getMap();
	}
	Maps::getMap(mapid)->killMobs(0, 0, false, false);
	return 0;
}

int LuaExports::countMobs(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mobid = 0;
	if (lua_isnumber(luaVm, 2))
		mobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->countMobs(mobid));
	return 1;
}

int LuaExports::getAllMapPlayerIds(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	if (Map *map = Maps::getMap(mapid)) {
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

int LuaExports::getNumPlayers(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, -1);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getNumPlayers());
	return 1;
}

int LuaExports::getReactorState(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, -2);
	int32_t reactorid = lua_tointeger(luaVm, -1);
	for (uint32_t i = 0; i < Maps::getMap(mapid)->getNumReactors(); i++) {
		if (Maps::getMap(mapid)->getReactor(i)->getReactorId() == reactorid) {
			lua_pushinteger(luaVm, Maps::getMap(mapid)->getReactor(i)->getState());
			return 1;
		}
	}
	lua_pushinteger(luaVm, 0);
	return 1;
}

int LuaExports::killMobs(lua_State *luaVm) {
	int32_t mobid = lua_tointeger(luaVm, 1);
	int32_t mapid = getPlayer(luaVm)->getMap();
	bool playerkill = true;
	if (lua_isboolean(luaVm, 2))
		playerkill = (lua_toboolean(luaVm, 2) == 1 ? true : false);
	int32_t killed = Maps::getMap(mapid)->killMobs(getPlayer(luaVm), mobid, playerkill, true);
	lua_pushinteger(luaVm, killed);
	return 1;
}

int LuaExports::setMapSpawn(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t spawn = lua_tointeger(luaVm, 2);
	Maps::getMap(mapid)->setMobSpawning(spawn);
	return 0;
}

int LuaExports::setReactorState(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, -3);
	int32_t reactorid = lua_tointeger(luaVm, -2);
	uint8_t state = lua_tointeger(luaVm, -1);
	for (size_t i = 0; i < Maps::getMap(mapid)->getNumReactors(); i++) {
		Reactor *reactor = Maps::getMap(mapid)->getReactor(i);
		if (reactor->getReactorId() == reactorid) {
			reactor->setState(state, true);
			break;
		}
	}
	return 0;
}

int LuaExports::showMapMessage(lua_State *luaVm) {
	string msg = lua_tostring(luaVm, -2);
	uint8_t type = lua_tointeger(luaVm, -1);
	int32_t map = getPlayer(luaVm)->getMap();
	Maps::getMap(map)->showMessage(msg, type);
	return 0;
}

int LuaExports::showMapTimer(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t time = lua_tointeger(luaVm, 2);
	Maps::getMap(mapid)->setMapTimer(time);
	return 0;
}

int LuaExports::spawnMob(lua_State *luaVm) {
	int32_t mobid = lua_tointeger(luaVm, -1);
	Player *player = getPlayer(luaVm);
	lua_pushinteger(luaVm, Maps::getMap(player->getMap())->spawnMob(mobid, player->getPos()));
	return 1;
}

int LuaExports::spawnMobPos(lua_State *luaVm) {
	int32_t mobid = lua_tointeger(luaVm, 1);
	int16_t x = lua_tointeger(luaVm, 2);
	int16_t y = lua_tointeger(luaVm, 3);
	int16_t fh = 0;
	if (lua_isnumber(luaVm, 4))
		fh = lua_tointeger(luaVm, 4);
	lua_pushinteger(luaVm, Maps::getMap(getPlayer(luaVm)->getMap())->spawnMob(mobid, Pos(x, y), fh));
	return 1;
}

// Mob
int LuaExports::getMobFh(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getFh());
	return 1;
}

int LuaExports::getMobHp(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getHp());
	return 1;
}

int LuaExports::getMobMaxHp(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getMaxHp());
	return 1;
}

int LuaExports::getMobMaxMp(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getMaxMp());
	return 1;
}

int LuaExports::getMobMp(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getMp());
	return 1;
}

int LuaExports::getMobPosX(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getPos().x);
	return 1;
}

int LuaExports::getMobPosY(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getPos().y);
	return 1;
}

int LuaExports::getRealMobId(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	lua_pushinteger(luaVm, Maps::getMap(mapid)->getMob(mapmobid)->getId());
	return 1;
}

int LuaExports::killMob(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	if (Mob *m = Maps::getMap(mapid)->getMob(mapmobid)) {
		m->applyDamage(0, m->getHp());
	}
	return 0;
}

int LuaExports::mobDropItem(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	int32_t mapmobid = lua_tointeger(luaVm, 2);
	int32_t itemid = lua_tointeger(luaVm, 3);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 4)) {
		amount = lua_tointeger(luaVm, 4);
	}
	if (Mob *m = Maps::getMap(mapid)->getMob(mapmobid)) {
		Item f(itemid, amount);
		Drop *drop = new Drop(mapid, f, m->getPos(), 0);
		drop->setTime(0);
		drop->doDrop(m->getPos());
	}
	return 0;
}

// Time
int LuaExports::getDate(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getDate());
	return 1;
}

int LuaExports::getDay(lua_State *luaVm) {
	bool stringreturn = false;
	if (lua_isboolean(luaVm, -1)) {
		stringreturn = (lua_toboolean(luaVm, -1) != 0 ? true : false);
	}
	if (stringreturn) {
		lua_pushstring(luaVm, TimeUtilities::getDayString(false).c_str());
	}
	else {
		lua_pushinteger(luaVm, TimeUtilities::getDay());
	}
	return 1;
}

int LuaExports::getHour(lua_State *luaVm) {
	bool military = false;
	if (lua_isboolean(luaVm, 1)) {
		military = lua_toboolean(luaVm, 1) != 0;
	}
	lua_pushinteger(luaVm, TimeUtilities::getHour(military));
	return 1;
}

int LuaExports::getMinute(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getMinute());
	return 1;
}

int LuaExports::getMonth(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getMonth());
	return 1;
}

int LuaExports::getNearestMinute(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getNearestMinuteMark(lua_tointeger(luaVm, 1)));
	return 1;
}

int LuaExports::getSecond(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getSecond());
	return 1;
}

int LuaExports::getTime(lua_State *luaVm) {
	lua_pushinteger(luaVm, (lua_Integer) time(0)); // Here's to hoping that lua_Integer is an 8-byte type on most platforms!
	return 1;
}

int LuaExports::getTimeZoneOffset(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getTimeZoneOffset());
	return 1;
}

int LuaExports::getWeek(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getWeek());
	return 1;
}

int LuaExports::getYear(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getYear(false));
	return 1;
}

int LuaExports::getTickCount(lua_State *luaVm) {
	lua_pushinteger(luaVm, TimeUtilities::getTickCount());
	return 1;
}

int LuaExports::getSecondsSinceEpoch(lua_State *luaVm) {
	lua_pushinteger(luaVm, (int32_t)time(0));
	return 1;
}

int LuaExports::isDst(lua_State *luaVm) {
	lua_pushboolean(luaVm, TimeUtilities::isDst());
	return 1;
}

// Rates
int LuaExports::getDropRate(lua_State *luaVm) {
	lua_pushnumber(luaVm, ChannelServer::Instance()->getDropRate());
	return 1;
}

int LuaExports::getExpRate(lua_State *luaVm) {
	lua_pushnumber(luaVm, ChannelServer::Instance()->getExpRate());
	return 1;
}

int LuaExports::getQuestExpRate(lua_State *luaVm) {
	lua_pushnumber(luaVm, ChannelServer::Instance()->getQuestExpRate());
	return 1;
}

int LuaExports::getMesoRate(lua_State *luaVm) {
	lua_pushnumber(luaVm, ChannelServer::Instance()->getMesoRate());
	return 1;
}

// Party
int LuaExports::checkPartyFootholds(lua_State *luaVm) {
	int8_t numbermembers = lua_tointeger(luaVm, 1);
	bool winner = false;
	if (Party *p = getPlayer(luaVm)->getParty()) {
		vector<vector<int16_t> > footholds;
		lua_pushnil(luaVm);
		while (lua_next(luaVm, 2)) {
			vector<int16_t> arr;
			lua_pushnil(luaVm);
			while (lua_next(luaVm, -2)) {
				int16_t fh = lua_tointeger(luaVm, -1);
				arr.push_back(fh);
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

int LuaExports::getAllPartyPlayerIds(lua_State *luaVm) {
	if (Party *p = getPlayer(luaVm)->getParty()) {
		vector<int32_t> ids = p->getAllPlayerIds();
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

int LuaExports::getPartyCount(lua_State *luaVm) {
	int32_t mcount = 0;
	if (Party *p = getPlayer(luaVm)->getParty()) {
		mcount = p->getMembersCount();
	}
	lua_pushinteger(luaVm, mcount);
	return 1;
}

int LuaExports::getPartyId(lua_State *luaVm) {
	int32_t pid = 0;
	if (Party *p = getPlayer(luaVm)->getParty()) {
		pid = p->getId();
	}
	lua_pushinteger(luaVm, pid);
	return 1;
}

int LuaExports::getPartyMapCount(lua_State *luaVm) {
	Player *player = getPlayer(luaVm);
	int8_t members = 0;
	if (Party *p = player->getParty()) {
		int32_t mapid = lua_tointeger(luaVm, 1);
		members = p->getMemberCountOnMap(mapid);
	}
	lua_pushinteger(luaVm, members);
	return 1;
}

int LuaExports::isPartyInLevelRange(lua_State *luaVm) {
	Player *player = getPlayer(luaVm);
	bool iswithin = false;
	if (Party *p = player->getParty()) {
		uint8_t lowbound = lua_tointeger(luaVm, 1);
		uint8_t highbound = lua_tointeger(luaVm, 2);
		iswithin = p->isWithinLevelRange(lowbound, highbound);
	}
	lua_pushboolean(luaVm, iswithin);
	return 1;
}

int LuaExports::isPartyLeader(lua_State *luaVm) {
	Player *player = getPlayer(luaVm);
	bool isleader = false;
	if (Party *p = player->getParty()) {
		isleader = player == p->getLeader();
	}
	lua_pushboolean(luaVm, isleader);
	return 1;
}

int LuaExports::verifyPartyFootholds(lua_State *luaVm) {
	int8_t numbermembers = lua_tointeger(luaVm, 1);
	bool winner = false;
	if (Party *p = getPlayer(luaVm)->getParty()) {
		vector<vector<int16_t> > footholds;
		lua_pushnil(luaVm);
		while (lua_next(luaVm, 1)) {
			vector<int16_t> arr;
			lua_pushnil(luaVm);
			while (lua_next(luaVm, -2)) {
				int16_t fh = lua_tointeger(luaVm, -1);
				arr.push_back(fh);
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

int LuaExports::warpParty(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	string to;
	if (lua_isstring(luaVm, 2)) { // Optional portal parameter
		string to = lua_tostring(luaVm, 2);
	}
	Player *player = getPlayer(luaVm);
	if (Party *p = player->getParty()) {
		p->warpAllMembers(mapid, to);
	}
	return 0;
}

// Instance
int LuaExports::addInstanceMap(lua_State *luaVm) {
	int32_t mapid = lua_tointeger(luaVm, 1);
	getInstance(luaVm)->addMap(mapid);
	return 0;
}

int LuaExports::addInstanceParty(lua_State *luaVm) {
	int32_t id = lua_tointeger(luaVm, -1);
	if (Party *p = PlayerDataProvider::Instance()->getParty(id)) {
		getInstance(luaVm)->addParty(p);
	}
	return 0;
}

int LuaExports::addInstancePlayer(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->addPlayer(player);
	return 0;
}

int LuaExports::addPlayerSignUp(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->addPlayerSignUp(player);
	return 0;
}

int LuaExports::banInstancePlayer(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	if (player != nullptr) {
		getInstance(luaVm)->setBanned(player->getName(), true);
	}
	return 0;
}

int LuaExports::checkInstanceTimer(lua_State *luaVm) {
	string name = lua_tostring(luaVm, 1);
	lua_pushinteger(luaVm, getInstance(luaVm)->checkTimer(name));
	return 1;
}

int LuaExports::createInstance(lua_State *luaVm) {
	string name = lua_tostring(luaVm, 1);
	int32_t time = lua_tointeger(luaVm, 2);
	bool showtimer = lua_toboolean(luaVm, 3) != 0;
	int32_t persistent = 0;
	Player *player = getPlayer(luaVm);
	int32_t map = 0;
	int32_t id = 0;
	if (lua_isnumber(luaVm, 4)) {
		persistent = lua_tointeger(luaVm, 4);
	}
	if (player != nullptr) {
		map = player->getMap();
		id = player->getId();
	}
	Instance *instance = new Instance(name, map, id, time, persistent, showtimer);
	Instances::InstancePtr()->addInstance(instance);
	instance->sendMessage(BeginInstance);

	if (instance->showTimer()) {
		instance->showTimer(true, true);
	}

	lua_pushstring(luaVm, name.c_str());
	lua_setglobal(luaVm, "_instancename");
	return 0;
}

int LuaExports::deleteInstanceVariable(lua_State *luaVm) {
	getInstance(luaVm)->getVariables()->deleteVariable(lua_tostring(luaVm, 1));
	return 0;
}

int LuaExports::getAllInstancePlayerIds(lua_State *luaVm) {
	vector<int32_t> ids = getInstance(luaVm)->getAllPlayerIds();
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < ids.size(); i++) {
		lua_pushinteger(luaVm, i + 1);
		lua_pushinteger(luaVm, ids[i]);
		lua_settable(luaVm, top);
	}
	return 1;
}

int LuaExports::getBannedInstancePlayerByIndex(lua_State *luaVm) {
	lua_pushstring(luaVm, getInstance(luaVm)->getBannedPlayerByIndex(lua_tointeger(luaVm, -1)).c_str());
	return 1;
}

int LuaExports::getBannedInstancePlayerCount(lua_State *luaVm) {
	lua_pushinteger(luaVm, getInstance(luaVm)->getBannedPlayerNum());
	return 1;
}

int LuaExports::getInstanceMax(lua_State *luaVm) {
	lua_pushinteger(luaVm, getInstance(luaVm)->getMaxPlayers());
	return 1;
}

int LuaExports::getInstancePlayerByIndex(lua_State *luaVm) {
	lua_pushstring(luaVm, getInstance(luaVm)->getPlayerByIndex(lua_tointeger(luaVm, -1)).c_str());
	return 1;
}

int LuaExports::getInstancePlayerCount(lua_State *luaVm) {
	lua_pushinteger(luaVm, getInstance(luaVm)->getPlayerNum());
	return 1;
}

int LuaExports::getInstancePlayerId(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	lua_pushinteger(luaVm, player->getId());
	return 1;
}

int LuaExports::getInstanceSignupCount(lua_State *luaVm) {
	lua_pushinteger(luaVm, getInstance(luaVm)->getPlayerSignupNum());
	return 1;
}

int LuaExports::getInstanceTime(lua_State *luaVm) {
	lua_pushinteger(luaVm, getInstance(luaVm)->checkInstanceTimer());
	return 1;
}

int LuaExports::getInstanceVariable(lua_State *luaVm) {
	bool integral = false;
	if (lua_isboolean(luaVm, 2)) {
		integral = true;
	}
	string val = getInstance(luaVm)->getVariables()->getVariable(lua_tostring(luaVm, 1));
	if (integral) {
		if (val == "") {
			lua_pushnil(luaVm);
		}
		else {
			lua_pushinteger(luaVm, boost::lexical_cast<int32_t>(val));
		}
	}
	else {
		lua_pushstring(luaVm, val.c_str());
	}
	return 1;
}

int LuaExports::isBannedInstancePlayer(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	lua_pushboolean(luaVm, getInstance(luaVm)->isBanned(player->getName()));
	return 1;
}

int LuaExports::isInstance(lua_State *luaVm) {
	lua_pushboolean(luaVm, Instances::InstancePtr()->isInstance(lua_tostring(luaVm, 1)));
	return 1;
}

int LuaExports::isInstanceMap(lua_State *luaVm) {
	lua_pushboolean(luaVm, getInstance(luaVm)->getMap(lua_tointeger(luaVm, -1)) != nullptr);
	return 1;
}

int LuaExports::isInstancePersistent(lua_State *luaVm) {
	lua_pushboolean(luaVm, getInstance(luaVm)->getPersistence() != 0);
	return 1;
}

int LuaExports::isPlayerSignedUp(lua_State *luaVm) {
	lua_pushboolean(luaVm, getInstance(luaVm)->isPlayerSignedUp(lua_tostring(luaVm, -1)));
	return 1;
}

int LuaExports::markForDelete(lua_State *luaVm) {
	getInstance(luaVm)->markForDelete();
	return 0;
}

int LuaExports::moveAllPlayers(lua_State *luaVm) {
	PortalInfo *portal = nullptr;

	int32_t mapid = lua_tointeger(luaVm, 1);

	if (lua_isstring(luaVm, 2)) { // Optional portal parameter
		string to = lua_tostring(luaVm, 2);
		portal = Maps::getMap(mapid)->getPortal(to);
	}

	getInstance(luaVm)->moveAllPlayers(mapid, true, portal);
	return 0;
}

int LuaExports::passPlayersBetweenInstances(lua_State *luaVm) {
	PortalInfo *portal = nullptr;

	int32_t mapid = lua_tointeger(luaVm, 1);

	if (lua_isstring(luaVm, 2)) { // Optional portal parameter
		string to = lua_tostring(luaVm, 2);
		portal = Maps::getMap(mapid)->getPortal(to);
	}

	getInstance(luaVm)->moveAllPlayers(mapid, false, portal);
	return 0;
}

int LuaExports::removeAllInstancePlayers(lua_State *luaVm) {
	getInstance(luaVm)->removeAllPlayers();
	return 0;
}

int LuaExports::removeInstancePlayer(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->removePlayer(player);
	return 0;
}

int LuaExports::removePlayerSignUp(lua_State *luaVm) {
	getInstance(luaVm)->removePlayerSignUp(lua_tostring(luaVm, -1));
	return 0;
}

int LuaExports::respawnInstanceMobs(lua_State *luaVm) {
	int32_t mapid = Maps::NoMap;
	if (lua_isnumber(luaVm, 1)) {
		mapid = lua_tointeger(luaVm, 1);
	}
	getInstance(luaVm)->respawnMobs(mapid);
	return 0;
}

int LuaExports::respawnInstanceReactors(lua_State *luaVm) {
	int32_t mapid = Maps::NoMap;
	if (lua_isnumber(luaVm, 1)) {
		mapid = lua_tointeger(luaVm, 1);
	}
	getInstance(luaVm)->respawnReactors(mapid);
	return 0;
}

int LuaExports::revertInstance(lua_State *luaVm) {
	lua_getglobal(luaVm, "_oldinstancename");
	lua_setglobal(luaVm, "_instancename");
	return 0;
}

int LuaExports::setInstance(lua_State *luaVm) {
	Instance *instance = Instances::InstancePtr()->getInstance(lua_tostring(luaVm, -1));
	if (instance != nullptr) {
		lua_getglobal(luaVm, "_instancename");
		lua_setglobal(luaVm, "_oldinstancename");

		lua_pushstring(luaVm, instance->getName().c_str());
		lua_setglobal(luaVm, "_instancename");
	}
	lua_pushboolean(luaVm, instance != nullptr);
	return 1;
}

int LuaExports::setInstanceMax(lua_State *luaVm) {
	getInstance(luaVm)->setMaxPlayers(lua_tointeger(luaVm, 1));
	return 0;
}

int LuaExports::setInstancePersistence(lua_State *luaVm) {
	getInstance(luaVm)->setPersistence(lua_tointeger(luaVm, 1));
	return 0;
}

int LuaExports::setInstanceReset(lua_State *luaVm) {
	getInstance(luaVm)->setResetAtEnd(lua_toboolean(luaVm, 1) != 0);
	return 0;
}

int LuaExports::setInstanceTime(lua_State *luaVm) {
	getInstance(luaVm)->setInstanceTimer(lua_tointeger(luaVm, 1));
	return 0;
}

int LuaExports::setInstanceVariable(lua_State *luaVm) {
	getInstance(luaVm)->getVariables()->setVariable(lua_tostring(luaVm, 1), lua_tostring(luaVm, 2));
	return 0;
}

int LuaExports::showInstanceTime(lua_State *luaVm) {
	getInstance(luaVm)->showTimer(lua_toboolean(luaVm, 1) != 0);
	return 0;
}

int LuaExports::startInstanceTimer(lua_State *luaVm) {
	string name = lua_tostring(luaVm, 1);
	TimerAction t;
	t.time = lua_tointeger(luaVm, 2);
	if (lua_isnumber(luaVm, 3)) {
		t.persistent = lua_tointeger(luaVm, 3);
	}
	t.counterid = getInstance(luaVm)->getCounterId();
	lua_pushboolean(luaVm, getInstance(luaVm)->addTimer(name, t));
	return 1;
}

int LuaExports::stopAllInstanceTimers(lua_State *luaVm) {
	getInstance(luaVm)->removeAllTimers();
	return 0;
}

int LuaExports::stopInstanceTimer(lua_State *luaVm) {
	string name = lua_tostring(luaVm, 1);
	getInstance(luaVm)->removeTimer(name);
	return 0;
}

int LuaExports::unbanInstancePlayer(lua_State *luaVm) {
	Player *player = getPlayerDeduced(-1, luaVm);
	getInstance(luaVm)->setBanned(player->getName(), false);
	return 0;
}

// Guilds
int LuaExports::addGuildPoint(lua_State *luaVm) {
	int32_t amount = 1;
	if (lua_isnumber(luaVm, -1)) {
		amount = lua_tointeger(luaVm, -1);
	}
	GuildPacket::addGuildPoint(getPlayer(luaVm)->getGuildId(), amount);
	return 0;
}

int LuaExports::disbandGuild(lua_State *luaVm) {
	GuildPacket::guildDisband(getPlayer(luaVm)->getGuildId());
	return 0;
}

int LuaExports::displayGuildRankBoard(lua_State *luaVm) {
	NpcHandler::showGuildRank(getPlayer(luaVm), lua_tointeger(luaVm, -1));
	return 0;
}

int LuaExports::getGuildCapacity(lua_State *luaVm) {
	int32_t push = 0;
	if (getPlayer(luaVm)->getGuildId() != 0) {
		push = PlayerDataProvider::Instance()->getGuild(getPlayer(luaVm)->getGuildId())->capacity;
	}
	lua_pushinteger(luaVm, push);
	return 1;
}

int LuaExports::hasEmblem(lua_State *luaVm) {
	lua_pushboolean(luaVm, PlayerDataProvider::Instance()->hasEmblem(getPlayer(luaVm)->getGuildId()));
	return 1;
}

int LuaExports::increaseGuildCapacity(lua_State *luaVm) {
	GuildPacket::sendIncreaseCapacity(getPlayer(luaVm)->getGuildId(), getPlayer(luaVm)->getId());
	return 0;
}

int LuaExports::removeEmblem(lua_State *luaVm) {
	GuildPacket::sendRemoveEmblem(getPlayer(luaVm)->getGuildId(), getPlayer(luaVm)->getId());
	return 0;
}

int LuaExports::sendNewGuildWindow(lua_State *luaVm) {
	GuildPacket::sendCreateGuildWindow(getPlayer(luaVm));
	return 0;
}

int LuaExports::sendChangeGuildEmblem(lua_State *luaVm) {
	GuildPacket::sendChangeGuildEmblem(getPlayer(luaVm));
	return 0;
}

// Alliance
int LuaExports::createAlliance(lua_State *luaVm) {
	string alliancename = lua_tostring(luaVm, -1);
	AlliancePacket::sendCreateAlliance(getPlayer(luaVm)->getId(), alliancename);
	return 1;
}

int LuaExports::getAllianceCapacity(lua_State *luaVm) {
	int32_t push = 2;
	if (Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(getPlayer(luaVm)->getAllianceId())) {
		push = alliance->capacity;
	}
	lua_pushinteger(luaVm, push);
	return 1;
}

int LuaExports::disbandAlliance(lua_State *luaVm) {
	AlliancePacket::sendDisbandAlliance(getPlayer(luaVm)->getAllianceId(), getPlayer(luaVm)->getId());
	return 0;
}

int LuaExports::increaseAllianceCapacity(lua_State *luaVm) {
	AlliancePacket::increaseAllianceCapacity(getPlayer(luaVm)->getAllianceId(), getPlayer(luaVm)->getId());
	return 0;
}