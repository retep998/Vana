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

#include "common/enum_base.hpp"
#include "common/types_temp.hpp"

#define ENUM(type, make) \
	make(type, none, 0) \
	make(type, skill, 1) \
	make(type, item, 2) \
	make(type, face, 3) \
	make(type, menu, 4) \
	make(type, basic_action, 5) \
	make(type, basic_face, 6) \
	make(type, effect, 7)

namespace vana {
	ENUM_CLASS(key_map_type, int8_t, ENUM)
}

ENUM_CLASS_CAST(vana::key_map_type, int8_t, ENUM)

#undef ENUM