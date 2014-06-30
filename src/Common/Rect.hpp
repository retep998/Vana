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

#include "Point.hpp"
#include "Types.hpp"
#include <algorithm>
#include <cmath>
#include <ostream>

struct Line;

// IMPORTANT
// The assumption made in the Line and Rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
// That is, -1000 x is on the left side, 1000 x is on the right side
// However, -1000 y is on the top and 1000 y is on the bottom
// Be VERY careful when dealing with things that require the Y position
class Rect {
public:
	Rect() = default;
	Rect(const Point &leftTop, const Point &rightBottom);

	auto left() const -> Line;
	auto right() const -> Line;
	auto top() const -> Line;
	auto bottom() const -> Line;
	auto rightTop() const -> Point;
	auto leftTop() const -> Point;
	auto leftBottom() const -> Point;
	auto rightBottom() const -> Point;
	auto area() const -> int32_t;
	auto perimeter() const -> int32_t;
	auto hypotenuse() const -> int32_t;
	auto height() const -> int32_t;
	auto width() const -> int32_t;
	auto contains(const Point &pos) const -> bool;
	auto containsAnyPartOfLine(const Line &line) const -> bool;
	auto containsFullLine(const Line &line) const -> bool;
	auto intersects(const Line &line) const -> bool;
	auto move(coord_t xMod, coord_t yMod) const -> Rect;
	auto move(const Point &pos) const -> Rect;
	auto resize(coord_t mod) const -> Rect;
	auto deform(coord_t leftTopMod, coord_t rightBottomMod) const -> Rect;
	auto swap(bool swapHorizontal, bool swapVertical) const -> Rect;
	auto normalize() const -> Rect;
	auto intersection(const Rect &other) const -> Rect;
	auto combine(const Rect &other) const -> Rect;
	auto findOverlap(const Rect &other) const -> SearchResult;

	friend auto operator <<(std::ostream &out, const Rect &rect) -> std::ostream &;
private:
	Point m_leftTop;
	Point m_rightBottom;
};

inline
auto operator <<(std::ostream &out, const Rect &rect) -> std::ostream & {
	Rect normalized = rect.normalize();
	return out << "(" << normalized.m_leftTop << ", " << normalized.m_rightBottom << ")";
}