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

#include "common/packet_builder.hpp"
#include "common/types.hpp"

namespace vana {
	namespace channel_server {
		class player;

		namespace packets {
			namespace fame {
				namespace errors {
					enum errors : int8_t {
						incorrect_user = 0x01,
						level_under15 = 0x02,
						already_famed_today = 0x03,
						famed_this_month = 0x04
					};
				}

				PACKET(send_fame, const string &name, uint8_t type, int32_t new_fame);
				PACKET(receive_fame, const string &name, uint8_t type);
				PACKET(send_error, int32_t reason);
			}
		}
	}
}