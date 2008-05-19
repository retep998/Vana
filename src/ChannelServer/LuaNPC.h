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

extern "C" {
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}

#include "NPCs.h"

class LuaNPC {
public:
	LuaNPC(char *filename, NPC *npc);
};

namespace LuaNPCExports {
	NPC * getNPC(lua_State *luaVm);

	// The exports
	int addText(lua_State *luaVm);
	int sendSimple(lua_State *luaVm);
	int end(lua_State *luaVm);
};

#endif