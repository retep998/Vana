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

#include "LuaNPC.h"
#include "NPCs.h"
#include "Player.h"
#include "Players.h"
#include "Quests.h"
#include "Maps.h"
#include "Levels.h"
#include "Shops.h"
#include "InventoryPacket.h"
#include <string>

hash_map <int, PortalInfo *> LuaNPCExports::portals;

LuaNPC::LuaNPC(const char *filename, int playerid, PortalInfo *portal) {
	lua_State *luaVm = lua_open();

	lua_pushinteger(luaVm, playerid); // Pushing id for reference from static functions
	lua_setglobal(luaVm, "playerid");
	if (Players::players[playerid]->getNPC() != NULL) {
		lua_pushinteger(luaVm, Players::players[playerid]->getNPC()->getState());
		lua_setglobal(luaVm, "state");
	}
	if (portal != NULL) {
		LuaNPCExports::portals[playerid] = portal;
	}

	lua_register(luaVm, "addText", &LuaNPCExports::addText);
	lua_register(luaVm, "addChar", &LuaNPCExports::addChar);
	lua_register(luaVm, "sendSimple", &LuaNPCExports::sendSimple);
	lua_register(luaVm, "sendYesNo", &LuaNPCExports::sendYesNo);
	lua_register(luaVm, "sendNext", &LuaNPCExports::sendNext);
	lua_register(luaVm, "sendBackNext", &LuaNPCExports::sendBackNext);
	lua_register(luaVm, "sendBackOK", &LuaNPCExports::sendBackOK);
	lua_register(luaVm, "sendOK", &LuaNPCExports::sendOK);
	lua_register(luaVm, "sendAcceptDecline", &LuaNPCExports::sendAcceptDecline);
	lua_register(luaVm, "sendGetText", &LuaNPCExports::sendGetText);
	lua_register(luaVm, "sendGetNumber", &LuaNPCExports::sendGetNumber);
	lua_register(luaVm, "sendStyle", &LuaNPCExports::sendStyle);
	lua_register(luaVm, "giveItem", &LuaNPCExports::giveItem);
	lua_register(luaVm, "giveMesos", &LuaNPCExports::giveMesos);
	lua_register(luaVm, "giveEXP", &LuaNPCExports::giveEXP);
	lua_register(luaVm, "getLevel", &LuaNPCExports::getLevel);
	lua_register(luaVm, "getGender", &LuaNPCExports::getGender);
	lua_register(luaVm, "getItemAmount", &LuaNPCExports::getItemAmount);
	lua_register(luaVm, "getMesos", &LuaNPCExports::getMesos);
	lua_register(luaVm, "getMap", &LuaNPCExports::getMap);
	lua_register(luaVm, "getHP", &LuaNPCExports::getHP);
	lua_register(luaVm, "getHair", &LuaNPCExports::getHair);
	lua_register(luaVm, "getEyes", &LuaNPCExports::getEyes);
	lua_register(luaVm, "getSelected", &LuaNPCExports::getSelected);
	lua_register(luaVm, "getNumber", &LuaNPCExports::getNumber);
	lua_register(luaVm, "getText", &LuaNPCExports::getText);
	lua_register(luaVm, "getVariable", &LuaNPCExports::getVariable);
	lua_register(luaVm, "getPlayerVariable", &LuaNPCExports::getPlayerVariable);
	lua_register(luaVm, "getPortalFrom", &LuaNPCExports::getPortalFrom);
	lua_register(luaVm, "setState", &LuaNPCExports::setState);
	lua_register(luaVm, "setStyle", &LuaNPCExports::setStyle);
	lua_register(luaVm, "setMap", &LuaNPCExports::setMap);
	lua_register(luaVm, "setHP", &LuaNPCExports::setHP);
	lua_register(luaVm, "setVariable", &LuaNPCExports::setVariable);
	lua_register(luaVm, "setPlayerVariable", &LuaNPCExports::setPlayerVariable);
	lua_register(luaVm, "setPortalTo", &LuaNPCExports::setPortalTo);
	lua_register(luaVm, "setPortalToId", &LuaNPCExports::setPortalToId);
	lua_register(luaVm, "showShop", &LuaNPCExports::showShop);
	lua_register(luaVm, "addQuest", &LuaNPCExports::addQuest);
	lua_register(luaVm, "endQuest", &LuaNPCExports::endQuest);
	lua_register(luaVm, "endNPC", &LuaNPCExports::end); // end() doesn't work (reserved?)

	luaL_dofile(luaVm, filename);
	lua_close(luaVm);
}

