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
#pragma once

#include "LuaEnvironment.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

DEFAULT_EXCEPTION(ConfigException, std::runtime_error);

class ConfigFile : public LuaEnvironment {
	NONCOPYABLE(ConfigFile);
	NO_DEFAULT_CONSTRUCTOR(ConfigFile);
public:
	ConfigFile(const string_t &filename);
	~ConfigFile();
	auto static getSaltingConfig() -> owned_ptr_t<ConfigFile>;
	auto static getWorldsConfig() -> owned_ptr_t<ConfigFile>;
	auto static getLoginServerConfig() -> owned_ptr_t<ConfigFile>;
	auto static getLoggerConfig() -> owned_ptr_t<ConfigFile>;
	auto static getDatabaseConfig() -> owned_ptr_t<ConfigFile>;
	auto static getConnectionPropertiesConfig() -> owned_ptr_t<ConfigFile>;
protected:
	auto handleError(const string_t &filename, const string_t &error) -> void override;
	auto handleKeyNotFound(const string_t &filename, const string_t &key) -> void override;
	auto handleFileNotFound(const string_t &filename) -> void override;
};