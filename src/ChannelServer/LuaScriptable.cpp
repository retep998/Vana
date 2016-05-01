/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "Common/Algorithm.hpp"
#include "Common/BeautyDataProvider.hpp"
#include "Common/GameConstants.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/InterHeader.hpp"
#include "Common/ItemConstants.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/McdbVersion.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/Randomizer.hpp"
#include "Common/ScriptDataProvider.hpp"
#include "Common/ShopDataProvider.hpp"
#include "Common/StringUtilities.hpp"
#include "Common/TimeUtilities.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Drop.hpp"
#include "ChannelServer/EffectPacket.hpp"
#include "ChannelServer/EventDataProvider.hpp"
#include "ChannelServer/Instance.hpp"
#include "ChannelServer/Instances.hpp"
#include "ChannelServer/Inventory.hpp"
#include "ChannelServer/InventoryPacket.hpp"
#include "ChannelServer/MapPacket.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Mob.hpp"
#include "ChannelServer/Npc.hpp"
#include "ChannelServer/NpcHandler.hpp"
#include "ChannelServer/Party.hpp"
#include "ChannelServer/PartyHandler.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerPacket.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/PlayersPacket.hpp"
#include "ChannelServer/Quests.hpp"
#include "ChannelServer/Reactor.hpp"
#include <iostream>
#include <vector>

