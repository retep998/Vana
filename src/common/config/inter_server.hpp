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

#include "common/config/ping.hpp"
#include "common/ip.hpp"
#include "common/lua/config_file.hpp"
#include "common/types.hpp"
#include <string>

namespace vana {
	namespace config {
		struct inter_server {
			inter_server() :
				login_ip{0}
			{
			}

			bool client_encryption = true;
			ping client_ping;
			ping server_ping;
			connection_port login_port = 0;
			ip login_ip;
		};
	}

	template <>
	struct lua::lua_serialize<config::inter_server> {
		auto read(lua_environment &config, const string &prefix) -> config::inter_server {
			config::inter_server ret;
			ret.client_encryption = config.get<bool>("use_client_encryption");
			ret.client_ping = config.get<config::ping>("client_ping");
			ret.server_ping = config.get<config::ping>("inter_ping");
			ret.login_ip = ip{ip::string_to_ipv4(config.get<string>("login_ip"))};
			ret.login_port = config.get<connection_port>("login_inter_port");
			return ret;
		}
	};
}