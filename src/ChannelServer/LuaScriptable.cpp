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
#include "LuaScriptable.h"
#include "Player.h"
#include "Players.h"
#include "Maps.h"
#include "Mobs.h"
#include "Reactors.h"
#include "Quests.h"
#include "Levels.h"
#include "Shops.h"
#include "PlayerPacket.h"
#include "InventoryPacket.h"
#include "Randomizer.h"

LuaScriptable::LuaScriptable(const string &filename, int playerid) : filename(filename), playerid(playerid), luaVm(lua_open()) {
	initialize();
}

LuaScriptable::~LuaScriptable() {
	lua_close(luaVm);
}

void LuaScriptable::initialize() {
	luaopen_base(luaVm);

	lua_pushinteger(luaVm, playerid); // Pushing id for reference from static functions
	lua_setglobal(luaVm, "playerid");

	lua_register(luaVm, "addSkillLevel", &LuaExports::addSkillLevel);
	lua_register(luaVm, "giveItem", &LuaExports::giveItem);
	lua_register(luaVm, "giveMesos", &LuaExports::giveMesos);
	lua_register(luaVm, "giveEXP", &LuaExports::giveEXP);
	lua_register(luaVm, "giveSP", &LuaExports::giveSP);
	lua_register(luaVm, "giveAP", &LuaExports::giveAP);
	lua_register(luaVm, "getSTR", &LuaExports::getSTR);
	lua_register(luaVm, "getDEX", &LuaExports::getDEX);
	lua_register(luaVm, "getINT", &LuaExports::getINT);
	lua_register(luaVm, "getLUK", &LuaExports::getLUK);
	lua_register(luaVm, "getJob", &LuaExports::getJob);
	lua_register(luaVm, "getLevel", &LuaExports::getLevel);
	lua_register(luaVm, "getGender", &LuaExports::getGender);
	lua_register(luaVm, "getItemAmount", &LuaExports::getItemAmount);
	lua_register(luaVm, "getSkillLevel", &LuaExports::getSkillLevel);
	lua_register(luaVm, "getMesos", &LuaExports::getMesos);
	lua_register(luaVm, "getMap", &LuaExports::getMap);
	lua_register(luaVm, "getEXP", &LuaExports::getEXP);
	lua_register(luaVm, "getHP", &LuaExports::getHP);
	lua_register(luaVm, "getMHP", &LuaExports::getMHP);
	lua_register(luaVm, "getRMHP", &LuaExports::getRMHP);
	lua_register(luaVm, "getMP", &LuaExports::getMP);
	lua_register(luaVm, "getMMP", &LuaExports::getMMP);
	lua_register(luaVm, "getRMMP", &LuaExports::getMMP);
	lua_register(luaVm, "getHair", &LuaExports::getHair);
	lua_register(luaVm, "getEyes", &LuaExports::getEyes);
	lua_register(luaVm, "getPlayerVariable", &LuaExports::getPlayerVariable);
	lua_register(luaVm, "getNumPlayers", &LuaExports::getNumPlayers);
	lua_register(luaVm, "getReactorState", &LuaExports::getReactorState);
	lua_register(luaVm, "getRandomNumber", &LuaExports::getRandomNumber);
	lua_register(luaVm, "killMob", &LuaExports::killMob);
	lua_register(luaVm, "setStyle", &LuaExports::setStyle);
	lua_register(luaVm, "setMap", &LuaExports::setMap);
	lua_register(luaVm, "setMusic", &LuaExports::setMusic);
	lua_register(luaVm, "setEXP", &LuaExports::setEXP);
	lua_register(luaVm, "setReactorsState", &LuaExports::setReactorsState);
	lua_register(luaVm, "setHP", &LuaExports::setHP);
	lua_register(luaVm, "setMHP", &LuaExports::setMHP);
	lua_register(luaVm, "setRMHP", &LuaExports::setRMHP);
	lua_register(luaVm, "setMP", &LuaExports::setMP);
	lua_register(luaVm, "setMMP", &LuaExports::setMMP);
	lua_register(luaVm, "setRMMP", &LuaExports::setRMMP);
	lua_register(luaVm, "setSTR", &LuaExports::setSTR);
	lua_register(luaVm, "setDEX", &LuaExports::setDEX);
	lua_register(luaVm, "setINT", &LuaExports::setINT);
	lua_register(luaVm, "setLUK", &LuaExports::setLUK);
	lua_register(luaVm, "setJob", &LuaExports::setJob);
	lua_register(luaVm, "setLevel", &LuaExports::setLevel);
	lua_register(luaVm, "setPlayerVariable", &LuaExports::setPlayerVariable);
	lua_register(luaVm, "showShop", &LuaExports::showShop);
	lua_register(luaVm, "showMessage", &LuaExports::showMessage);
	lua_register(luaVm, "showMapMessage", &LuaExports::showMapMessage);
	lua_register(luaVm, "spawnMob", &LuaExports::spawnMob);
	lua_register(luaVm, "spawnMobPos", &LuaExports::spawnMobPos);
	lua_register(luaVm, "deletePlayerVariable", &LuaExports::deletePlayerVariable);
}

