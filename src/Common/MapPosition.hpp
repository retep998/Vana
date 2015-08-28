/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Types.hpp"
#include <cmath>
#include <ostream>

namespace Vana {
	struct MapPosition {
		MapPosition(Point pt, foothold_id_t foothold) : pt{pt}, foothold{foothold} { }
		MapPosition() = default;

		Point pt;
		foothold_id_t foothold = 0;
		friend auto operator <<(std::ostream &out, const MapPosition &pos) -> std::ostream &;
	};

	inline
	auto operator <<(std::ostream &out, const MapPosition &pos) -> std::ostream & {
		return out << pos.pt << " [" << pos.foothold << "]";
	}
}