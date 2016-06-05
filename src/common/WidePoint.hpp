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

#include "common/IPacket.hpp"
#include "common/PacketBuilder.hpp"
#include "common/PacketReader.hpp"
#include "common/Point.hpp"
#include "common/Types.hpp"
#include <cmath>

namespace vana {
	// IMPORTANT
	// The assumption made in the point, line, and rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
	// That is, -1000 x is on the left side, 1000 x is on the right side
	// However, -1000 y is on the top and 1000 y is on the bottom
	// Be VERY careful when dealing with things that require the Y position
	struct wide_point {
		wide_point(const point &pos) : x{pos.x}, y{pos.y} { }
		wide_point(int32_t x, int32_t y) : x{x}, y{y} { }
		wide_point() = default;

		auto operator-(const wide_point &p) const -> int32_t {
			return static_cast<int32_t>(std::sqrt(std::pow(static_cast<float>(x - p.x), 2) + std::pow(static_cast<float>(y - p.y), 2)));
		}

		int32_t x = 0;
		int32_t y = 0;
		friend auto operator <<(std::ostream &out, const wide_point &pos) -> std::ostream &;
	};

	template <>
	struct packet_serialize<wide_point> {
		auto read(packet_reader &reader) -> wide_point {
			wide_point ret;
			ret.x = reader.get<int32_t>();
			ret.y = reader.get<int32_t>();
			return ret;
		}
		auto write(packet_builder &builder, const wide_point &obj) -> void {
			builder.add<int32_t>(obj.x);
			builder.add<int32_t>(obj.y);
		}
	};

	inline
	auto operator <<(std::ostream &out, const wide_point &pos) -> std::ostream & {
		return out << "{" << pos.x << ", " << pos.y << "}";
	}
}