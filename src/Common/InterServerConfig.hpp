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

namespace Vana {
	struct InterServerConfig {
		InterServerConfig() :
			loginIp{0}
		{
		}

		bool clientEncryption = true;
		PingConfig clientPing;
		PingConfig serverPing;
		port_t loginPort = 0;
		Ip loginIp;
	};

	template <>
	struct LuaSerialize<InterServerConfig> {
		auto read(LuaEnvironment &config, const string_t &prefix) -> InterServerConfig {
			InterServerConfig ret;
			ret.clientEncryption = config.get<bool>("use_client_encryption");
			ret.clientPing = config.get<PingConfig>("client_ping");
			ret.serverPing = config.get<PingConfig>("inter_ping");
			ret.loginIp = Ip{Ip::stringToIpv4(config.get<string_t>("login_ip"))};
			ret.loginPort = config.get<port_t>("login_inter_port");
			return ret;
		}
	};
}