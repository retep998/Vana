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

namespace vana {
	namespace login_server {
		namespace player_status {
			enum player_status : int8_t {
				logged_in = 0x00,
				set_pin = 0x01,
				check_pin = 0x04,
				set_gender = 0x0A,
				pin_select = 0x0B,
				ask_pin,
				not_logged_in,
			};
		}
	}
}