namespace Vana {
namespace ChannelServer {

// TODO FIXME msvc
// Remove this when MSVC supports static init
string_t LuaScriptable::sApiVersion = "1.0.0";

LuaScriptable::LuaScriptable(const string_t &filename, player_id_t playerId) :
	LuaEnvironment{filename},
	m_playerId{playerId}
{
	initialize();
}

LuaScriptable::LuaScriptable(const string_t &filename, player_id_t playerId, bool useThread) :
	LuaEnvironment{filename, useThread},
	m_playerId{playerId}
{
	initialize();
}

auto LuaScriptable::initialize() -> void {
	set<player_id_t>("system_player_id", m_playerId); // Pushing ID for reference from static functions
	set<string_t>("system_script", getScriptName());
	set<vector_t<string_t>>("system_path", getScriptPath());
	setEnvironmentVariables();

	auto player = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(m_playerId);
	if (player != nullptr && player->getInstance() != nullptr) {
		set<string_t>("system_instance_name", player->getInstance()->getName());
	}
	else {
		set<string_t>("system_instance_name", "");
	}

	// Miscellanous
	expose("consoleOutput", &LuaExports::consoleOutput);
	expose("getRandomNumber", &LuaExports::getRandomNumber);
	expose("log", &LuaExports::log);
	expose("selectDiscrete", &LuaExports::selectDiscrete);
	expose("showGlobalMessage", &LuaExports::showGlobalMessage);
	expose("showWorldMessage", &LuaExports::showWorldMessage);
	expose("testExport", &LuaExports::testExport);

	// Channel
	expose("deleteChannelVariable", &LuaExports::deleteChannelVariable);
	expose("getChannel", &LuaExports::getChannel);
	expose("getChannelVariable", &LuaExports::getChannelVariable);
	expose("getWorld", &LuaExports::getWorld);
	expose("setChannelVariable", &LuaExports::setChannelVariable);
	expose("showChannelMessage", &LuaExports::showChannelMessage);

	// Bosses
	expose("getHorntailChannels", &LuaExports::getHorntailChannels);
	expose("getMaxHorntailBattles", &LuaExports::getMaxHorntailBattles);
	expose("getMaxPapulatusBattles", &LuaExports::getMaxPapulatusBattles);
	expose("getMaxPianusBattles", &LuaExports::getMaxPianusBattles);
	expose("getMaxPinkBeanBattles", &LuaExports::getMaxPinkBeanBattles);
	expose("getMaxZakumBattles", &LuaExports::getMaxZakumBattles);
	expose("getPapulatusChannels", &LuaExports::getPapulatusChannels);
	expose("getPianusChannels", &LuaExports::getPianusChannels);
	expose("getPinkBeanChannels", &LuaExports::getPinkBeanChannels);
	expose("getZakumChannels", &LuaExports::getZakumChannels);
	expose("isHorntailChannel", &LuaExports::isHorntailChannel);
	expose("isPapulatusChannel", &LuaExports::isPapulatusChannel);
	expose("isPianusChannel", &LuaExports::isPianusChannel);
	expose("isPinkBeanChannel", &LuaExports::isPinkBeanChannel);
	expose("isZakumChannel", &LuaExports::isZakumChannel);

	// Npc
	expose("isBusy", &LuaExports::isBusy);
	expose("removeNpc", &LuaExports::removeNpc);
	expose("runNpc", &LuaExports::runNpc);
	expose("showShop", &LuaExports::showShop);
	expose("spawnNpc", &LuaExports::spawnNpc);

	// Beauty
	expose("getAllFaces", &LuaExports::getAllFaces);
	expose("getAllHairs", &LuaExports::getAllHairs);
	expose("getAllSkins", &LuaExports::getAllSkins);
	expose("getRandomFace", &LuaExports::getRandomFace);
	expose("getRandomHair", &LuaExports::getRandomHair);
	expose("getRandomSkin", &LuaExports::getRandomSkin);
	expose("isValidFace", &LuaExports::isValidFace);
	expose("isValidHair", &LuaExports::isValidHair);
	expose("isValidSkin", &LuaExports::isValidSkin);

	// Buddy
	expose("addBuddySlots", &LuaExports::addBuddySlots);
	expose("getBuddySlots", &LuaExports::getBuddySlots);

	// Skill
	expose("addSkillLevel", &LuaExports::addSkillLevel);
	expose("getSkillLevel", &LuaExports::getSkillLevel);
	expose("getMaxSkillLevel", &LuaExports::getMaxSkillLevel);
	expose("setMaxSkillLevel", &LuaExports::setMaxSkillLevel);

	// Quest
	expose("getQuestData", &LuaExports::getQuestData);
	expose("isQuestActive", &LuaExports::isQuestActive);
	expose("isQuestInactive", &LuaExports::isQuestInactive);
	expose("isQuestCompleted", &LuaExports::isQuestCompleted);
	expose("setQuestData", &LuaExports::setQuestData);

	// Inventory
	expose("addSlots", &LuaExports::addSlots);
	expose("addStorageSlots", &LuaExports::addStorageSlots);
	expose("destroyEquippedItem", &LuaExports::destroyEquippedItem);
	expose("getEquippedItemInSlot", &LuaExports::getEquippedItemInSlot);
	expose("getItemAmount", &LuaExports::getItemAmount);
	expose("getMaxStackSize", &LuaExports::getMaxStackSize);
	expose("getMesos", &LuaExports::getMesos);
	expose("getOpenSlots", &LuaExports::getOpenSlots);
	expose("giveItem", &LuaExports::giveItem);
	expose("giveItemGachapon", &LuaExports::giveItemGachapon);
	expose("giveMesos", &LuaExports::giveMesos);
	expose("hasOpenSlotsFor", &LuaExports::hasOpenSlotsFor);
	expose("isEquippedItem", &LuaExports::isEquippedItem);
	expose("isValidItem", &LuaExports::isValidItem);
	expose("useItem", &LuaExports::useItem);

	// Player
	expose("deletePlayerVariable", &LuaExports::deletePlayerVariable);
	expose("endMorph", &LuaExports::endMorph);
	expose("getAp", &LuaExports::getAp);
	expose("getDex", &LuaExports::getDex);
	expose("getExp", &LuaExports::getExp);
	expose("getFace", &LuaExports::getFace);
	expose("getFame", &LuaExports::getFame);
	expose("getFh", &LuaExports::getFh);
	expose("getGender", &LuaExports::getGender);
	expose("getGmLevel", &LuaExports::getGmLevel);
	expose("getHair", &LuaExports::getHair);
	expose("getHp", &LuaExports::getHp);
	expose("getHpMpAp", &LuaExports::getHpMpAp);
	expose("getId", &LuaExports::getId);
	expose("getInt", &LuaExports::getInt);
	expose("getJob", &LuaExports::getJob);
	expose("getLevel", &LuaExports::getLevel);
	expose("getLuk", &LuaExports::getLuk);
	expose("getMap", &LuaExports::getMap);
	expose("getMaxHp", &LuaExports::getMaxHp);
	expose("getMaxMp", &LuaExports::getMaxMp);
	expose("getMp", &LuaExports::getMp);
	expose("getName", &LuaExports::getName);
	expose("getPlayerVariable", &LuaExports::getPlayerVariable);
	expose("getPosX", &LuaExports::getPosX);
	expose("getPosY", &LuaExports::getPosY);
	expose("getRealMaxHp", &LuaExports::getRealMaxHp);
	expose("getRealMaxMp", &LuaExports::getRealMaxMp);
	expose("getSkin", &LuaExports::getSkin);
	expose("getSp", &LuaExports::getSp);
	expose("getStr", &LuaExports::getStr);
	expose("giveAp", &LuaExports::giveAp);
	expose("giveExp", &LuaExports::giveExp);
	expose("giveFame", &LuaExports::giveFame);
	expose("giveSp", &LuaExports::giveSp);
	expose("isActiveItem", &LuaExports::isActiveItem);
	expose("isActiveSkill", &LuaExports::isActiveSkill);
	expose("isGm", &LuaExports::isGm);
	expose("isOnline", &LuaExports::isOnline);
	expose("revertPlayer", &LuaExports::revertPlayer);
	expose("setAp", &LuaExports::setAp);
	expose("setDex", &LuaExports::setDex);
	expose("setExp", &LuaExports::setExp);
	expose("setHp", &LuaExports::setHp);
	expose("setInt", &LuaExports::setInt);
	expose("setJob", &LuaExports::setJob);
	expose("setLevel", &LuaExports::setLevel);
	expose("setLuk", &LuaExports::setLuk);
	expose("setMap", &LuaExports::setMap);
	expose("setMaxHp", &LuaExports::setMaxHp);
	expose("setMaxMp", &LuaExports::setMaxMp);
	expose("setMp", &LuaExports::setMp);
	expose("setPlayer", &LuaExports::setPlayer);
	expose("setPlayerVariable", &LuaExports::setPlayerVariable);
	expose("setSp", &LuaExports::setSp);
	expose("setStr", &LuaExports::setStr);
	expose("setStyle", &LuaExports::setStyle);
	expose("showInstructionBubble", &LuaExports::showInstructionBubble);
	expose("showMessage", &LuaExports::showMessage);

	// Effects
	expose("playFieldSound", &LuaExports::playFieldSound);
	expose("playMinigameSound", &LuaExports::playMinigameSound);
	expose("setMusic", &LuaExports::setMusic);
	expose("showMapEffect", &LuaExports::showMapEffect);
	expose("showMapEvent", &LuaExports::showMapEvent);

	// Map
	expose("clearDrops", &LuaExports::clearDrops);
	expose("clearMobs", &LuaExports::clearMobs);
	expose("countMobs", &LuaExports::countMobs);
	expose("setPortalState", &LuaExports::setPortalState);
	expose("getAllMapPlayerIds", &LuaExports::getAllMapPlayerIds);
	expose("getNumPlayers", &LuaExports::getNumPlayers);
	expose("getReactorState", &LuaExports::getReactorState);
	expose("killMobs", &LuaExports::killMobs);
	expose("setBoatDocked", &LuaExports::setBoatDocked);
	expose("setMapSpawn", &LuaExports::setMapSpawn);
	expose("setReactorState", &LuaExports::setReactorState);
	expose("showMapMessage", &LuaExports::showMapMessage);
	expose("showMapTimer", &LuaExports::showMapTimer);
	expose("spawnMob", &LuaExports::spawnMob);
	expose("spawnMobPos", &LuaExports::spawnMobPos);

	// Mob
	expose("getMobFh", &LuaExports::getMobFh);
	expose("getMobHp", &LuaExports::getMobHp);
	expose("getMobMaxHp", &LuaExports::getMobMaxHp);
	expose("getMobMaxMp", &LuaExports::getMobMaxMp);
	expose("getMobMp", &LuaExports::getMobMp);
	expose("getMobPosX", &LuaExports::getMobPosX);
	expose("getMobPosY", &LuaExports::getMobPosY);
	expose("getRealMobId", &LuaExports::getRealMobId);
	expose("killMob", &LuaExports::killMob);
	expose("mobDropItem", &LuaExports::mobDropItem);

	// Time
	expose("getDate", &LuaExports::getDate);
	expose("getDay", &LuaExports::getDay);
	expose("getHour", &LuaExports::getHour);
	expose("getMinute", &LuaExports::getMinute);
	expose("getMonth", &LuaExports::getMonth);
	expose("getNearestMinute", &LuaExports::getNearestMinute);
	expose("getSecond", &LuaExports::getSecond);
	expose("getTime", &LuaExports::getTime);
	expose("getTimeZoneOffset", &LuaExports::getTimeZoneOffset);
	expose("getWeek", &LuaExports::getWeek);
	expose("getYear", &LuaExports::getYear);
	expose("isDst", &LuaExports::isDst);

	// Rates
	expose("getDropMeso", &LuaExports::getDropMeso);
	expose("getDropRate", &LuaExports::getDropRate);
	expose("getExpRate", &LuaExports::getExpRate);
	expose("getGlobalDropMeso", &LuaExports::getGlobalDropMeso);
	expose("getGlobalDropRate", &LuaExports::getGlobalDropRate);
	expose("getQuestExpRate", &LuaExports::getQuestExpRate);

	// Party
	expose("checkPartyFootholds", &LuaExports::checkPartyFootholds);
	expose("getAllPartyPlayerIds", &LuaExports::getAllPartyPlayerIds);
	expose("getPartyCount", &LuaExports::getPartyCount);
	expose("getPartyId", &LuaExports::getPartyId);
	expose("getPartyMapCount", &LuaExports::getPartyMapCount);
	expose("isPartyInLevelRange", &LuaExports::isPartyInLevelRange);
	expose("isPartyLeader", &LuaExports::isPartyLeader);
	expose("verifyPartyFootholds", &LuaExports::verifyPartyFootholds);
	expose("warpParty", &LuaExports::warpParty);

	// Instance
	expose("addInstanceMap", &LuaExports::addInstanceMap);
	expose("addInstanceParty", &LuaExports::addInstanceParty);
	expose("addInstancePlayer", &LuaExports::addInstancePlayer);
	expose("checkInstanceTimer", &LuaExports::checkInstanceTimer);
	expose("createInstance", &LuaExports::createInstance);
	expose("deleteInstanceVariable", &LuaExports::deleteInstanceVariable);
	expose("getAllInstancePlayerIds", &LuaExports::getAllInstancePlayerIds);
	expose("getInstancePlayerCount", &LuaExports::getInstancePlayerCount);
	expose("getInstancePlayerId", &LuaExports::getInstancePlayerId);
	expose("getInstanceVariable", &LuaExports::getInstanceVariable);
	expose("isInstance", &LuaExports::isInstance);
	expose("isInstanceMap", &LuaExports::isInstanceMap);
	expose("isInstancePersistent", &LuaExports::isInstancePersistent);
	expose("markForDelete", &LuaExports::markForDelete);
	expose("moveAllPlayers", &LuaExports::moveAllPlayers);
	expose("passPlayersBetweenInstances", &LuaExports::passPlayersBetweenInstances);
	expose("removeAllInstancePlayers", &LuaExports::removeAllInstancePlayers);
	expose("removeInstancePlayer", &LuaExports::removeInstancePlayer);
	expose("respawnInstanceMobs", &LuaExports::respawnInstanceMobs);
	expose("respawnInstanceReactors", &LuaExports::respawnInstanceReactors);
	expose("revertInstance", &LuaExports::revertInstance);
	expose("setInstance", &LuaExports::setInstance);
	expose("setInstancePersistence", &LuaExports::setInstancePersistence);
	expose("setInstanceReset", &LuaExports::setInstanceReset);
	expose("setInstanceVariable", &LuaExports::setInstanceVariable);
	expose("showInstanceTime", &LuaExports::showInstanceTime);
	expose("startInstanceFutureTimer", &LuaExports::startInstanceFutureTimer);
	expose("startInstanceSecondOfHourTimer", &LuaExports::startInstanceSecondOfHourTimer);
	expose("stopAllInstanceTimers", &LuaExports::stopAllInstanceTimers);
	expose("stopInstanceTimer", &LuaExports::stopInstanceTimer);
}

auto LuaScriptable::setEnvironmentVariables() -> void {
	set<string_t>("instance_timer", "instance");

	set<int32_t>("msg_blue", Packets::Player::NoticeTypes::Blue);
	set<int32_t>("msg_red", Packets::Player::NoticeTypes::Red);
	set<int32_t>("msg_notice", Packets::Player::NoticeTypes::Notice);
	set<int32_t>("msg_box", Packets::Player::NoticeTypes::Box);

	set<int32_t>("gender_male", Gender::Male);
	set<int32_t>("gender_female", Gender::Female);

	set<bool>("boat_docked", true);
	set<bool>("boat_undocked", false);

	set<int32_t>("type_bool", VariableType::Bool);
	set<int32_t>("type_int", VariableType::Integer);
	set<int32_t>("type_num", VariableType::Number);
	set<int32_t>("type_str", VariableType::String);

	set<bool>("portal_enabled", true);
	set<bool>("portal_disabled", false);

	set<int8_t>("line_beginner", Jobs::JobLines::Beginner);
	set<int8_t>("line_warrior", Jobs::JobLines::Warrior);
	set<int8_t>("line_magician", Jobs::JobLines::Magician);
	set<int8_t>("line_bowman", Jobs::JobLines::Bowman);
	set<int8_t>("line_thief", Jobs::JobLines::Thief);
	set<int8_t>("line_pirate", Jobs::JobLines::Pirate);

	set<int8_t>("progression_second", 0);
	set<int8_t>("progression_third", 1);
	set<int8_t>("progression_fourth", 2);

	set<item_id_t>("item_mesos", Items::SackOfMoney);

	set<string_t>("locale_global", Mcdb::Locales::Global);
	set<string_t>("locale_korea", Mcdb::Locales::Korea);
	set<string_t>("locale_japan", Mcdb::Locales::Japan);
	set<string_t>("locale_china", Mcdb::Locales::China);
	set<string_t>("locale_europe", Mcdb::Locales::Europe);
	set<string_t>("locale_thailand", Mcdb::Locales::Thailand);
	set<string_t>("locale_tawian", Mcdb::Locales::Taiwan);
	set<string_t>("locale_sea", Mcdb::Locales::Sea);
	set<string_t>("locale_brazil", Mcdb::Locales::Brazil);

	set<version_t>("env_version", MapleVersion::Version);
	set<string_t>("env_subversion", MapleVersion::LoginSubversion);
	set<bool>("env_is_test_server", Mcdb::IsTestServer);
	set<string_t>("env_locale", Mcdb::Locale);
	set<string_t>("env_api_version", sApiVersion);
}

auto LuaScriptable::handleError(const string_t &filename, const string_t &error) -> void {
	auto &channel = ChannelServer::getInstance();
	auto player = channel.getPlayerDataProvider().getPlayer(m_playerId);

	channel.log(LogType::ScriptLog, "Script error in " + filename + ": " + error);
	if (player == nullptr) {
		return;
	}

	if (player->isGm()) {
		player->send(Packets::Player::showMessage(error, Packets::Player::NoticeTypes::Red));
	}
	else {
		player->send(Packets::Player::showMessage("There is a script error; please contact an administrator", Packets::Player::NoticeTypes::Red));
	}
}

// Lua Exports
auto LuaExports::getEnvironment(lua_State *luaVm) -> LuaEnvironment & {
	return LuaEnvironment::getEnvironment(luaVm);
}

auto LuaExports::getPlayer(lua_State *luaVm, LuaEnvironment &env) -> ref_ptr_t<Player> {
	player_id_t playerId = env.get<player_id_t>(luaVm, "system_player_id");
	return ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerId);
}

auto LuaExports::getPlayerDeduced(int parameter, lua_State *luaVm, LuaEnvironment &env) -> ref_ptr_t<Player> {
	ref_ptr_t<Player> player = nullptr;
	if (env.is(luaVm, parameter, LuaType::String)) {
		player = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(env.get<string_t>(luaVm, parameter));
	}
	else {
		player = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(env.get<player_id_t>(luaVm, parameter));
	}
	return player;
}

auto LuaExports::getInstance(lua_State *luaVm, LuaEnvironment &env) -> Instance * {
	string_t instanceName = env.get<string_t>(luaVm, "system_instance_name");
	return ChannelServer::getInstance().getInstances().getInstance(instanceName);
}

auto LuaExports::obtainSetVariablePair(lua_State *luaVm, LuaEnvironment &env) -> pair_t<string_t, string_t> {
	pair_t<string_t, string_t> ret;
	ret.first = env.get<string_t>(luaVm, 1);
	switch (env.typeOf(luaVm, 2)) {
		case LuaType::Nil: ret.second = "nil"; break;
		case LuaType::Bool: ret.second = env.get<bool>(luaVm, 2) ? "true" : "false"; break;
		case LuaType::Number: ret.second = StringUtilities::lexical_cast<string_t>(env.get<int32_t>(luaVm, 2)); break;
		case LuaType::String: ret.second = env.get<string_t>(luaVm, 2); break;
		default: throw NotImplementedException{"Lua datatype"};
	}
	return ret;
}

auto LuaExports::pushGetVariableData(lua_State *luaVm, LuaEnvironment &env, const string_t &value, VariableType::Type returnType) -> lua_return_t {
	if (value == "nil") {
		env.pushNil(luaVm);
	}
	else {
		switch (returnType) {
			case VariableType::Bool: env.push<bool>(luaVm, value == "true"); break;
			case VariableType::Number:
				if (value.empty()) {
					env.pushNil(luaVm);
				}
				else {
					env.push<double>(luaVm, StringUtilities::lexical_cast<double>(value));
				}
				break;
			case VariableType::Integer:
				if (value.empty()) {
					env.pushNil(luaVm);
				}
				else {
					env.push<int32_t>(luaVm, StringUtilities::lexical_cast<int32_t>(value));
				}
				break;
			case VariableType::String: env.push<string_t>(luaVm, value); break;
			default: throw std::invalid_argument{"returnType must be a valid type_ constant"};
		}
	}
	return 1;
}

auto LuaExports::isBossChannel(lua_State *luaVm, const vector_t<channel_id_t> &elements) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	channel_id_t channel = ChannelServer::getInstance().getChannelId() + 1;
	env.push<bool>(luaVm, ext::any_of(elements, [channel](channel_id_t testChannel) -> bool { return testChannel == channel; }));
	return 1;
}

