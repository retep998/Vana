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

#include "Point.hpp"
#include "Ratio.hpp"
#include "Types.hpp"
#include <cmath>
#include <ostream>

namespace Vana {
	// IMPORTANT
	// The assumption made in the Point, Line, and Rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
	// That is, -1000 x is on the left side, 1000 x is on the right side
	// However, -1000 y is on the top and 1000 y is on the bottom
	// Be VERY careful when dealing with things that require the Y position
	struct Line {
		Line() = default;
		Line(const Point &pt1, const Point &pt2);

		auto slope() const -> Ratio;
		auto contains(const Point &pos) const -> bool;
		auto slopeContains(const Point &pos) const -> bool;
		auto withinRangeX(coord_t xValue) const -> bool;
		auto withinRangeY(coord_t yValue) const -> bool;
		auto interpolateForX(coord_t yValue) const -> optional_t<coord_t>;
		auto interpolateForY(coord_t xValue) const -> optional_t<coord_t>;
		auto move(coord_t xOffset, coord_t yOffset) const -> Line;
		auto moveX(coord_t xOffset) const -> Line;
		auto moveY(coord_t yOffset) const -> Line;
		auto center() const -> Point;
		auto length() const -> int32_t;
		auto makeRect() const -> Rect;
		auto isVertical() const -> bool;
		auto isHorizontal() const -> bool;
		auto isOrigin() const -> bool;
		auto isEdge(const Point &pt) -> bool;
		auto isEdge(coord_t xValue, coord_t yValue) -> bool;

		Point pt1;
		Point pt2;

		friend auto operator <<(std::ostream &out, const Line &line) -> std::ostream &;
	};

	inline
	auto operator <<(std::ostream &out, const Line &line) -> std::ostream & {
		return out << "(" << line.pt1 << ", " << line.pt2 << ")";
	}
}