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
	enum class buff_skill_value {
		predefined,
		// special_processing indicates an unusual value (e.g. combo orbs) or set of values
		special_processing,
		// special_packet buffs share the same packet for first person/third person views
		special_packet,
		bitpacked_xy_16,
		bitpacked_skill_and_level_32,
		skill_id,
		level,
		x,
		y,
		speed,
		jump,
		watk,
		wdef,
		matk,
		mdef,
		accuracy,
		avoid,
		prop,
		morph,
		gender_specific_morph,
		mob_count,
		range,
		damage,
		time,
	};
}