// Miscellaneous
auto LuaExports::consoleOutput(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	switch (env.typeOf(luaVm, 1)) {
		case LuaType::None: std::cout << "INVALID_ARGUMENT" << std::endl; break;
		case LuaType::Nil: std::cout << "nil" << std::endl; break;
		case LuaType::Table: std::cout << "table" << std::endl; break;
		case LuaType::Bool: std::cout << (env.get<bool>(luaVm, 1) ? "true" : "false") << std::endl; break;
		case LuaType::LightUserData: std::cout << "lightuserdata" << std::endl; break;
		case LuaType::UserData: std::cout << "userdata" << std::endl; break;
		case LuaType::Thread: std::cout << "thread" << std::endl; break;
		case LuaType::Function: std::cout << "function" << std::endl; break;
		default: std::cout << env.get<string_t>(luaVm, 1) << std::endl; break;
	}

	return 0;
}

auto LuaExports::getRandomNumber(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, Randomizer::rand<int32_t>(env.get<int32_t>(luaVm, 1), 1));
	return 1;
}

auto LuaExports::log(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	ChannelServer::getInstance().log(LogType::ScriptLog, env.get<string_t>(luaVm, 1));
	return 0;
}

auto LuaExports::selectDiscrete(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	vector_t<double> relativeChances = env.get<vector_t<double>>(luaVm, 1);
	if (relativeChances.size() == 0) {
		env.pushNil(luaVm);
	}
	else {
		std::discrete_distribution<> dist{relativeChances.begin(), relativeChances.end()};
		// Account for Lua array start
		env.push<int32_t>(luaVm, Randomizer::rand(dist) + 1);
	}
	return 1;
}

auto LuaExports::showGlobalMessage(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t msg = env.get<string_t>(luaVm, 1);
	int8_t type = env.get<int8_t>(luaVm, 2);
	ChannelServer::getInstance().sendWorld(
		Vana::Packets::prepend(Packets::Player::showMessage(msg, type), [](PacketBuilder &builder) {
			builder.add<header_t>(IMSG_TO_LOGIN);
			builder.add<header_t>(IMSG_TO_ALL_WORLDS);
			builder.add<header_t>(IMSG_TO_ALL_CHANNELS);
			builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
		}));
	return 0;
}

auto LuaExports::showWorldMessage(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t msg = env.get<string_t>(luaVm, 1);
	int8_t type = env.get<int8_t>(luaVm, 2);
	ChannelServer::getInstance().sendWorld(
		Vana::Packets::prepend(Packets::Player::showMessage(msg, type), [](PacketBuilder &builder) {
			builder.add<header_t>(IMSG_TO_ALL_CHANNELS);
			builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
		}));
	return 0;
}

auto LuaExports::testExport(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	return 0;
}

// Channel
auto LuaExports::deleteChannelVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t key = env.get<string_t>(luaVm, 1);
	ChannelServer::getInstance().getEventDataProvider().getVariables()->deleteVariable(key);
	return 0;
}

auto LuaExports::getChannel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<channel_id_t>(luaVm, ChannelServer::getInstance().getChannelId() + 1);
	return 1;
}

auto LuaExports::getChannelVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	VariableType::Type returnType = VariableType::String;
	if (env.is(luaVm, 2, LuaType::Number)) {
		returnType = (VariableType::Type)env.get<int32_t>(luaVm, 2);
	}
	string_t val = ChannelServer::getInstance().getEventDataProvider().getVariables()->getVariable(env.get<string_t>(luaVm, 1));
	pushGetVariableData(luaVm, env, val, returnType);
	return 1;
}

auto LuaExports::getWorld(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<world_id_t>(luaVm, ChannelServer::getInstance().getWorldId() + 1);
	return 1;
}

auto LuaExports::setChannelVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto kvp = obtainSetVariablePair(luaVm, env);
	ChannelServer::getInstance().getEventDataProvider().getVariables()->setVariable(kvp.first, kvp.second);
	return 0;
}

auto LuaExports::showChannelMessage(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t msg = env.get<string_t>(luaVm, 1);
	int8_t type = env.get<int8_t>(luaVm, 2);
	ChannelServer::getInstance().getPlayerDataProvider().send(Packets::Player::showMessage(msg, type));
	return 0;
}

// Bosses
auto LuaExports::getHorntailChannels(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<channel_id_t>>(luaVm, ChannelServer::getInstance().getConfig().horntail.channels);
	return 1;
}

auto LuaExports::getMaxHorntailBattles(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int16_t>(luaVm, ChannelServer::getInstance().getConfig().horntail.attempts);
	return 1;
}

