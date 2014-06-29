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
#pragma once

#include "Pos.hpp"
#include "Types.hpp"
#include <cmath>

// IMPORTANT
// The assumption made in the Line and Rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
// That is, -1000 x is on the left side, 1000 x is on the right side
// However, -1000 y is on the top and 1000 y is on the bottom
// Be VERY careful when dealing with things that require the Y position
struct Line {
	Line() = default;
	Line(const Pos &pt1, const Pos &pt2) : pt1(pt1), pt2(pt2) { }

	auto slope() const -> float;
	auto contains(const Pos &pos) const -> bool;
	auto slopeContains(const Pos &pos) const -> bool;
	auto withinRangeX(coord_t xValue) const -> bool;
	auto withinRangeY(coord_t yValue) const -> bool;
	auto interpolateForX(coord_t yValue) const -> coord_t;
	auto interpolateForY(coord_t xValue) const -> coord_t;
	auto length() const -> int32_t;
	auto makeRect() const -> Rect;
	auto isVertical() const -> bool;
	auto isHorizontal() const -> bool;
	auto isOrigin() const -> bool;
	auto isEdge(const Pos &pt) -> bool;
	auto isEdge(coord_t xValue, coord_t yValue) -> bool;

	Pos pt1;
	Pos pt2;
};