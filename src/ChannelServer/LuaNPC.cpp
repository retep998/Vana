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
#include "Player.h"
#include "Players.h"

LuaNPC::LuaNPC(char *filename, NPC *npc) {
	lua_State *luaVm = lua_open();

	lua_pushinteger(luaVm, npc->getPlayer()->getPlayerid()); // Pushing id for reference from static functions
	lua_setglobal(luaVm, "playerid");
	lua_pushinteger(luaVm, npc->getState();
	lua_setglobal(luaVm, "state");

	lua_register(luaVm, "addText", &LuaNPCExports::addText);
	lua_register(luaVm, "sendSimple", &LuaNPCExports::sendSimple);
	lua_register(luaVm, "endNPC", &LuaNPCExports::end); // end() doesn't work (reserved?)

	luaL_dofile(luaVm, filename);
	lua_close(luaVm);
}

NPC * LuaNPCExports::getNPC(lua_State *luaVm) {
	lua_getglobal(luaVm, "playerid");
	return Players::players[lua_tointeger(luaVm, -1)]->getNPC();
}

int LuaNPCExports::addText(lua_State *luaVm) {
	getNPC(luaVm)->addText(lua_tostring(luaVm, -1));
	return 1;
}

int LuaNPCExports::sendSimple(lua_State *luaVm) {
	getNPC(luaVm)->sendSimple();
	return 1;
}

int LuaNPCExports::end(lua_State *luaVm) {
	getNPC(luaVm)->end();
	return 1;
}