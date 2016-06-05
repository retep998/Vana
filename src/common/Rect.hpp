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

#include "common/Point.hpp"
#include "common/Types.hpp"
#include <algorithm>
#include <cmath>
#include <ostream>

namespace vana {
	struct line;

	// IMPORTANT
	// The assumption made in the point, line, and rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
	// That is, -1000 x is on the left side, 1000 x is on the right side
	// However, -1000 y is on the top and 1000 y is on the bottom
	// Be VERY careful when dealing with things that require the Y position
	class rect {
	public:
		rect() = default;
		rect(const point &left_top, const point &right_bottom);
		rect(const point &left_top, game_coord width, game_coord height);
		rect(game_coord left_top_x, game_coord left_top_y, game_coord width, game_coord height);

		auto left() const -> line;
		auto right() const -> line;
		auto top() const -> line;
		auto bottom() const -> line;
		auto diagonal_left_top_right_bottom() const -> line;
		auto diagonal_left_bottom_right_top() const -> line;
		auto right_top() const -> point;
		auto left_top() const -> point;
		auto left_bottom() const -> point;
		auto right_bottom() const -> point;
		auto center() const -> point;
		auto area() const -> int32_t;
		auto perimeter() const -> int32_t;
		auto hypotenuse() const -> int32_t;
		auto height() const -> int32_t;
		auto width() const -> int32_t;
		auto contains(const point &pos) const -> bool;
		auto contains_any_part_of_line(const line &line) const -> bool;
		auto contains_full_line(const line &line) const -> bool;
		auto intersects(const line &line) const -> bool;
		auto move(game_coord offset_x, game_coord offset_y) const -> rect;
		auto move(const point &pos) const -> rect;
		auto resize(game_coord mod) const -> rect;
		auto deform(game_coord left_top_mod, game_coord right_bottom_mod) const -> rect;
		auto swap(bool swap_horizontal, bool swap_vertical) const -> rect;
		auto normalize() const -> rect;
		auto intersection(const rect &other) const -> rect;
		auto combine(const rect &other) const -> rect;
		auto find_overlap(const rect &other) const -> search_result;

		friend auto operator <<(std::ostream &out, const rect &rect) -> std::ostream &;
	private:
		point m_left_top;
		point m_right_bottom;
	};

	inline
	auto operator <<(std::ostream &out, const rect &value) -> std::ostream & {
		rect normalized = value.normalize();
		return out << "(" << normalized.m_left_top << ", " << normalized.m_right_bottom << ")";
	}
}