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
#include "Line.hpp"
#include <cmath>

namespace vana {

line::line(const point &pt1, const point &pt2) :
	pt1{pt1},
	pt2{pt2}
{
}

auto line::slope() const -> ratio {
	game_coord rise = pt2.y - pt1.y;
	game_coord run = pt2.x - pt1.x;
	return ratio{rise, run};
}

auto line::contains(const point &pos) const -> bool {
	return slope_contains(pos) && (pt1.x != pt2.x ?
		within_range_x(pos.x) :
		within_range_y(pos.y));
}

auto line::slope_contains(const point &pos) const -> bool {
	// Tests for colinearity
	auto slope1 = static_cast<int32_t>(pt2.x - pt1.x) * static_cast<int32_t>(pos.y - pt1.y);
	auto slope2 = static_cast<int32_t>(pos.x - pt1.x) * static_cast<int32_t>(pt2.y - pt1.y);
	return slope1 == slope2;
}

auto line::within_range_x(game_coord value_x) const -> bool {
	return
		(pt1.x < value_x && value_x <= pt2.x) ||
		(pt2.x < value_x && value_x <= pt1.x);
}

auto line::within_range_y(game_coord value_y) const -> bool {
	return
		(pt1.y < value_y && value_y <= pt2.y) ||
		(pt2.y < value_y && value_y <= pt1.y);
}

auto line::interpolate_for_x(game_coord value_y) const -> optional<game_coord> {
	auto line_slope = slope();
	// Both x values are the same, there is no slope, we have no sensible interpolation for this particular value
	if (!line_slope.is_defined()) {
		// Unless the y value happens to be within our range
		if (within_range_y(value_y)) {
			return pt1.x;
		}
		return {};
	}
	int32_t difference = value_y - pt1.y;
	difference *= line_slope.bottom();
	difference /= line_slope.top();
	if (line_slope.is_negative()) difference *= -1;
	return static_cast<game_coord>(difference + pt1.x);
}

auto line::interpolate_for_y(game_coord value_x) const -> optional<game_coord> {
	auto line_slope = slope();
	// Both y values are the same, there is no slope, we have no sensible interpolation for this particular value
	if (line_slope.is_zero()) {
		// Unless the x value happens to be within our range
		if (within_range_x(value_x)) {
			return pt1.y;
		}
		return {};
	}
	int32_t difference = value_x - pt1.x;
	difference *= line_slope.top();
	difference /= line_slope.bottom();
	if (line_slope.is_negative()) difference *= -1;
	return static_cast<game_coord>(difference + pt1.y);
}

auto line::move(game_coord offset_x, game_coord offset_y) const -> line {
	return line{
		pt1.move(offset_x, offset_y),
		pt2.move(offset_x, offset_y)
	};
}

auto line::move_x(game_coord offset) const -> line {
	return line{
		pt1.move_x(offset),
		pt2.move_x(offset)
	};
}

auto line::move_y(game_coord offset) const -> line {
	return line{
		pt1.move_y(offset),
		pt2.move_y(offset)
	};
}

auto line::center() const -> point {
	return point{
		(pt2.x + pt1.x) / 2,
		(pt2.y + pt1.y) / 2
	};;
}

auto line::length() const -> int32_t {
	return std::abs(pt1 - pt2);
}

auto line::make_rect() const -> rect {
	return rect{pt1, pt2}.normalize();
}

auto line::is_vertical() const -> bool {
	return pt1.x == pt2.x;
}

auto line::is_horizontal() const -> bool {
	return pt1.y == pt2.y;
}

auto line::is_origin() const -> bool {
	return pt1.is_origin() && pt2.is_origin();
}

auto line::is_edge(const point &pt) -> bool {
	return
		(pt.x == pt1.x && pt.y == pt1.y) ||
		(pt.x == pt2.x && pt.y == pt2.y);
}

auto line::is_edge(game_coord value_x, game_coord value_y) -> bool {
	return is_edge(point{value_x, value_y});
}

}