auto LuaExports::getMaxPapulatusBattles(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int16_t>(luaVm, ChannelServer::getInstance().getConfig().papulatus.attempts);
	return 1;
}

auto LuaExports::getMaxPianusBattles(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int16_t>(luaVm, ChannelServer::getInstance().getConfig().pianus.attempts);
	return 1;
}

auto LuaExports::getMaxPinkBeanBattles(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int16_t>(luaVm, ChannelServer::getInstance().getConfig().pinkbean.attempts);
	return 1;
}

auto LuaExports::getMaxZakumBattles(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int16_t>(luaVm, ChannelServer::getInstance().getConfig().zakum.attempts);
	return 1;
}

auto LuaExports::getPapulatusChannels(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<channel_id_t>>(luaVm, ChannelServer::getInstance().getConfig().papulatus.channels);
	return 1;
}

auto LuaExports::getPianusChannels(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<channel_id_t>>(luaVm, ChannelServer::getInstance().getConfig().pianus.channels);
	return 1;
}

auto LuaExports::getPinkBeanChannels(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<channel_id_t>>(luaVm, ChannelServer::getInstance().getConfig().pinkbean.channels);
	return 1;
}

auto LuaExports::getZakumChannels(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<channel_id_t>>(luaVm, ChannelServer::getInstance().getConfig().zakum.channels);
	return 1;
}

auto LuaExports::isHorntailChannel(lua_State *luaVm) -> lua_return_t {
	return isBossChannel(luaVm, ChannelServer::getInstance().getConfig().horntail.channels);
}

auto LuaExports::isPapulatusChannel(lua_State *luaVm) -> lua_return_t {
	return isBossChannel(luaVm, ChannelServer::getInstance().getConfig().papulatus.channels);
}

auto LuaExports::isPianusChannel(lua_State *luaVm) -> lua_return_t {
	return isBossChannel(luaVm, ChannelServer::getInstance().getConfig().pianus.channels);
}

auto LuaExports::isPinkBeanChannel(lua_State *luaVm) -> lua_return_t {
	return isBossChannel(luaVm, ChannelServer::getInstance().getConfig().pinkbean.channels);
}

auto LuaExports::isZakumChannel(lua_State *luaVm) -> lua_return_t {
	return isBossChannel(luaVm, ChannelServer::getInstance().getConfig().zakum.channels);
}

// NPC
auto LuaExports::isBusy(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, getPlayer(luaVm, env)->getNpc() != nullptr);
	return 1;
}

auto LuaExports::removeNpc(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	uint32_t index = env.get<uint32_t>(luaVm, 2);
	Maps::getMap(mapId)->removeNpc(index);
	return 0;
}

auto LuaExports::runNpc(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	npc_id_t npcId = env.get<npc_id_t>(luaVm, 1);
	string_t script;
	auto &channel = ChannelServer::getInstance();
	if (env.is(luaVm, 2, LuaType::String)) {
		// We already have our script name
		string_t specified = env.get<string_t>(luaVm, 2);
		script = channel.getScriptDataProvider().buildScriptPath(ScriptTypes::Npc, specified);
	}
	else {
		script = channel.getScriptDataProvider().getScript(&channel, npcId, ScriptTypes::Npc);
	}
	Npc *npc = new Npc{npcId, getPlayer(luaVm, env), script};
	npc->run();
	return 0;
}

auto LuaExports::showShop(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	shop_id_t shopId = env.get<shop_id_t>(luaVm, 1);
	NpcHandler::showShop(getPlayer(luaVm, env), shopId);
	return 0;
}

auto LuaExports::spawnNpc(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	npc_id_t npcId = env.get<npc_id_t>(luaVm, 2);
	coord_t x = env.get<coord_t>(luaVm, 3);
	coord_t y = env.get<coord_t>(luaVm, 4);

	NpcSpawnInfo npc;
	npc.id = npcId;
	npc.foothold = 0;
	npc.pos = Point{x, y};
	npc.rx0 = x - 50;
	npc.rx1 = x + 50;

	env.push<map_object_t>(luaVm, Maps::getMap(mapId)->addNpc(npc));
	return 1;
}

// Beauty
auto LuaExports::getAllFaces(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<face_id_t>>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().getFaces(getPlayer(luaVm, env)->getGender()));
	return 1;
}

auto LuaExports::getAllHairs(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<hair_id_t>>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().getHair(getPlayer(luaVm, env)->getGender()));
	return 1;
}

auto LuaExports::getAllSkins(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<skin_id_t>>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().getSkins());
	return 1;
}

auto LuaExports::getRandomFace(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<face_id_t>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().getRandomFace(getPlayer(luaVm, env)->getGender()));
	return 1;
}

auto LuaExports::getRandomHair(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<hair_id_t>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().getRandomHair(getPlayer(luaVm, env)->getGender()));
	return 1;
}

auto LuaExports::getRandomSkin(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<skin_id_t>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().getRandomSkin());
	return 1;
}

auto LuaExports::isValidFace(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().isValidFace(getPlayer(luaVm, env)->getGender(), env.get<face_id_t>(luaVm, 1)));
	return 1;
}

auto LuaExports::isValidHair(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().isValidHair(getPlayer(luaVm, env)->getGender(), env.get<hair_id_t>(luaVm, 1)));
	return 1;
}

auto LuaExports::isValidSkin(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, ChannelServer::getInstance().getBeautyDataProvider().isValidSkin(env.get<skin_id_t>(luaVm, 1)));
	return 1;
}

// Buddy
auto LuaExports::addBuddySlots(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto p = getPlayer(luaVm, env);
	uint8_t csize = p->getBuddyListSize();
	int8_t mod = env.get<int8_t>(luaVm, 1);
	p->setBuddyListSize(csize + mod);
	return 0;
}

auto LuaExports::getBuddySlots(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<uint8_t>(luaVm, getPlayer(luaVm, env)->getBuddyListSize());
	return 1;
}

// Skill
auto LuaExports::addSkillLevel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	skill_id_t skillId = env.get<skill_id_t>(luaVm, 1);
	skill_level_t level = env.get<skill_level_t>(luaVm, 2);

	if (env.is(luaVm, 3, LuaType::Number)) {
		// Optional argument of increasing a skill's max level
		getPlayer(luaVm, env)->getSkills()->setMaxSkillLevel(skillId, env.get<skill_level_t>(luaVm, 3));
	}

	getPlayer(luaVm, env)->getSkills()->addSkillLevel(skillId, level);
	return 0;
}

auto LuaExports::getSkillLevel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	skill_id_t skillId = env.get<skill_id_t>(luaVm, 1);
	env.push<skill_level_t>(luaVm, getPlayer(luaVm, env)->getSkills()->getSkillLevel(skillId));
	return 1;
}

auto LuaExports::getMaxSkillLevel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	skill_id_t skillId = env.get<skill_id_t>(luaVm, 1);
	env.push<skill_level_t>(luaVm, getPlayer(luaVm, env)->getSkills()->getMaxSkillLevel(skillId));
	return 1;
}

auto LuaExports::setMaxSkillLevel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	skill_id_t skillId = env.get<skill_id_t>(luaVm, 1);
	skill_level_t level = env.get<skill_level_t>(luaVm, 2);
	getPlayer(luaVm, env)->getSkills()->setMaxSkillLevel(skillId, level);
	return 0;
}

// Quest
auto LuaExports::getQuestData(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	quest_id_t questId = env.get<quest_id_t>(luaVm, 1);
	env.push<string_t>(luaVm, getPlayer(luaVm, env)->getQuests()->getQuestData(questId));
	return 1;
}

auto LuaExports::isQuestActive(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	quest_id_t questId = env.get<quest_id_t>(luaVm, 1);
	env.push<bool>(luaVm, getPlayer(luaVm, env)->getQuests()->isQuestActive(questId));
	return 1;
}

auto LuaExports::isQuestInactive(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	quest_id_t questId = env.get<quest_id_t>(luaVm, 1);
	auto player = getPlayer(luaVm, env);
	bool active = !(player->getQuests()->isQuestActive(questId) || player->getQuests()->isQuestComplete(questId));
	env.push<bool>(luaVm, active);
	return 1;
}

auto LuaExports::isQuestCompleted(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	quest_id_t questId = env.get<quest_id_t>(luaVm, 1);
	env.push<bool>(luaVm, getPlayer(luaVm, env)->getQuests()->isQuestComplete(questId));
	return 1;
}

auto LuaExports::setQuestData(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	quest_id_t questId = env.get<quest_id_t>(luaVm, 1);
	string_t data = env.get<string_t>(luaVm, 2);
	getPlayer(luaVm, env)->getQuests()->setQuestData(questId, data);
	return 0;
}

// Inventory
auto LuaExports::addSlots(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	inventory_t inventory = env.get<inventory_t>(luaVm, 1);
	inventory_slot_count_t rows = env.get<inventory_slot_count_t>(luaVm, 2);
	getPlayer(luaVm, env)->getInventory()->addMaxSlots(inventory, rows);
	return 0;
}

auto LuaExports::addStorageSlots(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	storage_slot_t slots = env.get<storage_slot_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStorage()->setSlots(getPlayer(luaVm, env)->getStorage()->getSlots() + slots);
	return 0;
}

