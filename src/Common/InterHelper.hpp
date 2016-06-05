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

#include "Common/Types.hpp"

namespace vana {
	// Specific packet constants that are shared between servers
	namespace party_actions {
		enum party_actions : int8_t {
			create = 0x01,
			leave = 0x02,
			join = 0x03,
			invite = 0x04,
			expel = 0x05,
			set_leader = 0x06,
			silent_update,
			log_in_or_out,
		};
	}

	using protocol_sync = int8_t;
	using protocol_update_bits = uint16_t;
	namespace sync {
		namespace sync_types {
			enum sync_types : protocol_sync {
				channel_start,
				config,
				update,
				player,
				party,
				buddy,
				guild,
				guild_bbs,
				alliance,
			};
		}
		namespace config {
			enum sync_modes : protocol_sync {
				rate_set,
				rate_reset,
				scrolling_header,
			};
		}
		namespace player {
			enum sync_modes : protocol_sync {
				connect,
				disconnect,
				change_channel_request,
				change_channel_go,
				new_connectable,
				delete_connectable,
				update_player,
				character_created,
				character_deleted,
			};
			namespace update_bits {
				enum : protocol_update_bits {
					level = 0x01,
					job = 0x02,
					map = 0x04,
					channel = 0x08,
					ip = 0x10,
					cash = 0x20,
					mts = 0x40,
					transfer = 0x80,
					full = 0x8000,
				};
			}
		}
		namespace party {
			enum sync_modes : protocol_sync {
				disband,
				create,
				switch_leader,
				remove_member,
				add_member,
			};
		}
		namespace buddy {
			enum sync_modes : protocol_sync {
				invite,
				accept_invite,
				remove_buddy,
				readd_buddy,
			};
		}
	}
}