void LuaScriptable::run() {
	luaL_dofile(luaVm, filename.c_str());
}

void LuaScriptable::setVariable(const string &name, int val) {
	lua_pushinteger(luaVm, val);
	lua_setglobal(luaVm, name.c_str());
}

void LuaScriptable::setVariable(const string &name, const string &val) {
	lua_pushstring(luaVm, val.c_str());
	lua_setglobal(luaVm, name.c_str());
}

Player * LuaExports::getPlayer(lua_State *luaVm) {
	lua_getglobal(luaVm, "playerid");
	return Players::players[lua_tointeger(luaVm, -1)];
}

int LuaExports::addSkillLevel(lua_State *luaVm) {
	int skillid = lua_tointeger(luaVm, -2);
	int level = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->skills->addSkillLevel(skillid, level);
	return 1;
}

int LuaExports::giveItem(lua_State *luaVm) {
	int itemid = lua_tointeger(luaVm, -2);
	int amount = lua_tointeger(luaVm, -1);
	Quests::giveItem(getPlayer(luaVm), itemid, amount);
	return 1;
}

int LuaExports::giveMesos(lua_State *luaVm) {
	int mesos = lua_tointeger(luaVm, -1);
	Quests::giveMesos(getPlayer(luaVm), mesos);
	return 1;
}

int LuaExports::giveEXP(lua_State *luaVm) {
	int exp = lua_tointeger(luaVm, -1);
	Levels::giveEXP(getPlayer(luaVm), exp, 1);
	return 1;
}

int LuaExports::giveSP(lua_State *luaVm) {
	short sp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setSp(getPlayer(luaVm)->getSp()+sp);
	return 1;
}

int LuaExports::giveAP(lua_State *luaVm) {
	short ap = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setAp(getPlayer(luaVm)->getAp()+ap);
	return 1;
}

int LuaExports::getSTR(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getStr());
	return 1;
}

int LuaExports::getDEX(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getDex());
	return 1;
}

int LuaExports::getINT(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getInt());
	return 1;
}

int LuaExports::getLUK(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getLuk());
	return 1;
}

int LuaExports::getJob(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getJob());
	return 1;
}

int LuaExports::getLevel(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getLevel());
	return 1;
}

int LuaExports::getGender(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getGender());
	return 1;
}

int LuaExports::getItemAmount(lua_State *luaVm) {
	int itemid = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->inv->getItemAmount(itemid));
	return 1;
}

int LuaExports::getSkillLevel(lua_State *luaVm) {
	int skillid = lua_tointeger(luaVm, -1);
	lua_pushnumber(luaVm, getPlayer(luaVm)->skills->getSkillLevel(skillid));
	return 1;
}

int LuaExports::getMesos(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->inv->getMesos());
	return 1;
}

int LuaExports::getMap(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getMap());
	return 1;
}

int LuaExports::getEXP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getExp());
	return 1;
}

int LuaExports::getHP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getHP());
	return 1;
}

int LuaExports::getMHP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getMHP());
	return 1;
}

int LuaExports::getRMHP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getRMHP());
	return 1;
}

int LuaExports::getMP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getMP());
	return 1;
}

int LuaExports::getMMP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getMMP());
	return 1;
}

int LuaExports::getRMMP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getRMMP());
	return 1;
}

int LuaExports::getHair(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getHair());
	return 1;
}

int LuaExports::getEyes(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getEyes());
	return 1;
}

int LuaExports::getPlayerVariable(lua_State *luaVm) {
	std::string key = string(lua_tostring(luaVm, -1));
	lua_pushstring(luaVm, getPlayer(luaVm)->getVariable(key).c_str());
	return 1;
}

int LuaExports::getNumPlayers(lua_State *luaVm) {
	int mapid = lua_tointeger(luaVm, -1);
	lua_pushinteger(luaVm, Maps::maps[mapid]->getNumPlayers());
	return 1;
}

int LuaExports::getReactorState(lua_State *luaVm) {
	int mapid = lua_tointeger(luaVm, -2);
	int reactorid = lua_tointeger(luaVm, -1);
	for (unsigned int i = 0; i < Maps::maps[mapid]->getNumReactors(); i++) {
		if (Maps::maps[mapid]->getReactor(i)->getReactorID() == reactorid) {
			lua_pushinteger(luaVm, Maps::maps[mapid]->getReactor(i)->getState());
			return 1;
		}
	}
	lua_pushinteger(luaVm, 0);
	return 1;
}

int LuaExports::getRandomNumber(lua_State *luaVm) {
	int number = lua_tointeger(luaVm, -1);
	lua_pushinteger(luaVm, Randomizer::Instance()->randInt(number-1)+1);
	return 1;
}

int LuaExports::killMob(lua_State *luaVm) {
	int mobid = lua_tointeger(luaVm, -1);
	int mapid = getPlayer(luaVm)->getMap();
	Maps::maps[mapid]->killMobs(getPlayer(luaVm), mobid);
	return 1;
}

