#include "Config.h"

Config::Config(char *filename) {
	luaVm = lua_open();
	luaopen_base(luaVm);

	luaL_dofile(luaVm, filename);
}

int Config::getInt(char *value) {
	lua_getglobal(luaVm, value);
	return lua_tointeger(luaVm, -1);
}

char * Config::getString(char *value) {
	lua_getglobal(luaVm, value);
	return (char *) lua_tostring(luaVm, -1);
}

bool Config::getBool(char *value) {
	lua_getglobal(luaVm, value);
	return (lua_toboolean(luaVm, -1) != 0);
}