NPC * LuaNPCExports::getNPC(lua_State *luaVm) {
	return getPlayer(luaVm)->getNPC();
}

PortalInfo * LuaNPCExports::getPortal(lua_State *luaVm) {
	return portals[getPlayer(luaVm)->getPlayerid()];
}

Player * LuaNPCExports::getPlayer(lua_State *luaVm) {
	lua_getglobal(luaVm, "playerid");
	return Players::players[lua_tointeger(luaVm, -1)];
}

int LuaNPCExports::addText(lua_State *luaVm) {
	getNPC(luaVm)->addText(lua_tostring(luaVm, -1));
	return 1;
}

int LuaNPCExports::addChar(lua_State *luaVm) {
	getNPC(luaVm)->addChar((char) lua_tointeger(luaVm, -1));
	return 1;
}

int LuaNPCExports::sendSimple(lua_State *luaVm) {
	getNPC(luaVm)->sendSimple();
	return 1;
}
	
int LuaNPCExports::sendYesNo(lua_State *luaVm) {
	getNPC(luaVm)->sendYesNo();
	return 1;
}

int LuaNPCExports::sendNext(lua_State *luaVm) {
	getNPC(luaVm)->sendNext();
	return 1;
}

int LuaNPCExports::sendBackNext(lua_State *luaVm) {
	getNPC(luaVm)->sendBackNext();
	return 1;
}

int LuaNPCExports::sendBackOK(lua_State *luaVm) {
	getNPC(luaVm)->sendBackOK();
	return 1;
}

int LuaNPCExports::sendOK(lua_State *luaVm) {
	getNPC(luaVm)->sendOK();
	return 1;
}

int LuaNPCExports::sendAcceptDecline(lua_State *luaVm) {
	getNPC(luaVm)->sendAcceptDecline();
	return 1;
}

int LuaNPCExports::sendGetText(lua_State *luaVm) {
	getNPC(luaVm)->sendGetText();
	return 1;
}

int LuaNPCExports::sendGetNumber(lua_State *luaVm) {
	getNPC(luaVm)->sendGetNumber(lua_tointeger(luaVm, -3), lua_tointeger(luaVm, -2), lua_tointeger(luaVm, -1));
	return 1;
}

int LuaNPCExports::sendStyle(lua_State *luaVm) {
	char size = (char) lua_tointeger(luaVm, -1);
	int *styles = new int[size];

	lua_pop(luaVm, 1);
	lua_pushnil(luaVm);
	while (lua_next(luaVm, -2) != 0) {
		int a = lua_tointeger(luaVm, -2)-1;
		int b = lua_tointeger(luaVm, -1);
		styles[lua_tointeger(luaVm, -2)-1] = lua_tointeger(luaVm, -1);
		lua_pop(luaVm, 1);
	}

	getNPC(luaVm)->sendStyle(styles, size);
	return 1;
}

int LuaNPCExports::giveItem(lua_State *luaVm) {
	Quests::giveItem(getNPC(luaVm)->getPlayer(), lua_tointeger(luaVm, -2), lua_tointeger(luaVm, -1));
	return 1;
}

int LuaNPCExports::giveMesos(lua_State *luaVm) {
	Quests::giveMesos(getNPC(luaVm)->getPlayer(), lua_tointeger(luaVm, -1));
	return 1;
}

int LuaNPCExports::giveEXP(lua_State *luaVm) {
	Levels::giveEXP(getNPC(luaVm)->getPlayer(), lua_tointeger(luaVm, -1), 1);
	return 1;
}

int LuaNPCExports::getLevel(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getLevel());
	return 1;
}

int LuaNPCExports::getGender(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getGender());
	return 1;
}

int LuaNPCExports::getItemAmount(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->inv->getItemAmount(lua_tointeger(luaVm, -1)));
	return 1;
}

int LuaNPCExports::getMesos(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->inv->getMesos());
	return 1;
}

int LuaNPCExports::getMap(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getMap());
	return 1;
}

int LuaNPCExports::getHP(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getHP());
	return 1;
}

