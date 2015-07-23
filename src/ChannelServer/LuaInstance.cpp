/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ChannelServer.hpp"
#include "Instance.hpp"
#include "Instances.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"

LuaInstance::LuaInstance(const string_t &name, player_id_t playerId) :
	LuaScriptable{ChannelServer::getInstance().getScriptDataProvider().buildScriptPath(ScriptTypes::Instance, name), playerId}
{
	set<string_t>("system_instance_name", name);

	expose("createInstance", &LuaExports::createInstanceInstance);

	run(); // Running is loading the functions
}

auto LuaExports::createInstanceInstance(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t name = env.get<string_t>(luaVm, 1);
	int32_t time = env.get<int32_t>(luaVm, 2);
	bool showTimer = env.get<bool>(luaVm, 3);
	int32_t persistent = 0;
	if (env.is(luaVm, 4, LuaType::Number)) {
		persistent = env.get<int32_t>(luaVm, 4);
	}

	Instance *instance = new Instance(name, 0, 0, seconds_t{time}, seconds_t{persistent}, showTimer);
	ChannelServer::getInstance().getInstances().addInstance(instance);
	instance->beginInstance();

	if (instance->showTimer()) {
		instance->showTimer(true, true);
	}

	return 0;
}