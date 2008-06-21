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
#ifndef LUASCRIPTABLE_H
#define LUASCRIPTABLE_H

extern "C" {
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}

#include <hash_map>
#include <string>

using stdext::hash_map;
using std::string;

class Player;

class LuaScriptable {
public:
	LuaScriptable(const string &filename, int playerid);
	virtual ~LuaScriptable();

	void initialize();
	void run();
protected:
	string filename;
	int playerid;
	lua_State *luaVm;
};

namespace LuaExports {
	Player * getPlayer(lua_State *luaVm);

	// The exports
	int addSkillLevel(lua_State *luaVm);

	int giveItem(lua_State *luaVm);
	int giveMesos(lua_State *luaVm);
	int giveEXP(lua_State *luaVm);
	int giveSP(lua_State *luaVm);
	int giveAP(lua_State *luaVm);

	int getSTR(lua_State *luaVm);
	int getDEX(lua_State *luaVm);
	int getINT(lua_State *luaVm);
	int getLUK(lua_State *luaVm);
	int getJob(lua_State *luaVm);
	int getLevel(lua_State *luaVm);
	int getGender(lua_State *luaVm);
	int getItemAmount(lua_State *luaVm);
	int getSkillLevel(lua_State *luaVm);
	int getMesos(lua_State *luaVm);
	int getMap(lua_State *luaVm);
	int getHP(lua_State *luaVm);
	int getMHP(lua_State *luaVm);
	int getRMHP(lua_State *luaVm);
	int getMP(lua_State *luaVm);
	int getMMP(lua_State *luaVm);
	int getRMMP(lua_State *luaVm);
	int getHair(lua_State *luaVm);
	int getEyes(lua_State *luaVm);
	int getPlayerVariable(lua_State *luaVm);
	int getNumPlayers(lua_State *luaVm);
	int getReactorState(lua_State *luaVm);
	int getRandomNumber(lua_State *luaVm);

	int killMob(lua_State *luaVm);

	int setStyle(lua_State *luaVm);
	int setMap(lua_State *luaVm);
	int setMusic(lua_State *luaVm);
	int setReactorsState(lua_State *luaVm);
	int setHP(lua_State *luaVm);
	int setMHP(lua_State *luaVm);
	int setRMHP(lua_State *luaVm);
	int setMP(lua_State *luaVm);
	int setMMP(lua_State *luaVm);
	int setRMMP(lua_State *luaVm);
	int setSTR(lua_State *luaVm);
	int setDEX(lua_State *luaVm);
	int setINT(lua_State *luaVm);
	int setLUK(lua_State *luaVm);
	int setJob(lua_State *luaVm);
	int setPlayerVariable(lua_State *luaVm);

	int showShop(lua_State *luaVm);
	int showMessage(lua_State *luaVm);
	int showMapMessage(lua_State *luaVm);

	int spawnMob(lua_State *luaVm);
	int spawnMobPos(lua_State *luvaVm);

	int deletePlayerVariable(lua_State *luaVm);
};

#endif