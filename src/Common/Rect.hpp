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
#include <algorithm>
#include <cmath>

struct Line;

// IMPORTANT
// The assumption made in the Line and Rect classes are that the coordinate system works like you'd expect for x, but is inverted for y
// That is, -1000 x is on the left side, 1000 x is on the right side
// However, -1000 y is on the top and 1000 y is on the bottom
// Be VERY careful when dealing with things that require the Y position
class Rect {
public:
	Rect() = default;
	Rect(const Pos &leftTop, const Pos &rightBottom);

	auto left() const -> Line;
	auto right() const -> Line;
	auto top() const -> Line;
	auto bottom() const -> Line;
	auto rightTop() const -> Pos;
	auto leftTop() const -> Pos;
	auto leftBottom() const -> Pos;
	auto rightBottom() const -> Pos;
	auto area() const -> int32_t;
	auto perimeter() const -> int32_t;
	auto hypotenuse() const -> int32_t;
	auto height() const -> int32_t;
	auto width() const -> int32_t;
	auto contains(const Pos &pos) const -> bool;
	auto containsAnyPartOfLine(const Line &line) const -> bool;
	auto containsFullLine(const Line &line) const -> bool;
	auto intersects(const Line &line) const -> bool;
	auto move(int16_t xMod, int16_t yMod) const -> Rect;
	auto move(const Pos &pos) const -> Rect;
	auto resize(int16_t mod) const -> Rect;
	auto deform(int16_t leftTopMod, int16_t rightBottomMod) const -> Rect;
	auto swap(bool swapHorizontal, bool swapVertical) const -> Rect;
	auto normalize() const -> Rect;
	auto intersection(const Rect &other) const -> Rect;
	auto combine(const Rect &other) const -> Rect;
	auto findOverlap(const Rect &other) const -> SearchResult;
private:
	Pos m_leftTop;
	Pos m_rightBottom;
};