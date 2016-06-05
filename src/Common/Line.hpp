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

#include "Common/Point.hpp"
#include "Common/Ratio.hpp"
#include "Common/Types.hpp"
#include <cmath>
#include <ostream>

namespace vana {
	// IMPORTANT
	// The assumption made in the point, line, and rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
	// That is, -1000 x is on the left side, 1000 x is on the right side
	// However, -1000 y is on the top and 1000 y is on the bottom
	// Be VERY careful when dealing with things that require the Y position
	struct line {
		line() = default;
		line(const point &pt1, const point &pt2);

		auto slope() const -> ratio;
		auto contains(const point &pos) const -> bool;
		auto slope_contains(const point &pos) const -> bool;
		auto within_range_x(game_coord value_x) const -> bool;
		auto within_range_y(game_coord value_y) const -> bool;
		auto interpolate_for_x(game_coord value_y) const -> optional<game_coord>;
		auto interpolate_for_y(game_coord value_x) const -> optional<game_coord>;
		auto move(game_coord offset_x, game_coord offset_y) const -> line;
		auto move_x(game_coord offset) const -> line;
		auto move_y(game_coord offset) const -> line;
		auto center() const -> point;
		auto length() const -> int32_t;
		auto make_rect() const -> rect;
		auto is_vertical() const -> bool;
		auto is_horizontal() const -> bool;
		auto is_origin() const -> bool;
		auto is_edge(const point &pt) -> bool;
		auto is_edge(game_coord value_x, game_coord value_y) -> bool;

		point pt1;
		point pt2;

		friend auto operator <<(std::ostream &out, const line &line) -> std::ostream &;
	};

	inline
	auto operator <<(std::ostream &out, const line &line) -> std::ostream & {
		return out << "(" << line.pt1 << ", " << line.pt2 << ")";
	}
}