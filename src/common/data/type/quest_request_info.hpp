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
	namespace data {
		namespace type {
			struct quest_request_info {
				bool is_quest = false;
				bool is_mob = false;
				bool is_job = false;
				bool is_item = false;
				int32_t id = -1;
				int32_t count = -1;
				int8_t quest_state = -1;
			};
		}
	}
}