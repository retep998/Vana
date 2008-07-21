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
#ifndef LUANPC_H
#define LUANPC_H

#include "LuaScriptable.h"
#include <hash_map>
#include <string>

using stdext::hash_map;
using std::string;

class NPC;

class LuaNPC : public LuaScriptable {
public:
	LuaNPC(const string &filename, int playerid);
};

namespace LuaExports {
	NPC * getNPC(lua_State *luaVm);

	// The exports
	int addText(lua_State *luaVm);
	int addChar(lua_State *luaVm);

	int sendSimple(lua_State *luaVm);
	int sendYesNo(lua_State *luaVm);
	int sendNext(lua_State *luaVm);
	int sendBackNext(lua_State *luaVm);
	int sendBackOK(lua_State *luaVm);
	int sendOK(lua_State *luaVm);
	int sendAcceptDecline(lua_State *luaVm);
	int sendGetText(lua_State *luaVm);
	int sendGetNumber(lua_State *luaVm);
	int sendStyle(lua_State *luaVm);

	int giveItem(lua_State *luaVm);
	int giveMesos(lua_State *luaVm);
	int giveEXP(lua_State *luaVm);
	int giveSP(lua_State *luaVm);
	int giveAP(lua_State *luaVm);

	int getSelected(lua_State *luaVm);
	int getNumber(lua_State *luaVm);
	int getText(lua_State *luaVm);
	int getVariable(lua_State *luaVm);
	int getMaxSkillLevel(lua_State *luaVm);

	int setState(lua_State *luaVm);
	int setVariable(lua_State *luaVm);
	int setMaxSkillLevel(lua_State *luaVm);

	int addQuest(lua_State *luaVm);
	int endQuest(lua_State *luaVm);

	int endNPC(lua_State *luaVm);

	int restart(lua_State *luaVm);
};

#endif
