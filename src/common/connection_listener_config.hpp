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
#include "common/connection_type.hpp"
#include "common/ip.hpp"
#include "common/types.hpp"
#include <string>

namespace vana {
	struct connection_listener_config {
		config::ping ping;
		bool encrypt;
		connection_type type;
		string subversion;
		connection_port port;
		ip::type ip_type;

		connection_listener_config(
			const config::ping &ping,
			bool encrypt,
			connection_type type,
			string subversion,
			connection_port port,
			ip::type ip_type) :
			ping{ping},
			encrypt{encrypt},
			type{type},
			subversion{subversion},
			port{port},
			ip_type{ip_type}
		{
		}
	};
}