auto LuaExports::destroyEquippedItem(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	auto player = getPlayer(luaVm, env);
	bool destroyed = player->getInventory()->isEquippedItem(itemId);
	if (destroyed) {
		player->getInventory()->destroyEquippedItem(itemId);
	}
	env.push<bool>(luaVm, destroyed);
	return 1;
}

auto LuaExports::getEquippedItemInSlot(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	inventory_slot_t slot = env.get<inventory_slot_t>(luaVm, 1);
	env.push<item_id_t>(luaVm, getPlayer(luaVm, env)->getInventory()->getEquippedId(slot));
	return 1;
}

auto LuaExports::getItemAmount(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	env.push<slot_qty_t>(luaVm, getPlayer(luaVm, env)->getInventory()->getItemAmount(itemId));
	return 1;
}

auto LuaExports::getMaxStackSize(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	env.push<slot_qty_t>(luaVm, ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId)->maxSlot);
	return 1;
}

auto LuaExports::getMesos(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<mesos_t>(luaVm, getPlayer(luaVm, env)->getInventory()->getMesos());
	return 1;
}

auto LuaExports::getOpenSlots(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	inventory_t inv = env.get<inventory_t>(luaVm, 1);
	env.push<inventory_slot_count_t>(luaVm, getPlayer(luaVm, env)->getInventory()->getOpenSlotsNum(inv));
	return 1;
}

auto LuaExports::giveItem(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	slot_qty_t amount = 1;
	if (env.is(luaVm, 2, LuaType::Number)) {
		amount = env.get<slot_qty_t>(luaVm, 2);
	}
	bool success = Quests::giveItem(getPlayer(luaVm, env), itemId, amount) == Result::Successful;
	env.push<bool>(luaVm, success);
	return 1;
}

auto LuaExports::giveItemGachapon(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	slot_qty_t amount = 1;
	if (env.is(luaVm, 2, LuaType::Number)) {
		amount = env.get<slot_qty_t>(luaVm, 2);
	}
	bool success = Quests::giveItem(getPlayer(luaVm, env), itemId, amount, Items::StatVariance::Gachapon) == Result::Successful;
	env.push<bool>(luaVm, success);
	return 1;
}

auto LuaExports::giveMesos(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	mesos_t mesos = env.get<mesos_t>(luaVm, 1);
	bool success = Quests::giveMesos(getPlayer(luaVm, env), mesos) == Result::Successful;
	env.push<bool>(luaVm, success);
	return 1;
}

auto LuaExports::hasOpenSlotsFor(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	slot_qty_t amount = 1;
	if (env.is(luaVm, 2, LuaType::Number)) {
		amount = env.get<slot_qty_t>(luaVm, 2);
	}
	env.push<bool>(luaVm, getPlayer(luaVm, env)->getInventory()->hasOpenSlotsFor(itemId, amount));
	return 1;
}

auto LuaExports::isEquippedItem(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	env.push<bool>(luaVm, getPlayer(luaVm, env)->getInventory()->isEquippedItem(itemId));
	return 1;
}

auto LuaExports::isValidItem(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	env.push<bool>(luaVm, ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId) != nullptr);
	return 1;
}

auto LuaExports::useItem(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	Inventory::useItem(getPlayer(luaVm, env), itemId);
	return 0;
}

// Player
auto LuaExports::deletePlayerVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t key = env.get<string_t>(luaVm, 1);
	getPlayer(luaVm, env)->getVariables()->deleteVariable(key);
	return 0;
}

auto LuaExports::endMorph(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getPlayer(luaVm, env)->getActiveBuffs()->endMorph();
	return 0;
}

auto LuaExports::getAp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<stat_t>(luaVm, getPlayer(luaVm, env)->getStats()->getAp());
	return 1;
}

auto LuaExports::getDex(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<stat_t>(luaVm, getPlayer(luaVm, env)->getStats()->getDex());
	return 1;
}

auto LuaExports::getExp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<experience_t>(luaVm, getPlayer(luaVm, env)->getStats()->getExp());
	return 1;
}

auto LuaExports::getFace(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<face_id_t>(luaVm, getPlayer(luaVm, env)->getFace());
	return 1;
}

auto LuaExports::getFame(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<fame_t>(luaVm, getPlayer(luaVm, env)->getStats()->getFame());
	return 1;
}

auto LuaExports::getFh(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<foothold_id_t>(luaVm, getPlayer(luaVm, env)->getFoothold());
	return 1;
}

auto LuaExports::getGender(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<gender_id_t>(luaVm, getPlayer(luaVm, env)->getGender());
	return 1;
}

auto LuaExports::getGmLevel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, getPlayer(luaVm, env)->getGmLevel());
	return 1;
}

auto LuaExports::getHair(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<hair_id_t>(luaVm, getPlayer(luaVm, env)->getHair());
	return 1;
}

auto LuaExports::getHp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<health_t>(luaVm, getPlayer(luaVm, env)->getStats()->getHp());
	return 1;
}

auto LuaExports::getHpMpAp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<health_ap_t>(luaVm, getPlayer(luaVm, env)->getStats()->getHpMpAp());
	return 1;
}

auto LuaExports::getId(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<player_id_t>(luaVm, getPlayer(luaVm, env)->getId());
	return 1;
}

auto LuaExports::getInt(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<stat_t>(luaVm, getPlayer(luaVm, env)->getStats()->getInt());
	return 1;
}

auto LuaExports::getJob(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<job_id_t>(luaVm, getPlayer(luaVm, env)->getStats()->getJob());
	return 1;
}

auto LuaExports::getLevel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<player_level_t>(luaVm, getPlayer(luaVm, env)->getStats()->getLevel());
	return 1;
}

auto LuaExports::getLuk(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<stat_t>(luaVm, getPlayer(luaVm, env)->getStats()->getLuk());
	return 1;
}

auto LuaExports::getMap(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<map_id_t>(luaVm, getPlayer(luaVm, env)->getMapId());
	return 1;
}

auto LuaExports::getMaxHp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<health_t>(luaVm, getPlayer(luaVm, env)->getStats()->getMaxHp());
	return 1;
}

auto LuaExports::getMaxMp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<health_t>(luaVm, getPlayer(luaVm, env)->getStats()->getMaxMp());
	return 1;
}

auto LuaExports::getMp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<health_t>(luaVm, getPlayer(luaVm, env)->getStats()->getMp());
	return 1;
}

auto LuaExports::getName(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<string_t>(luaVm, getPlayer(luaVm, env)->getName());
	return 1;
}

auto LuaExports::getPlayerVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	VariableType::Type returnType = VariableType::String;
	if (env.is(luaVm, 2, LuaType::Number)) {
		returnType = (VariableType::Type)env.get<int32_t>(luaVm, 2);
	}
	string_t val = getPlayer(luaVm, env)->getVariables()->getVariable(env.get<string_t>(luaVm, 1));
	pushGetVariableData(luaVm, env, val, returnType);
	return 1;
}

auto LuaExports::getPosX(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<coord_t>(luaVm, getPlayer(luaVm, env)->getPos().x);
	return 1;
}

auto LuaExports::getPosY(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<coord_t>(luaVm, getPlayer(luaVm, env)->getPos().y);
	return 1;
}

auto LuaExports::getRealMaxHp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<health_t>(luaVm, getPlayer(luaVm, env)->getStats()->getMaxHp(true));
	return 1;
}

auto LuaExports::getRealMaxMp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<health_t>(luaVm, getPlayer(luaVm, env)->getStats()->getMaxMp(true));
	return 1;
}

auto LuaExports::getSkin(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<skin_id_t>(luaVm, getPlayer(luaVm, env)->getSkin());
	return 1;
}

auto LuaExports::getSp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<stat_t>(luaVm, getPlayer(luaVm, env)->getStats()->getSp());
	return 1;
}

auto LuaExports::getStr(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<stat_t>(luaVm, getPlayer(luaVm, env)->getStats()->getStr());
	return 1;
}

auto LuaExports::giveAp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t ap = env.get<stat_t>(luaVm, -1);
	getPlayer(luaVm, env)->getStats()->setAp(getPlayer(luaVm, env)->getStats()->getAp() + ap);
	return 0;
}

auto LuaExports::giveExp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	experience_t exp = env.get<experience_t>(luaVm, -1);
	getPlayer(luaVm, env)->getStats()->giveExp(exp * ChannelServer::getInstance().getConfig().rates.questExpRate, true);
	return 0;
}

auto LuaExports::giveFame(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	fame_t fame = env.get<fame_t>(luaVm, 1);
	bool success = Quests::giveFame(getPlayer(luaVm, env), fame) == Result::Successful;
	env.push<bool>(luaVm, success);
	return 1;
}

auto LuaExports::giveSp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t sp = env.get<stat_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setSp(getPlayer(luaVm, env)->getStats()->getSp() + sp);
	return 0;
}

auto LuaExports::isActiveItem(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t item = env.get<item_id_t>(luaVm, 1);
	bool hasBuff = getPlayer(luaVm, env)->getActiveBuffs()->hasBuff(
		BuffSourceType::Item,
		item);

	env.push<bool>(luaVm, hasBuff);
	return 1;
}

