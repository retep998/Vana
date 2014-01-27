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
#pragma once

#include "LuaScriptable.h"
#include <string>
#include <unordered_map>

struct PortalInfo;

class LuaPortal : public LuaScriptable {
	NONCOPYABLE(LuaPortal);
	NO_DEFAULT_CONSTRUCTOR(LuaPortal);
public:
	LuaPortal(const string_t &filename, int32_t playerId, PortalInfo *portal);
};

namespace LuaExports {
	auto getPortal(lua_State *luaVm) -> PortalInfo *;
	extern hash_map_t<int32_t, PortalInfo *> portals;

	// Portal exports

	// Portal
	auto getPortalName(lua_State *luaVm) -> int;
	auto instantWarp(lua_State *luaVm) -> int;
	auto playPortalSe(lua_State *luaVm) -> int;
}