int LuaExports::setStyle(lua_State *luaVm) {
	int id = lua_tointeger(luaVm, -1);
	if (id/10000 == 0) {
		getPlayer(luaVm)->setSkin((char)id);
	}
	else if (id/10000 == 2) {
		getPlayer(luaVm)->setEyes(id);
	}
	else if (id/10000 == 3) {
		getPlayer(luaVm)->setHair(id);
	}
	InventoryPacket::updatePlayer(getPlayer(luaVm));
	return 1;
}

int LuaExports::setMap(lua_State *luaVm) {
	int mapid = lua_tointeger(luaVm, -1);
	if (Maps::maps.find(mapid) != Maps::maps.end())
		Maps::changeMap(getPlayer(luaVm), mapid, 0);
	return 1;
}

int LuaExports::setMusic(lua_State *luaVm) {
	Maps::changeMusic(getPlayer(luaVm)->getMap(), lua_tostring(luaVm, -1));
	return 1;
}

int LuaExports::setEXP(lua_State *luaVm) {
	int exp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setExp(exp);
	return 1;
}

int LuaExports::setReactorsState(lua_State *luaVm) {
	int mapid = lua_tointeger(luaVm, -3);
	int reactorid = lua_tointeger(luaVm, -2);
	int state = lua_tointeger(luaVm, -1);
	for (size_t i = 0; i < Maps::maps[mapid]->getNumReactors(); i++) {
		Reactor *reactor = Maps::maps[mapid]->getReactor(i);
		if (reactor->getReactorID() == reactorid) {
			reactor->setState(state, true);
			break;
		}
	}
	return 1;
}

int LuaExports::setHP(lua_State *luaVm) {
	int hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setHP(hp);
	return 1;
}

int LuaExports::setMHP(lua_State *luaVm) {
	int hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setMHP(hp);
	return 1;
}

int LuaExports::setRMHP(lua_State *luaVm) {
	int hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setRMHP(hp);
	return 1;
}

int LuaExports::setMP(lua_State *luaVm) {
	int mp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setMP(mp);
	return 1;
}

int LuaExports::setMMP(lua_State *luaVm) {
	int mp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setMMP(mp);
	return 1;
}

int LuaExports::setRMMP(lua_State *luaVm) {
	int mp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setRMMP(mp);
	return 1;
}

int LuaExports::setSTR(lua_State *luaVm) {
	short str = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setStr(str);
	return 1;
}

int LuaExports::setDEX(lua_State *luaVm) {
	short dex = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setDex(dex);
	return 1;
}

int LuaExports::setINT(lua_State *luaVm) {
	short intt = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setInt(intt);
	return 1;
}

int LuaExports::setLUK(lua_State *luaVm) {
	short luk = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setLuk(luk);
	return 1;
}

int LuaExports::setJob(lua_State *luaVm) {
	short job = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setJob(job);
	return 1;
}

int LuaExports::setLevel(lua_State *luaVm) {
	int level = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setLevel(level);
	return 1;
}

int LuaExports::setPlayerVariable(lua_State *luaVm) {
	std::string value = string(lua_tostring(luaVm, -1));
	std::string key = string(lua_tostring(luaVm, -2));
	getPlayer(luaVm)->setVariable(key, value);
	return 1;
}

int LuaExports::showShop(lua_State *luaVm) {
	int shopid = lua_tointeger(luaVm, -1);
	Shops::showShop(getPlayer(luaVm), shopid);
	return 1;
}

int LuaExports::showMessage(lua_State *luaVm) {
	std::string msg = lua_tostring(luaVm, -2);
	int type = lua_tointeger(luaVm, -1);
	PlayerPacket::showMessage(getPlayer(luaVm), msg, type);
	return 1;
}

int LuaExports::showMapMessage(lua_State *luaVm) {
	std::string msg = lua_tostring(luaVm, -2);
	int type = lua_tointeger(luaVm, -1);
	int map = getPlayer(luaVm)->getMap();
	for (size_t i = 0; i < Maps::maps[map]->getNumPlayers(); i++) {
		PlayerPacket::showMessage(Maps::maps[map]->getPlayer(i), msg, type);
	}
	return 1;
}

int LuaExports::spawnMob(lua_State *luaVm) {
	int mobid = lua_tointeger(luaVm, -1);
	Mobs::spawnMob(getPlayer(luaVm), mobid);
	return 1;
}

int LuaExports::spawnMobPos(lua_State *luaVm) {
	int mobid = lua_tointeger(luaVm, -3);
	short x = lua_tointeger(luaVm, -2);
	short y = lua_tointeger(luaVm, -1);
	Mobs::spawnMobPos(getPlayer(luaVm)->getMap(), mobid, Pos(x, y));
	return 1;
}

int LuaExports::deletePlayerVariable(lua_State *luaVm) {
	std::string key = string(lua_tostring(luaVm, -1));
	getPlayer(luaVm)->deleteVariable(key);
	return 1;
}