int LuaNPCExports::getHair(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getHair());
	return 1;
}

int LuaNPCExports::getEyes(lua_State *luaVm) {
	lua_pushnumber(luaVm, getPlayer(luaVm)->getEyes());
	return 1;
}

int LuaNPCExports::getSelected(lua_State *luaVm) {
	lua_pushnumber(luaVm, getNPC(luaVm)->getSelected());
	return 1;
}

int LuaNPCExports::getNumber(lua_State *luaVm) {
	lua_pushnumber(luaVm, getNPC(luaVm)->getNumber());
	return 1;
}

int LuaNPCExports::getText(lua_State *luaVm) {
	lua_pushstring(luaVm, getNPC(luaVm)->getText());
	return 1;
}

int LuaNPCExports::getVariable(lua_State *luaVm) {
	std::string key = string(lua_tostring(luaVm, -1));
	lua_pushnumber(luaVm, getNPC(luaVm)->getVariable(key));
	return 1;
}

int LuaNPCExports::getPlayerVariable(lua_State *luaVm) {
	std::string key = string(lua_tostring(luaVm, -1));
	lua_pushstring(luaVm, getPlayer(luaVm)->getVariable(key).c_str());
	return 1;
}

int LuaNPCExports::getPortalFrom(lua_State *luaVm) {
	lua_pushstring(luaVm, getPortal(luaVm)->from);
	return 1;
}

int LuaNPCExports::setState(lua_State *luaVm) {
	getNPC(luaVm)->setState(lua_tointeger(luaVm, -1));
	return 1;
}

int LuaNPCExports::setStyle(lua_State *luaVm) {
	int id = lua_tointeger(luaVm, -1);
	if(id/10000 == 0){
		getPlayer(luaVm)->setSkin((char)id);
	}
	else if(id/10000 == 2){
		getPlayer(luaVm)->setEyes(id);
	}
	else if(id/10000 == 3){
		getPlayer(luaVm)->setHair(id);
	}
	InventoryPacket::updatePlayer(getNPC(luaVm)->getPlayer());
	return 1;
}

int LuaNPCExports::setMap(lua_State *luaVm) {
	int mapid = lua_tointeger(luaVm, -1);
	if(Maps::info.find(mapid) != Maps::info.end())
		Maps::changeMap(getNPC(luaVm)->getPlayer(), mapid, 0);
	return 1;
}

int LuaNPCExports::setHP(lua_State *luaVm) {
	int hp = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->setHP(hp);
	return 1;
}

int LuaNPCExports::setVariable(lua_State *luaVm) {
	int value = lua_tointeger(luaVm, -1);
	std::string key = string(lua_tostring(luaVm, -2));
	getNPC(luaVm)->setVariable(key, value);
	return 1;
}

int LuaNPCExports::setPlayerVariable(lua_State *luaVm) {
	std::string value = string(lua_tostring(luaVm, -1));
	std::string key = string(lua_tostring(luaVm, -2));
	getPlayer(luaVm)->setVariable(key, value);
	return 1;
}

int LuaNPCExports::setPortalTo(lua_State *luaVm) {
	strcpy_s(getPortal(luaVm)->to, lua_tostring(luaVm, -1));
	return 1;
}

int LuaNPCExports::setPortalToId(lua_State *luaVm) {
	int toid = lua_tointeger(luaVm, -1);
	getPortal(luaVm)->toid = toid;
	return 1;
}

int LuaNPCExports::showShop(lua_State *luaVm) {
	int shopid = lua_tointeger(luaVm, -1);
	Shops::showShop(getPlayer(luaVm), shopid);
	return 1;
}

int LuaNPCExports::deletePlayerVariable(lua_State *luaVm) {
	std::string key = string(lua_tostring(luaVm, -1));
	getPlayer(luaVm)->deleteVariable(key);
	return 1;
}

int LuaNPCExports::addQuest(lua_State *luaVm) {
	int questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->quests->addQuest(questid, getNPC(luaVm)->getNpcID());
	return 1;
}

int LuaNPCExports::endQuest(lua_State *luaVm) {
	int questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->quests->finishQuest(questid, getNPC(luaVm)->getNpcID());
	return 1;
}

int LuaNPCExports::end(lua_State *luaVm) {
	getNPC(luaVm)->end();
	return 1;
}
