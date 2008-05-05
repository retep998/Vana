#ifndef CONFIG_H
#define CONFIG_H

extern "C" {
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}

class Config {
public:
	Config(char *filename);
	int getInt(char *value);
	char * getString(char *value);
	bool getBool(char *value);
private:
	lua_State *luaVm;
};

#endif