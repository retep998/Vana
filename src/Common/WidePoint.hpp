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

#include "IPacket.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Point.hpp"
#include "Types.hpp"
#include <cmath>

// IMPORTANT
// The assumption made in the Point, Line, and Rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
// That is, -1000 x is on the left side, 1000 x is on the right side
// However, -1000 y is on the top and 1000 y is on the bottom
// Be VERY careful when dealing with things that require the Y position
struct WidePoint {
	WidePoint(const Point &pos) : x{pos.x}, y{pos.y} { }
	WidePoint(int32_t x, int32_t y) : x{x}, y{y} { }
	WidePoint() = default;

	auto operator-(const WidePoint &p) const -> int32_t {
		return static_cast<int32_t>(sqrt(pow(static_cast<float>(x - p.x), 2) + pow(static_cast<float>(y - p.y), 2)));
	}

	int32_t x = 0;
	int32_t y = 0;
	friend auto operator <<(std::ostream &out, const WidePoint &pos) -> std::ostream &;
};

template <>
struct PacketSerialize<WidePoint> {
	auto read(PacketReader &reader) -> WidePoint {
		WidePoint ret;
		ret.x = reader.get<int32_t>();
		ret.y = reader.get<int32_t>();
		return ret;
	}
	auto write(PacketBuilder &builder, const WidePoint &obj) -> void {
		builder.add<int32_t>(obj.x);
		builder.add<int32_t>(obj.y);
	}
};

inline
auto operator <<(std::ostream &out, const WidePoint &pos) -> std::ostream & {
	return out << "{" << pos.x << ", " << pos.y << "}";
}