auto LuaExports::isActiveSkill(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	skill_id_t skill = env.get<skill_id_t>(luaVm, 1);
	bool hasBuff = getPlayer(luaVm, env)->getActiveBuffs()->hasBuff(
		GameLogicUtilities::isMobSkill(skill) ?
			BuffSourceType::MobSkill :
			BuffSourceType::Skill,
		skill);
	env.push<bool>(luaVm, hasBuff);
	return 1;
}

auto LuaExports::isGm(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, getPlayer(luaVm, env)->isGm());
	return 1;
}

auto LuaExports::isOnline(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, getPlayerDeduced(1, luaVm, env) != nullptr);
	return 1;
}

auto LuaExports::revertPlayer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.set<player_id_t>(luaVm, "system_player_id", env.get<player_id_t>(luaVm, "system_old_player_id"));
	return 0;
}

auto LuaExports::setAp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t ap = env.get<stat_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setAp(ap);
	return 0;
}

auto LuaExports::setDex(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t dex = env.get<stat_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setDex(dex);
	return 0;
}

auto LuaExports::setExp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	experience_t exp = env.get<experience_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setExp(exp);
	return 0;
}

auto LuaExports::setHp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	health_t hp = env.get<health_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setHp(hp);
	return 0;
}

auto LuaExports::setInt(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t intt = env.get<stat_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setInt(intt);
	return 0;
}

auto LuaExports::setJob(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	job_id_t job = env.get<job_id_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setJob(job);
	return 0;
}

auto LuaExports::setLevel(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	player_level_t level = env.get<player_level_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setLevel(level);
	return 0;
}

auto LuaExports::setLuk(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t luk = env.get<stat_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setLuk(luk);
	return 0;
}

auto LuaExports::setMap(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);

	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	if (env.is(luaVm, 2, LuaType::String)) {
		// Optional portal parameter
		string_t to = env.get<string_t>(luaVm, 2);
		Map *map = Maps::getMap(mapId);
		const PortalInfo * const destinationPortal = map->queryPortalName(to);
		getPlayer(luaVm, env)->setMap(mapId, destinationPortal);
		env.set<bool>(luaVm, "player_map_changed", true);
	}
	else {
		if (Maps::getMap(mapId) != nullptr) {
			getPlayer(luaVm, env)->setMap(mapId);
			env.set<bool>(luaVm, "player_map_changed", true);
		}
	}

	return 0;
}

auto LuaExports::setMaxHp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	health_t hp = env.get<health_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setMaxHp(hp);
	return 0;
}

auto LuaExports::setMaxMp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	health_t mp = env.get<health_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setMaxMp(mp);
	return 0;
}

auto LuaExports::setMp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	health_t mp = env.get<health_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setMp(mp);
	return 0;
}

auto LuaExports::setPlayer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto player = getPlayerDeduced(1, luaVm, env);
	if (player != nullptr) {
		env.set<player_id_t>(luaVm, "system_old_player_id", env.get<player_id_t>(luaVm, "system_player_id"));
		env.set<player_id_t>(luaVm, "system_player_id", player->getId());
	}
	env.push<bool>(luaVm, player != nullptr);
	return 1;
}

auto LuaExports::setPlayerVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto kvp = obtainSetVariablePair(luaVm, env);
	getPlayer(luaVm, env)->getVariables()->setVariable(kvp.first, kvp.second);
	return 0;
}

auto LuaExports::setSp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t sp = env.get<stat_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setSp(sp);
	return 0;
}

auto LuaExports::setStr(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	stat_t str = env.get<stat_t>(luaVm, 1);
	getPlayer(luaVm, env)->getStats()->setStr(str);
	return 0;
}

auto LuaExports::setStyle(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	int32_t id = env.get<int32_t>(luaVm, 1);
	int32_t type = GameLogicUtilities::getItemType(id);
	auto player = getPlayer(luaVm, env);
	if (type == 0) {
		player->setSkin(static_cast<skin_id_t>(id));
	}
	else if (type == 2) {
		player->setFace(id);
	}
	else if (type == 3) {
		player->setHair(id);
	}
	player->sendMap(Packets::Inventory::updatePlayer(player));
	return 0;
}

auto LuaExports::showInstructionBubble(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t msg = env.get<string_t>(luaVm, 1);
	coord_t width = env.get<coord_t>(luaVm, 2);
	// TODO FIXME lua
	// This export appears to believe this arg is "height" however the packet function thinks it's "time"
	// Figure out which is correct
	int16_t height = env.get<int16_t>(luaVm, 3);

	if (width == 0) {
		width = -1;
	}
	if (height == 0) {
		height = 5;
	}

	getPlayer(luaVm, env)->send(Packets::Player::instructionBubble(msg, width, height));
	return 0;
}

auto LuaExports::showMessage(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t msg = env.get<string_t>(luaVm, 1);
	int8_t type = env.get<int8_t>(luaVm, 2);
	getPlayer(luaVm, env)->send(Packets::Player::showMessage(msg, type));
	return 0;
}

// Effects
auto LuaExports::playFieldSound(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t val = env.get<string_t>(luaVm, 1);
	auto &packet = Packets::sendFieldSound(val);
	if (env.is(luaVm, 2, LuaType::Number)) {
		Maps::getMap(env.get<map_id_t>(luaVm, 2))->send(packet);
	}
	else {
		getPlayer(luaVm, env)->send(packet);
	}
	return 0;
}

auto LuaExports::playMinigameSound(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t val = env.get<string_t>(luaVm, 1);
	auto &packet = Packets::sendMinigameSound(val);
	if (env.is(luaVm, 2, LuaType::Number)) {
		Maps::getMap(env.get<map_id_t>(luaVm, 2))->send(packet);
	}
	else {
		getPlayer(luaVm, env)->send(packet);
	}
	return 0;
}

auto LuaExports::setMusic(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = -1;
	string_t music = env.get<string_t>(luaVm, 1);

	if (env.is(luaVm, 2, LuaType::Number)) {
		mapId = env.get<map_id_t>(luaVm, 2);
	}
	else if (auto player = getPlayer(luaVm, env)) {
		mapId = player->getMapId();
	}

	Map *map = Maps::getMap(mapId);
	if (mapId == -1 || map == nullptr) {
		throw std::invalid_argument("mapId must be a valid map");
	}

	map->setMusic(music);
	return 0;
}

auto LuaExports::showMapEffect(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t val = env.get<string_t>(luaVm, 1);
	if (env.is(luaVm, 2, LuaType::Number)) {
		map_id_t mapId = env.get<map_id_t>(luaVm, 2);
		Maps::getMap(mapId)->send(Packets::sendEffect(val));
	}
	else {
		getPlayer(luaVm, env)->sendMap(Packets::sendEffect(val));
	}
	return 0;
}

auto LuaExports::showMapEvent(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t val = env.get<string_t>(luaVm, 1);
	if (env.is(luaVm, 2, LuaType::Number)) {
		map_id_t mapId = env.get<map_id_t>(luaVm, 2);
		Maps::getMap(mapId)->send(Packets::sendEvent(val));
	}
	else {
		getPlayer(luaVm, env)->sendMap(Packets::sendEvent(val));
	}
	return 0;
}

// Map
auto LuaExports::clearDrops(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	Maps::getMap(mapId)->clearDrops(false);
	return 0;
}

auto LuaExports::clearMobs(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	bool distributeExpAndDrops = true;
	if (env.is(luaVm, 2, LuaType::Bool)) {
		distributeExpAndDrops = env.get<bool>(luaVm, 2);
	}
	Maps::getMap(mapId)->killMobs(nullptr, distributeExpAndDrops);
	return 0;
}

auto LuaExports::countMobs(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	mob_id_t mobId = 0;
	if (env.is(luaVm, 2, LuaType::Number)) {
		mobId = env.get<mob_id_t>(luaVm, 2);
	}
	env.push<int32_t>(luaVm, Maps::getMap(mapId)->countMobs(mobId));
	return 1;
}

auto LuaExports::setPortalState(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	string_t portalName = env.get<string_t>(luaVm, 2);
	bool enabled = env.get<bool>(luaVm, 3);

	Map *map = Maps::getMap(mapId);
	map->setPortalState(portalName, enabled);
	return 0;
}

auto LuaExports::getAllMapPlayerIds(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	if (Map *map = Maps::getMap(mapId)) {
		env.push<vector_t<player_id_t>>(luaVm, map->getAllPlayerIds());
		return 1;
	}
	return 0;
}

auto LuaExports::getNumPlayers(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	env.push<uint32_t>(luaVm, Maps::getMap(mapId)->getNumPlayers());
	return 1;
}

auto LuaExports::getReactorState(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	reactor_id_t reactorId = env.get<reactor_id_t>(luaVm, 2);
	Map *map = Maps::getMap(mapId);
	for (size_t i = 0; i < map->getNumReactors(); i++) {
		if (map->getReactor(i)->getReactorId() == reactorId) {
			env.push<int8_t>(luaVm, map->getReactor(i)->getState());
			return 1;
		}
	}
	env.push<int8_t>(luaVm, 0);
	return 1;
}

