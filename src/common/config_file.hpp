/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "common/lua_environment.hpp"
#include "common/types_temp.hpp"
#include <string>
#include <vector>

namespace vana {
	DEFAULT_EXCEPTION(config_exception, std::runtime_error);

	class config_file : public lua_environment {
		NONCOPYABLE(config_file);
		NO_DEFAULT_CONSTRUCTOR(config_file);
	public:
		config_file(const string &filename);
		~config_file();
		auto static get_salting_config() -> owned_ptr<config_file>;
		auto static get_worlds_config() -> owned_ptr<config_file>;
		auto static get_login_config() -> owned_ptr<config_file>;
		auto static get_logger_config() -> owned_ptr<config_file>;
		auto static get_database_config() -> owned_ptr<config_file>;
		auto static get_connection_properties_config() -> owned_ptr<config_file>;
	protected:
		auto handle_error(const string &filename, const string &error) -> void override;
		auto handle_key_not_found(const string &filename, const string &key) -> void override;
		auto handle_file_not_found(const string &filename) -> void override;
	};
}