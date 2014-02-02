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
#include "LuaInstance.hpp"
#include "Instance.hpp"
#include "Instances.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"

LuaInstance::LuaInstance(const string_t &name, int32_t playerId) :
	LuaScriptable("scripts/instances/" + name + ".lua", playerId)
{
	set<string_t>("system_instanceName", name);

	expose("createInstance", &LuaExports::createInstanceInstance);

	run(); // Running is loading the functions
}

auto LuaExports::createInstanceInstance(lua_State *luaVm) -> int {
	string_t name = lua_tostring(luaVm, 1);
	int32_t time = lua_tointeger(luaVm, 2);
	bool showTimer = lua_toboolean(luaVm, 3) != 0;
	int32_t persistent = 0;
	if (lua_isnumber(luaVm, 4)) {
		persistent = lua_tointeger(luaVm, 4);
	}

	Instance *instance = new Instance(name, 0, 0, seconds_t(time), seconds_t(persistent), showTimer);
	Instances::getInstance().addInstance(instance);
	instance->beginInstance();

	if (instance->showTimer()) {
		instance->showTimer(true, true);
	}

	return 0;
}