auto LuaExports::killMobs(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	mob_id_t mobId = env.get<mob_id_t>(luaVm, 1);
	int32_t killed = getPlayer(luaVm, env)->getMap()->killMobs(nullptr, true, mobId);
	env.push<int32_t>(luaVm, killed);
	return 1;
}

auto LuaExports::setBoatDocked(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	bool docked = env.get<bool>(luaVm, 2);
	Maps::getMap(mapId)->boatDock(docked);
	return 0;
}

auto LuaExports::setMapSpawn(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	mob_id_t spawn = env.get<mob_id_t>(luaVm, 2);
	Maps::getMap(mapId)->setMobSpawning(spawn);
	return 0;
}

auto LuaExports::setReactorState(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	reactor_id_t reactorId = env.get<reactor_id_t>(luaVm, 2);
	int8_t state = env.get<int8_t>(luaVm, 3);
	for (size_t i = 0; i < Maps::getMap(mapId)->getNumReactors(); i++) {
		Reactor *reactor = Maps::getMap(mapId)->getReactor(i);
		if (reactor->getReactorId() == reactorId) {
			reactor->setState(state, true);
			break;
		}
	}
	return 0;
}

auto LuaExports::showMapMessage(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t msg = env.get<string_t>(luaVm, 1);
	int8_t type = env.get<int8_t>(luaVm, 2);
	getPlayer(luaVm, env)->sendMap(Packets::Player::showMessage(msg, type));
	return 0;
}

auto LuaExports::showMapTimer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	seconds_t time = env.get<seconds_t>(luaVm, 2);
	Maps::getMap(mapId)->setMapTimer(time);
	return 0;
}

auto LuaExports::spawnMob(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	mob_id_t mobId = env.get<mob_id_t>(luaVm, 1);
	auto player = getPlayer(luaVm, env);
	env.push<map_object_t>(luaVm, player->getMap()->spawnMob(mobId, player->getPos())->getMapMobId());
	return 1;
}

auto LuaExports::spawnMobPos(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	mob_id_t mobId = env.get<mob_id_t>(luaVm, 2);
	coord_t x = env.get<coord_t>(luaVm, 3);
	coord_t y = env.get<coord_t>(luaVm, 4);
	foothold_id_t foothold = 0;
	if (env.is(luaVm, 5, LuaType::Number)) {
		foothold = env.get<foothold_id_t>(luaVm, 5);
	}
	env.push<map_object_t>(luaVm, Maps::getMap(mapId)->spawnMob(mobId, Point{x, y}, foothold)->getMapMobId());
	return 1;
}

// Mob
auto LuaExports::getMobFh(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<foothold_id_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getFoothold());
	return 1;
}

auto LuaExports::getMobHp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<int32_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getHp());
	return 1;
}

auto LuaExports::getMobMaxHp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<int32_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMaxHp());
	return 1;
}

auto LuaExports::getMobMaxMp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<int32_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMaxMp());
	return 1;
}

auto LuaExports::getMobMp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<int32_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMp());
	return 1;
}

auto LuaExports::getMobPosX(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<coord_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getPos().x);
	return 1;
}

auto LuaExports::getMobPosY(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<coord_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getPos().y);
	return 1;
}

auto LuaExports::getRealMobId(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	env.push<mob_id_t>(luaVm, Maps::getMap(mapId)->getMob(mapMobId)->getMobId());
	return 1;
}

auto LuaExports::killMob(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	auto mob = Maps::getMap(mapId)->getMob(mapMobId);
	if (mob != nullptr) {
		mob->kill();
	}
	return 0;
}

auto LuaExports::mobDropItem(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	map_object_t mapMobId = env.get<map_object_t>(luaVm, 2);
	item_id_t itemId = env.get<item_id_t>(luaVm, 3);
	slot_qty_t amount = 1;
	if (env.is(luaVm, 4, LuaType::Number)) {
		amount = env.get<slot_qty_t>(luaVm, 4);
	}
	auto mob = Maps::getMap(mapId)->getMob(mapMobId);
	if (mob != nullptr) {
		Item f{itemId, amount};
		Drop *drop = new Drop(mapId, f, mob->getPos(), 0);
		drop->setTime(0);
		drop->doDrop(mob->getPos());
	}
	return 0;
}

// Time
auto LuaExports::getDate(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, TimeUtilities::getDate());
	return 1;
}

auto LuaExports::getDay(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	bool isStringReturn = false;
	if (env.is(luaVm, 1, LuaType::Bool)) {
		isStringReturn = env.get<bool>(luaVm, 1);
	}
	if (isStringReturn) {
		env.push<string_t>(luaVm, TimeUtilities::getDayString(false));
	}
	else {
		env.push<int32_t>(luaVm, TimeUtilities::getDay());
	}
	return 1;
}

auto LuaExports::getHour(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	bool military = false;
	if (env.is(luaVm, 1, LuaType::Bool)) {
		military = env.get<bool>(luaVm, 1);
	}
	env.push<int32_t>(luaVm, TimeUtilities::getHour(military));
	return 1;
}

auto LuaExports::getMinute(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, TimeUtilities::getMinute());
	return 1;
}

auto LuaExports::getMonth(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, TimeUtilities::getMonth());
	return 1;
}

auto LuaExports::getNearestMinute(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<seconds_t>(luaVm, TimeUtilities::getDistanceToNextMinuteMark(env.get<int32_t>(luaVm, 1)));
	return 1;
}

auto LuaExports::getSecond(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, TimeUtilities::getSecond());
	return 1;
}

auto LuaExports::getTime(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<double>(luaVm, static_cast<double>(time(nullptr)));
	return 1;
}

auto LuaExports::getTimeZoneOffset(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, TimeUtilities::getTimeZoneOffset());
	return 1;
}

auto LuaExports::getWeek(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, TimeUtilities::getWeek());
	return 1;
}

auto LuaExports::getYear(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, TimeUtilities::getYear(false));
	return 1;
}

auto LuaExports::isDst(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, TimeUtilities::isDst());
	return 1;
}

// Rates
auto LuaExports::getDropMeso(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, ChannelServer::getInstance().getConfig().rates.dropMeso);
	return 1;
}

auto LuaExports::getDropRate(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, ChannelServer::getInstance().getConfig().rates.dropRate);
	return 1;
}

auto LuaExports::getExpRate(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, ChannelServer::getInstance().getConfig().rates.mobExpRate);
	return 1;
}

auto LuaExports::getGlobalDropMeso(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, ChannelServer::getInstance().getConfig().rates.globalDropMeso);
	return 1;
}

auto LuaExports::getGlobalDropRate(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, ChannelServer::getInstance().getConfig().rates.globalDropRate);
	return 1;
}

auto LuaExports::getQuestExpRate(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, ChannelServer::getInstance().getConfig().rates.questExpRate);
	return 1;
}

// Party
auto LuaExports::checkPartyFootholds(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	int8_t memberCount = env.get<int8_t>(luaVm, 1);
	Party *p = getPlayer(luaVm, env)->getParty();
	Result verified = Result::Failure;
	if (p != nullptr) {
		vector_t<vector_t<foothold_id_t>> footholds = env.get<vector_t<vector_t<foothold_id_t>>>(luaVm, 2);
		verified = p->checkFootholds(memberCount, footholds);
	}
	env.push<bool>(luaVm, verified == Result::Successful);
	return 1;
}

auto LuaExports::getAllPartyPlayerIds(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	if (Party *party = getPlayer(luaVm, env)->getParty()) {
		env.push<vector_t<player_id_t>>(luaVm, party->getAllPlayerIds());
		return 1;
	}
	return 0;
}

auto LuaExports::getPartyCount(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	int8_t memberCount = 0;
	Party *p = getPlayer(luaVm, env)->getParty();
	if (p != nullptr) {
		memberCount = p->getMembersCount();
	}
	env.push<int8_t>(luaVm, memberCount);
	return 1;
}

auto LuaExports::getPartyId(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	party_id_t id = 0;
	Party *p = getPlayer(luaVm, env)->getParty();
	if (p != nullptr) {
		id = p->getId();
	}
	env.push<party_id_t>(luaVm, id);
	return 1;
}

auto LuaExports::getPartyMapCount(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto player = getPlayer(luaVm, env);
	Party *p = player->getParty();
	int8_t members = 0;
	if (p != nullptr) {
		map_id_t mapId = env.get<map_id_t>(luaVm, 1);
		members = p->getMemberCountOnMap(mapId);
	}
	env.push<int8_t>(luaVm, members);
	return 1;
}

auto LuaExports::isPartyInLevelRange(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto player = getPlayer(luaVm, env);
	Party *p = player->getParty();
	bool isWithin = false;
	if (p != nullptr) {
		player_level_t lowBound = env.get<player_level_t>(luaVm, 1);
		player_level_t highBound = env.get<player_level_t>(luaVm, 2);
		isWithin = p->isWithinLevelRange(lowBound, highBound);
	}
	env.push<bool>(luaVm, isWithin);
	return 1;
}

auto LuaExports::isPartyLeader(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto player = getPlayer(luaVm, env);
	Party *p = player->getParty();
	bool isLeader = false;
	if (p != nullptr) {
		isLeader = player == p->getLeader();
	}
	env.push<bool>(luaVm, isLeader);
	return 1;
}

