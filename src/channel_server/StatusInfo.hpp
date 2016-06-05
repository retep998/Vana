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

#include "common/types_temp.hpp"

namespace vana {
	namespace channel_server {
		struct status_info {
			// TODO FIXME it appears that a review of reflection/skill ID are needed - some had 0, some had -1 for both
			status_info() = default;
			status_info(int32_t status, int32_t val, int32_t skill_id, seconds time);
			status_info(int32_t status, int32_t val, int16_t mob_skill, int16_t level, seconds time);
			status_info(int32_t status, int32_t val, int16_t mob_skill, int16_t level, int32_t reflect, seconds time);

			int16_t mob_skill = 0;
			int16_t level = 0;
			int32_t status = 0;
			int32_t skill_id = 0;
			int32_t reflection = -1;
			int32_t val = 0;
			seconds time;
		};
	}
}