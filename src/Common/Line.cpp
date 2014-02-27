/*
Copyright (C) 2008-2014 Vana Development Team

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

auto Line::slope() const -> float {
	return static_cast<float>(pt1.y - pt2.y) / static_cast<float>(pt1.x - pt2.x);
}

auto Line::contains(const Pos &pos) const -> bool {
	return slopeContains(pos) && (pt1.x != pt2.x ?
		withinRangeX(pos.x) :
		withinRangeY(pos.y));
}

auto Line::slopeContains(const Pos &pos) const -> bool {
	return static_cast<int32_t>(pt2.x - pt1.x) * static_cast<int32_t>(pos.y - pt1.y) == static_cast<int32_t>(pos.x - pt1.x) * static_cast<int32_t>(pt2.y - pt1.y);
}

auto Line::withinRangeX(coord_t xValue) const -> bool {
	return (pt1.x < xValue && xValue <= pt2.x) || (pt2.x < xValue && xValue <= pt1.x);
}

auto Line::withinRangeY(coord_t yValue) const -> bool {
	return (pt1.y < yValue && yValue <= pt2.y) || (pt2.y < yValue && yValue <= pt1.y);
}

auto Line::interpolateForX(coord_t yValue) const -> coord_t {
	return static_cast<coord_t>(slope() * (yValue - pt1.y) + pt1.x);
}

auto Line::interpolateForY(coord_t xValue) const -> coord_t {
	return static_cast<coord_t>(slope() * (xValue - pt1.x) + pt1.y);
}

auto Line::length() const -> int32_t {
	return abs(pt1 - pt2);
}

auto Line::makeRect() const -> Rect {
	return Rect(pt1, pt2).normalize();
}