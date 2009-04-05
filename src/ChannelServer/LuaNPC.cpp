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
#include "LuaNPC.h"
#include "NPCs.h"
#include "Player.h"
#include "Players.h"
#include "Quests.h"
#include "StoragePacket.h"
#include <vector>

using std::vector;

LuaNPC::LuaNPC(const string &filename, int32_t playerid) : LuaScriptable(filename, playerid) {
	// Miscellaneous
	lua_register(luaVm, "showStorage", &LuaExports::showStorage);

	// NPC interaction
	lua_register(luaVm, "addText", &LuaExports::addText);
	lua_register(luaVm, "endNPC", &LuaExports::endNPC); // end() is reserved.
	lua_register(luaVm, "getNumber", &LuaExports::getNumber);
	lua_register(luaVm, "getSelected", &LuaExports::getSelected);
	lua_register(luaVm, "getText", &LuaExports::getText);
	lua_register(luaVm, "restart", &LuaExports::restart);
	lua_register(luaVm, "sendAcceptDecline", &LuaExports::sendAcceptDecline);
	lua_register(luaVm, "sendBackNext", &LuaExports::sendBackNext);
	lua_register(luaVm, "sendBackOK", &LuaExports::sendBackOK);
	lua_register(luaVm, "sendGetNumber", &LuaExports::sendGetNumber);
	lua_register(luaVm, "sendGetText", &LuaExports::sendGetText);
	lua_register(luaVm, "sendNext", &LuaExports::sendNext);
	lua_register(luaVm, "sendOK", &LuaExports::sendOK);
	lua_register(luaVm, "sendSimple", &LuaExports::sendSimple);
	lua_register(luaVm, "sendStyle", &LuaExports::sendStyle);
	lua_register(luaVm, "sendYesNo", &LuaExports::sendYesNo);
	lua_register(luaVm, "setState", &LuaExports::setState);

	// Quest
	lua_register(luaVm, "addQuest", &LuaExports::addQuest);
	lua_register(luaVm, "endQuest", &LuaExports::endQuest);

	// Skill
	lua_register(luaVm, "getMaxSkillLevel", &LuaExports::getMaxSkillLevel);
	lua_register(luaVm, "setMaxSkillLevel", &LuaExports::setMaxSkillLevel);
}

bool LuaNPC::run() {
	bool ret = LuaScriptable::run();

	if (!ret) {
		// Error in NPC script
		Players::Instance()->getPlayer(playerid)->getNPC()->end();
	}

	return ret;
}

NPC * LuaExports::getNPC(lua_State *luaVm) {
	return getPlayer(luaVm)->getNPC();
}

// Miscellaneous
int LuaExports::showStorage(lua_State *luaVm) {
	StoragePacket::showStorage(getPlayer(luaVm), getNPC(luaVm)->getNpcID());
	return 0;
}

// NPC interaction
int LuaExports::addText(lua_State *luaVm) {
	getNPC(luaVm)->addText(lua_tostring(luaVm, -1));
	return 0;
}

int LuaExports::endNPC(lua_State *luaVm) {
	getNPC(luaVm)->end();
	return 0;
}

int LuaExports::getNumber(lua_State *luaVm) {
	lua_pushnumber(luaVm, getNPC(luaVm)->getNumber());
	return 1;
}

int LuaExports::getSelected(lua_State *luaVm) {
	lua_pushnumber(luaVm, getNPC(luaVm)->getSelected());
	return 1;
}

int LuaExports::getText(lua_State *luaVm) {
	lua_pushstring(luaVm, getNPC(luaVm)->getText().c_str());
	return 1;
}

int LuaExports::restart(lua_State *luaVm) {
	getNPC(luaVm)->run();
	return 0;
}

int LuaExports::sendAcceptDecline(lua_State *luaVm) {
	getNPC(luaVm)->sendAcceptDecline();
	return 0;
}

int LuaExports::sendBackNext(lua_State *luaVm) {
	getNPC(luaVm)->sendBackNext();
	return 0;
}

int LuaExports::sendBackOK(lua_State *luaVm) {
	getNPC(luaVm)->sendBackOK();
	return 0;
}

int LuaExports::sendGetNumber(lua_State *luaVm) {
	getNPC(luaVm)->sendGetNumber(lua_tointeger(luaVm, -3), lua_tointeger(luaVm, -2), lua_tointeger(luaVm, -1));
	return 0;
}

int LuaExports::sendGetText(lua_State *luaVm) {
	getNPC(luaVm)->sendGetText();
	return 0;
}

int LuaExports::sendNext(lua_State *luaVm) {
	getNPC(luaVm)->sendNext();
	return 0;
}

int LuaExports::sendOK(lua_State *luaVm) {
	getNPC(luaVm)->sendOK();
	return 0;
}

int LuaExports::sendSimple(lua_State *luaVm) {
	getNPC(luaVm)->sendSimple();
	return 0;
}

int LuaExports::sendStyle(lua_State *luaVm) {
	vector<int32_t> styles;

	lua_pushnil(luaVm);
	while (lua_next(luaVm, 1) != 0) {
		styles.push_back(lua_tointeger(luaVm, -1));
		lua_pop(luaVm, 1);
	}

	getNPC(luaVm)->sendStyle(&styles[0], styles.size());
	return 0;
}

int LuaExports::sendYesNo(lua_State *luaVm) {
	getNPC(luaVm)->sendYesNo();
	return 0;
}

int LuaExports::setState(lua_State *luaVm) {
	getNPC(luaVm)->setState(lua_tointeger(luaVm, -1));
	return 0;
}

// Quest
int LuaExports::addQuest(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->addQuest(questid, getNPC(luaVm)->getNpcID());
	return 0;
}

int LuaExports::endQuest(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->finishQuest(questid, getNPC(luaVm)->getNpcID());
	return 0;
}

// Skill
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