auto LuaExports::verifyPartyFootholds(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	Party *p = getPlayer(luaVm, env)->getParty();
	Result verified = Result::Failure;
	if (p != nullptr) {
		vector_t<vector_t<foothold_id_t>> footholds = env.get<vector_t<vector_t<foothold_id_t>>>(luaVm, 1);
		verified = p->verifyFootholds(footholds);
	}
	env.push<bool>(luaVm, verified == Result::Successful);
	return 1;
}

auto LuaExports::warpParty(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	string_t to;
	if (env.is(luaVm, 2, LuaType::String)) {
		// Optional portal parameter
		string_t to = env.get<string_t>(luaVm, 2);
	}
	auto player = getPlayer(luaVm, env);
	Party *p = player->getParty();
	if (p != nullptr) {
		p->warpAllMembers(mapId, to);
	}
	return 0;
}

// Instance
auto LuaExports::addInstanceMap(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	getInstance(luaVm, env)->addMap(mapId);
	return 0;
}

auto LuaExports::addInstanceParty(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	party_id_t id = env.get<party_id_t>(luaVm, 1);
	if (Party *p = ChannelServer::getInstance().getPlayerDataProvider().getParty(id)) {
		getInstance(luaVm, env)->addParty(p);
	}
	return 0;
}

auto LuaExports::addInstancePlayer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto player = getPlayerDeduced(1, luaVm, env);
	getInstance(luaVm, env)->addPlayer(player);
	return 0;
}

auto LuaExports::checkInstanceTimer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t name = env.get<string_t>(luaVm, 1);
	env.push<int32_t>(luaVm, static_cast<int32_t>(getInstance(luaVm, env)->getTimerSecondsRemaining(name).count()));
	return 1;
}

auto LuaExports::createInstance(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t name = env.get<string_t>(luaVm, 1);
	seconds_t time = env.get<seconds_t>(luaVm, 2);
	bool showTimer = env.get<bool>(luaVm, 3);
	seconds_t persistent{0};
	map_id_t map = 0;
	player_id_t id = 0;
	auto player = getPlayer(luaVm, env);
	if (env.is(luaVm, 4, LuaType::Number)) {
		persistent = env.get<seconds_t>(luaVm, 4);
	}
	if (player != nullptr) {
		map = player->getMapId();
		id = player->getId();
	}
	Instance *instance = new Instance(name, map, id, time, persistent, showTimer);
	ChannelServer::getInstance().getInstances().addInstance(instance);
	instance->beginInstance();

	if (instance->showTimer()) {
		instance->showTimer(true, true);
	}

	env.set<string_t>(luaVm, "system_instance_name", name);
	return 0;
}

auto LuaExports::deleteInstanceVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getInstance(luaVm, env)->getVariables()->deleteVariable(env.get<string_t>(luaVm, 1));
	return 0;
}

auto LuaExports::getAllInstancePlayerIds(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<vector_t<player_id_t>>(luaVm, getInstance(luaVm, env)->getAllPlayerIds());
	return 1;
}

auto LuaExports::getInstancePlayerCount(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<uint32_t>(luaVm, getInstance(luaVm, env)->getPlayerNum());
	return 1;
}

auto LuaExports::getInstancePlayerId(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto player = getPlayerDeduced(1, luaVm, env);
	env.push<player_id_t>(luaVm, player->getId());
	return 1;
}

auto LuaExports::getInstanceVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	VariableType::Type returnType = VariableType::String;
	if (env.is(luaVm, 2, LuaType::Number)) {
		returnType = (VariableType::Type)env.get<int32_t>(luaVm, 2);
	}
	string_t val = getInstance(luaVm, env)->getVariables()->getVariable(env.get<string_t>(luaVm, 1));
	pushGetVariableData(luaVm, env, val, returnType);
	return 1;
}

auto LuaExports::isInstance(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, ChannelServer::getInstance().getInstances().isInstance(env.get<string_t>(luaVm, 1)));
	return 1;
}

auto LuaExports::isInstanceMap(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, getInstance(luaVm, env)->isInstanceMap(env.get<map_id_t>(luaVm, 1)));
	return 1;
}

auto LuaExports::isInstancePersistent(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<bool>(luaVm, getInstance(luaVm, env)->getPersistence().count() != 0);
	return 1;
}

auto LuaExports::markForDelete(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getInstance(luaVm, env)->markForDelete();
	return 0;
}

auto LuaExports::moveAllPlayers(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);

	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	if (env.is(luaVm, 2, LuaType::String)) {
		// Optional portal parameter
		string_t to = env.get<string_t>(luaVm, 2);
		Map *map = Maps::getMap(mapId);
		const PortalInfo * const destinationPortal = map->queryPortalName(to);
		getInstance(luaVm, env)->moveAllPlayers(mapId, true, destinationPortal);
	}
	else {
		getInstance(luaVm, env)->moveAllPlayers(mapId, true);
	}
	return 0;
}

auto LuaExports::passPlayersBetweenInstances(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);

	map_id_t mapId = env.get<map_id_t>(luaVm, 1);
	if (env.is(luaVm, 2, LuaType::String)) {
		// Optional portal parameter
		string_t to = env.get<string_t>(luaVm, 2);
		Map *map = Maps::getMap(mapId);
		const PortalInfo * const destinationPortal = map->queryPortalName(to);
		getInstance(luaVm, env)->moveAllPlayers(mapId, false, destinationPortal);
	}
	else {
		getInstance(luaVm, env)->moveAllPlayers(mapId, false);
	}
	return 0;
}

auto LuaExports::removeAllInstancePlayers(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getInstance(luaVm, env)->removeAllPlayers();
	return 0;
}

auto LuaExports::removeInstancePlayer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto player = getPlayerDeduced(1, luaVm, env);
	getInstance(luaVm, env)->removePlayer(player);
	return 0;
}

auto LuaExports::respawnInstanceMobs(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = Vana::Maps::NoMap;
	if (env.is(luaVm, 1, LuaType::Number)) {
		mapId = env.get<map_id_t>(luaVm, 1);
	}
	getInstance(luaVm, env)->respawnMobs(mapId);
	return 0;
}

auto LuaExports::respawnInstanceReactors(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	map_id_t mapId = Vana::Maps::NoMap;
	if (env.is(luaVm, 1, LuaType::Number)) {
		mapId = env.get<map_id_t>(luaVm, 1);
	}
	getInstance(luaVm, env)->respawnReactors(mapId);
	return 0;
}

auto LuaExports::revertInstance(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.set<string_t>(luaVm, "system_instance_name", env.get<string_t>(luaVm, "system_old_instance_name"));
	return 0;
}

auto LuaExports::setInstance(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t instanceName = env.get<string_t>(luaVm, 1);
	Instance *instance = ChannelServer::getInstance().getInstances().getInstance(instanceName);
	if (instance != nullptr) {
		string_t oldInstanceName = env.get<string_t>(luaVm, "system_instance_name");
		env.set<string_t>(luaVm, "system_old_instance_name", oldInstanceName);
		env.set<string_t>(luaVm, "system_instance_name", instanceName);
	}
	env.push<bool>(luaVm, instance != nullptr);
	return 1;
}

auto LuaExports::setInstancePersistence(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	seconds_t persistence = env.get<seconds_t>(luaVm, 1);
	getInstance(luaVm, env)->setPersistence(persistence);
	return 0;
}

auto LuaExports::setInstanceReset(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	bool reset = env.get<bool>(luaVm, 1);
	getInstance(luaVm, env)->setResetAtEnd(reset);
	return 0;
}

auto LuaExports::setInstanceVariable(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	auto kvp = obtainSetVariablePair(luaVm, env);
	getInstance(luaVm, env)->getVariables()->setVariable(kvp.first, kvp.second);
	return 0;
}

auto LuaExports::showInstanceTime(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getInstance(luaVm, env)->showTimer(env.get<bool>(luaVm, 1));
	return 0;
}

auto LuaExports::startInstanceFutureTimer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t name = env.get<string_t>(luaVm, 1);
	seconds_t time = env.get<seconds_t>(luaVm, 2);
	seconds_t persistence{0};
	if (env.is(luaVm, 3, LuaType::Number)) {
		persistence = env.get<seconds_t>(luaVm, 3);
	}
	env.push<bool>(luaVm, getInstance(luaVm, env)->addFutureTimer(name, time, persistence));
	return 1;
}

auto LuaExports::startInstanceSecondOfHourTimer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t name = env.get<string_t>(luaVm, 1);
	int16_t secondOfHour = env.get<int16_t>(luaVm, 2);
	seconds_t persistence{0};
	if (env.is(luaVm, 3, LuaType::Number)) {
		persistence = env.get<seconds_t>(luaVm, 3);
	}
	env.push<bool>(luaVm, getInstance(luaVm, env)->addSecondOfHourTimer(name, secondOfHour, persistence));
	return 1;
}

auto LuaExports::stopAllInstanceTimers(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getInstance(luaVm, env)->removeAllTimers();
	return 0;
}

auto LuaExports::stopInstanceTimer(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t name = env.get<string_t>(luaVm, 1);
	getInstance(luaVm, env)->removeTimer(name);
	return 0;
}

}
}