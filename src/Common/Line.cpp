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
#include "Line.hpp"
#include "Types.hpp"
#include <cmath>

Line::Line(const Point &pt1, const Point &pt2) :
	pt1{pt1},
	pt2{pt2}
{
}

auto Line::slope() const -> Ratio {
	coord_t rise = pt2.y - pt1.y;
	coord_t run = pt2.x - pt1.x;
	return Ratio{rise, run};
}

auto Line::contains(const Point &pos) const -> bool {
	return slopeContains(pos) && (pt1.x != pt2.x ?
		withinRangeX(pos.x) :
		withinRangeY(pos.y));
}

auto Line::slopeContains(const Point &pos) const -> bool {
	// Tests for colinearity
	auto slope1 = static_cast<int32_t>(pt2.x - pt1.x) * static_cast<int32_t>(pos.y - pt1.y);
	auto slope2 = static_cast<int32_t>(pos.x - pt1.x) * static_cast<int32_t>(pt2.y - pt1.y);
	return slope1 == slope2;
}

auto Line::withinRangeX(coord_t xValue) const -> bool {
	return
		(pt1.x < xValue && xValue <= pt2.x) ||
		(pt2.x < xValue && xValue <= pt1.x);
}

auto Line::withinRangeY(coord_t yValue) const -> bool {
	return
		(pt1.y < yValue && yValue <= pt2.y) ||
		(pt2.y < yValue && yValue <= pt1.y);
}

auto Line::interpolateForX(coord_t yValue) const -> optional_t<coord_t> {
	auto lineSlope = slope();
	// Both x values are the same, there is no slope, we have no sensible interpolation for this particular value
	if (!lineSlope.isDefined()) {
		// Unless the y value happens to be within our range
		if (withinRangeY(yValue)) {
			return pt1.x;
		}
		return {};
	}
	int32_t difference = yValue - pt1.y;
	difference *= lineSlope.bottom();
	difference /= lineSlope.top();
	if (lineSlope.isNegative()) difference *= -1;
	return static_cast<coord_t>(difference + pt1.x);
}

auto Line::interpolateForY(coord_t xValue) const -> optional_t<coord_t> {
	auto lineSlope = slope();
	// Both y values are the same, there is no slope, we have no sensible interpolation for this particular value
	if (lineSlope.isZero()) {
		// Unless the x value happens to be within our range
		if (withinRangeX(xValue)) {
			return pt1.y;
		}
		return {};
	}
	int32_t difference = xValue - pt1.x;
	difference *= lineSlope.top();
	difference /= lineSlope.bottom();
	if (lineSlope.isNegative()) difference *= -1;
	return static_cast<coord_t>(difference + pt1.y);
}

auto Line::move(coord_t xOffset, coord_t yOffset) const -> Line {
	return Line{
		pt1.move(xOffset, yOffset),
		pt2.move(xOffset, yOffset)
	};
}

auto Line::moveX(coord_t xOffset) const -> Line {
	return Line{
		pt1.moveX(xOffset),
		pt2.moveX(xOffset)
	};
}

auto Line::moveY(coord_t yOffset) const -> Line {
	return Line{
		pt1.moveY(yOffset),
		pt2.moveY(yOffset)
	};
}

auto Line::center() const -> Point {
	return Point{
		(pt2.x + pt1.x) / 2,
		(pt2.y + pt1.y) / 2
	};;
}

auto Line::length() const -> int32_t {
	return std::abs(pt1 - pt2);
}

auto Line::makeRect() const -> Rect {
	return Rect{pt1, pt2}.normalize();
}

auto Line::isVertical() const -> bool {
	return pt1.x == pt2.x;
}

auto Line::isHorizontal() const -> bool {
	return pt1.y == pt2.y;
}

auto Line::isOrigin() const -> bool {
	return pt1.isOrigin() && pt2.isOrigin();
}

auto Line::isEdge(const Point &pt) -> bool {
	return
		(pt.x == pt1.x && pt.y == pt1.y) ||
		(pt.x == pt2.x && pt.y == pt2.y);
}

auto Line::isEdge(coord_t xValue, coord_t yValue) -> bool {
	return isEdge(Point{xValue, yValue});
}