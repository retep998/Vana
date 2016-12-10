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

#include "common/types.hpp"

namespace vana {
	namespace log {
		enum class type {
			none = 0,
			info = 1, // Starting at 1 so it's easy enough to simply insert the logtype for SQL
			warning,
			debug,
			error,
			debug_error,
			critical_error,
			hacking,
			server_connect,
			server_disconnect,
			server_auth_failure,
			login,
			login_auth_failure,
			logout,
			client_error,
			gm_command,
			admin_command,
			boss_kill,
			trade,
			shop_transaction,
			storage_transaction,
			instance_begin,
			drop,
			chat,
			whisper,
			malformed_packet,
			script_log,
			ban,
			unban,
			// If any more are added, please add them to the implementation of get_level_string
		};
	}
}