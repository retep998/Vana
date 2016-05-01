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

#include "Common/IPacket.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Types.hpp"
#include <cmath>
#include <ostream>

namespace Vana {
	// IMPORTANT
	// The assumption made in the Point, Line, and Rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
	// That is, -1000 x is on the left side, 1000 x is on the right side
	// However, -1000 y is on the top and 1000 y is on the bottom
	// Be VERY careful when dealing with things that require the Y position
	struct Point {
		Point(coord_t x, coord_t y);
		Point() = default;

		auto isOrigin() const -> bool;
		auto move(coord_t x, coord_t y) const -> Point;
		auto moveX(coord_t x) const -> Point;
		auto moveY(coord_t y) const -> Point;

		auto operator-(const Point &p) const -> int32_t {
			return static_cast<int32_t>(std::sqrt(std::pow(static_cast<float>(x - p.x), 2) + std::pow(static_cast<float>(y - p.y), 2)));
		}

		coord_t x = 0;
		coord_t y = 0;
		friend auto operator <<(std::ostream &out, const Point &pos) -> std::ostream &;
	};

	template <>
	struct PacketSerialize<Point> {
		auto read(PacketReader &reader) -> Point {
			Point ret;
			ret.x = reader.get<coord_t>();
			ret.y = reader.get<coord_t>();
			return ret;
		}
		auto write(PacketBuilder &builder, const Point &obj) -> void {
			builder.add<coord_t>(obj.x);
			builder.add<coord_t>(obj.y);
		}
	};

	inline
	auto operator <<(std::ostream &out, const Point &pos) -> std::ostream & {
		return out << "{" << pos.x << ", " << pos.y << "}";
	}
}