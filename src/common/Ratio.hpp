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

#include "common/Types.hpp"

namespace vana {
	class ratio {
	public:
		ratio(game_coord top, game_coord bottom);
		auto is_unit() const -> bool;
		auto is_zero() const -> bool;
		auto is_defined() const -> bool;
		auto is_negative() const -> bool;

		auto top() const -> game_coord;
		auto bottom() const -> game_coord;
		auto reciprocal() const -> ratio;
		auto invert_sign() const -> ratio;
		auto negative_reciprocal() const -> ratio;
	private:
		game_coord m_top = 0;
		game_coord m_bottom = 0;
		bool m_is_defined = false;
		bool m_is_negative = false;
	};
}