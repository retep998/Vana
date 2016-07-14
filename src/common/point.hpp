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

#include "common/i_packet.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/types.hpp"
#include <cmath>
#include <ostream>

namespace vana {
	// IMPORTANT
	// The assumption made in the point, line, and rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
	// That is, -1000 x is on the left side, 1000 x is on the right side
	// However, -1000 y is on the top and 1000 y is on the bottom
	// Be VERY careful when dealing with things that require the Y position
	struct point {
		point(game_coord x, game_coord y);
		point() = default;

		auto is_origin() const -> bool;
		auto invert() const -> point;
		auto negate() const -> point;
		auto negate_x() const -> point;
		auto negate_y() const -> point;
		auto move(game_coord x, game_coord y) const -> point;
		auto move_x(game_coord x) const -> point;
		auto move_y(game_coord y) const -> point;
		auto distance(const point &other) const -> int32_t;

		auto operator-(const point &p) const -> int32_t { return distance(p); }

		game_coord x = 0;
		game_coord y = 0;
		friend auto operator <<(std::ostream &out, const point &pos) -> std::ostream &;
	};

	template <>
	struct packet_serialize<point> {
		auto read(packet_reader &reader) -> point {
			point ret;
			ret.x = reader.get<game_coord>();
			ret.y = reader.get<game_coord>();
			return ret;
		}
		auto write(packet_builder &builder, const point &obj) -> void {
			builder.add<game_coord>(obj.x);
			builder.add<game_coord>(obj.y);
		}
	};

	inline
	auto operator <<(std::ostream &out, const point &pos) -> std::ostream & {
		return out << "{" << pos.x << ", " << pos.y << "}";
	}
}