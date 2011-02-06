/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "LuaInstance.h"
#include "Instance.h"
#include "Instances.h"
#include "Player.h"
#include "PlayerDataProvider.h"

LuaInstance::LuaInstance(const string &name, int32_t playerid) : LuaScriptable("scripts/instances/" + name + ".lua", playerid) {
	initialize();
	setVariable("_instancename", name);

	lua_register(luaVm, "createInstance", &LuaExports::createInstanceInstance);

	LuaScriptable::run(); // Running is loading the functions
}

bool LuaInstance::run(InstanceMessages message) {
	switch (message) {
		case BeginInstance:
			lua_getglobal(luaVm, "beginInstance");
			break;
	}
	if (lua_pcall(luaVm, 0, 0, 0)) {
		handleError();
		return false;
	}
	return true;
}

bool LuaInstance::run(InstanceMessages message, int32_t parameter) {
	switch (message) {
		case PlayerDeath:
			lua_getglobal(luaVm, "playerDeath");
			lua_pushinteger(luaVm, parameter);
			break;
		case InstanceTimerEnd:
		case InstanceTimerNaturalEnd:
			lua_getglobal(luaVm, "instanceTimerEnd");
			lua_pushboolean(luaVm, (parameter != 0));
			break;
		case PartyDisband:
			lua_getglobal(luaVm, "partyDisband");
			lua_pushinteger(luaVm, parameter);
			break;
	}
	if (lua_pcall(luaVm, 1, 0, 0)) {
		handleError();
		return false;
	}
	return true;
}

bool LuaInstance::run(InstanceMessages message, const string &parameter1, int32_t parameter2) {
	switch (message) {
		case TimerEnd:
		case TimerNaturalEnd:
			lua_getglobal(luaVm, "timerEnd");
			lua_pushstring(luaVm, parameter1.c_str());
			lua_pushboolean(luaVm, (parameter2 != 0));
			break;
	}
	if (lua_pcall(luaVm, 2, 0, 0)) {
		handleError();
		return false;
	}
	return true;
}

bool LuaInstance::run(InstanceMessages message, int32_t parameter1, int32_t parameter2) {
	switch (message) {
		case PlayerDisconnect:
			lua_getglobal(luaVm, "playerDisconnect");
			lua_pushinteger(luaVm, parameter1);
			lua_pushboolean(luaVm, parameter2 != 0);
			break;
		case PartyRemoveMember:
			lua_getglobal(luaVm, "partyRemoveMember");
			lua_pushinteger(luaVm, parameter1);
			lua_pushinteger(luaVm, parameter2);
			break;
	}

	if (lua_pcall(luaVm, 2, 0, 0)) {
		handleError();
		return false;
	}
	return true;
}

bool LuaInstance::run(InstanceMessages message, int32_t parameter1, int32_t parameter2, int32_t parameter3) {
	switch (message) {
		case MobDeath:
			lua_getglobal(luaVm, "mobDeath");
			break;
		case MobSpawn:
			lua_getglobal(luaVm, "mobSpawn");
			break;
	}
	lua_pushinteger(luaVm, parameter1);
	lua_pushinteger(luaVm, parameter2);
	lua_pushinteger(luaVm, parameter3);
	if (lua_pcall(luaVm, 3, 0, 0)) {
		handleError();
		return false;
	}
	return true;
}

bool LuaInstance::run(InstanceMessages message, int32_t parameter1, int32_t parameter2, int32_t parameter3, int32_t parameter4) {
	switch (message) {
		case PlayerChangeMap:
			lua_getglobal(luaVm, "changeMap");
			lua_pushinteger(luaVm, parameter1);
			lua_pushinteger(luaVm, parameter2);
			lua_pushinteger(luaVm, parameter3);
			lua_pushboolean(luaVm, parameter4 != 0);
			break;
	}
	if (lua_pcall(luaVm, 4, 0, 0)) {
		handleError();
		return false;
	}
	return true;
}

bool LuaInstance::run(InstanceMessages message, int32_t parameter1, int32_t parameter2, int32_t parameter3, int32_t parameter4, int32_t parameter5) {
	switch (message) {
		case FriendlyMobHit:
			lua_getglobal(luaVm, "friendlyHit");
			lua_pushinteger(luaVm, parameter1);
			lua_pushinteger(luaVm, parameter2);
			lua_pushinteger(luaVm, parameter3);
			lua_pushboolean(luaVm, parameter4);
			lua_pushboolean(luaVm, parameter5);
			break;
	}
	if (lua_pcall(luaVm, 5, 0, 0)) {
		handleError();
		return false;
	}
	return true;
}

int LuaExports::createInstanceInstance(lua_State *luaVm) {
	string name = lua_tostring(luaVm, 1);
	int32_t time = lua_tointeger(luaVm, 2);
	bool showtimer = lua_toboolean(luaVm, 3) != 0;
	int32_t persistent = 0;
	if (lua_isnumber(luaVm, 4)) {
		persistent = lua_tointeger(luaVm, 4);
	}
	Instance *instance = new Instance(name, 0, 0, time, persistent, showtimer);
	Instances::InstancePtr()->addInstance(instance);
	instance->sendMessage(BeginInstance);

	if (instance->showTimer())
		instance->showTimer(true, true);

	return 0;
}
