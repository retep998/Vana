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
	initialize();
	setVariable("system_instanceName", name);

	expose("createInstance", &LuaExports::createInstanceInstance);

	LuaScriptable::run(); // Running is loading the functions
}

auto LuaInstance::run(InstanceMessage message) -> Result {
	switch (message) {
		case InstanceMessage::BeginInstance:
			lua_getglobal(m_luaVm, "beginInstance");
			break;
	}
	if (lua_pcall(m_luaVm, 0, 0, 0)) {
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaInstance::run(InstanceMessage message, int32_t parameter) -> Result {
	switch (message) {
		case InstanceMessage::PlayerDeath:
			lua_getglobal(m_luaVm, "playerDeath");
			lua_pushinteger(m_luaVm, parameter);
			break;
		case InstanceMessage::InstanceTimerEnd:
		case InstanceMessage::InstanceTimerNaturalEnd:
			lua_getglobal(m_luaVm, "instanceTimerEnd");
			lua_pushboolean(m_luaVm, parameter != 0);
			break;
		case InstanceMessage::PartyDisband:
			lua_getglobal(m_luaVm, "partyDisband");
			lua_pushinteger(m_luaVm, parameter);
			break;
	}
	if (lua_pcall(m_luaVm, 1, 0, 0)) {
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaInstance::run(InstanceMessage message, const string_t &parameter1, int32_t parameter2) -> Result {
	switch (message) {
		case InstanceMessage::TimerEnd:
		case InstanceMessage::TimerNaturalEnd:
			lua_getglobal(m_luaVm, "timerEnd");
			lua_pushstring(m_luaVm, parameter1.c_str());
			lua_pushboolean(m_luaVm, parameter2 != 0);
			break;
	}
	if (lua_pcall(m_luaVm, 2, 0, 0)) {
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaInstance::run(InstanceMessage message, int32_t parameter1, int32_t parameter2) -> Result {
	switch (message) {
		case InstanceMessage::PlayerDisconnect:
			lua_getglobal(m_luaVm, "playerDisconnect");
			lua_pushinteger(m_luaVm, parameter1);
			lua_pushboolean(m_luaVm, parameter2 != 0);
			break;
		case InstanceMessage::PartyRemoveMember:
			lua_getglobal(m_luaVm, "partyRemoveMember");
			lua_pushinteger(m_luaVm, parameter1);
			lua_pushinteger(m_luaVm, parameter2);
			break;
	}

	if (lua_pcall(m_luaVm, 2, 0, 0)) {
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaInstance::run(InstanceMessage message, int32_t parameter1, int32_t parameter2, int32_t parameter3) -> Result {
	switch (message) {
		case InstanceMessage::MobDeath:
			lua_getglobal(m_luaVm, "mobDeath");
			break;
		case InstanceMessage::MobSpawn:
			lua_getglobal(m_luaVm, "mobSpawn");
			break;
	}
	lua_pushinteger(m_luaVm, parameter1);
	lua_pushinteger(m_luaVm, parameter2);
	lua_pushinteger(m_luaVm, parameter3);
	if (lua_pcall(m_luaVm, 3, 0, 0)) {
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaInstance::run(InstanceMessage message, int32_t parameter1, int32_t parameter2, int32_t parameter3, int32_t parameter4) -> Result {
	switch (message) {
		case InstanceMessage::PlayerChangeMap:
			lua_getglobal(m_luaVm, "changeMap");
			lua_pushinteger(m_luaVm, parameter1);
			lua_pushinteger(m_luaVm, parameter2);
			lua_pushinteger(m_luaVm, parameter3);
			lua_pushboolean(m_luaVm, parameter4 != 0);
			break;
	}
	if (lua_pcall(m_luaVm, 4, 0, 0)) {
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaInstance::run(InstanceMessage message, int32_t parameter1, int32_t parameter2, int32_t parameter3, int32_t parameter4, int32_t parameter5) -> Result {
	switch (message) {
		case InstanceMessage::FriendlyMobHit:
			lua_getglobal(m_luaVm, "friendlyHit");
			lua_pushinteger(m_luaVm, parameter1);
			lua_pushinteger(m_luaVm, parameter2);
			lua_pushinteger(m_luaVm, parameter3);
			lua_pushboolean(m_luaVm, parameter4);
			lua_pushboolean(m_luaVm, parameter5);
			break;
	}
	if (lua_pcall(m_luaVm, 5, 0, 0)) {
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
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
	instance->sendMessage(InstanceMessage::BeginInstance);

	if (instance->showTimer()) {
		instance->showTimer(true, true);
	}

	return 0;
}