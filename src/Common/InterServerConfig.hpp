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

#include "Common/ConfigFile.hpp"
#include "Common/Ip.hpp"
#include "Common/PingConfig.hpp"
#include "Common/Types.hpp"
#include <string>

namespace vana {
	struct inter_server_config {
		inter_server_config() :
			login_ip{0}
		{
		}

		bool client_encryption = true;
		ping_config client_ping;
		ping_config server_ping;
		connection_port login_port = 0;
		ip login_ip;
	};

	template <>
	struct lua_serialize<inter_server_config> {
		auto read(lua_environment &config, const string &prefix) -> inter_server_config {
			inter_server_config ret;
			ret.client_encryption = config.get<bool>("use_client_encryption");
			ret.client_ping = config.get<ping_config>("client_ping");
			ret.server_ping = config.get<ping_config>("inter_ping");
			ret.login_ip = ip{ip::string_to_ipv4(config.get<string>("login_ip"))};
			ret.login_port = config.get<connection_port>("login_inter_port");
			return ret;
		}
	};
}