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

#include "Common/PacketBuilder.hpp"
#include "Common/Types.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		class player;

		namespace packets {
			namespace gm {
				namespace hired_merchant_modes {
					enum {
						map = 0x00,
						channel = 0x01,
					};
				}

				PACKET(begin_hide);
				PACKET(end_hide);
				PACKET(warning, bool succeed);
				PACKET(block);
				PACKET(invalid_character_name);
				PACKET(hired_merchant_place, int8_t mode, int32_t id);
				PACKET(set_get_var_result, const string &name, const string &variable, const string &value);
			}
		}
	}
}