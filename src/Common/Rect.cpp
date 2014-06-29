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
#include "Rect.hpp"
#include "Line.hpp"
#include "Types.hpp"
#include <algorithm>
#include <cmath>

Rect::Rect(const Point &leftTop, const Point &rightBottom) :
	m_leftTop(leftTop),
	m_rightBottom(rightBottom)
{
}

auto Rect::left() const -> Line {
	return Line{m_leftTop, leftBottom()};
}

auto Rect::right() const -> Line {
	return Line{rightTop(), m_rightBottom};
}

auto Rect::top() const -> Line {
	return Line{m_leftTop, rightTop()};
}

auto Rect::bottom() const -> Line {
	return Line{leftBottom(), m_rightBottom};
}

auto Rect::leftTop() const -> Point {
	return m_leftTop;
}

auto Rect::rightBottom() const -> Point {
	return m_rightBottom;
}

auto Rect::rightTop() const -> Point {
	Point ret{m_rightBottom.x, m_leftTop.y};
	return ret;
}

auto Rect::leftBottom() const -> Point {
	Point ret{m_leftTop.x, m_rightBottom.y};
	return ret;
}

auto Rect::area() const -> int32_t {
	return height() * width();
}

auto Rect::perimeter() const -> int32_t {
	return height() * 2 + width() * 2;
}

auto Rect::hypotenuse() const -> int32_t {
	return abs(m_leftTop - m_rightBottom);
}

auto Rect::height() const -> int32_t {
	return abs(static_cast<int32_t>(m_leftTop.y) - static_cast<int32_t>(m_rightBottom.y));
}

auto Rect::width() const -> int32_t {
	return abs(static_cast<int32_t>(m_rightBottom.x) - static_cast<int32_t>(m_leftTop.x));
}

auto Rect::contains(const Point &pos) const -> bool {
	return pos.y <= m_rightBottom.y && pos.y >= m_leftTop.y && pos.x >= m_leftTop.x && pos.x <= m_rightBottom.x;
}

auto Rect::containsFullLine(const Line &line) const -> bool {
	return contains(line.pt1) && contains(line.pt2);
}

auto Rect::containsAnyPartOfLine(const Line &line) const -> bool {
	return containsFullLine(line) || intersects(line);
}

auto Rect::intersects(const Line &line) const -> bool {
	int32_t x1 = line.pt1.x;
	int32_t x2 = line.pt2.x;
	int32_t y1 = line.pt1.y;
	int32_t y2 = line.pt2.y;

	if (x1 > m_rightBottom.x && x2 > m_rightBottom.x) {
		return false;
	}
	if (x1 < m_leftTop.x && x2 < m_leftTop.x) {
		return false;
	}
	if (y1 < m_leftTop.y && y2 < m_leftTop.y) {
		return false;
	}
	if (y1 > m_rightBottom.y && y2 > m_rightBottom.y) {
		return false;
	}

	auto testPoint = [x1, x2, y1, y2](const Point &pos) -> int32_t {
		return (y2 - y1) * static_cast<int32_t>(pos.x) + (x1 - x2) * static_cast<int32_t>(pos.y) + (x2 * y1 - x1 * y2);
	};

	int32_t testLeftTop = testPoint(m_leftTop);
	int32_t testRightTop = testPoint(rightTop());
	int32_t testRightBottom = testPoint(m_rightBottom);
	int32_t testLeftBottom = testPoint(leftBottom());
	if (testLeftTop == 0 || testRightTop == 0 || testRightBottom == 0 || testLeftBottom == 0) {
		return true;
	}

	return false;
}

auto Rect::move(coord_t xMod, coord_t yMod) const -> Rect {
	Rect ret = {m_leftTop, m_rightBottom};
	ret.m_leftTop.x += xMod;
	ret.m_leftTop.y += yMod;
	ret.m_rightBottom.x += xMod;
	ret.m_rightBottom.y += yMod;
	return ret;
}

auto Rect::move(const Point &pos) const -> Rect {
	return move(pos.x, pos.y);
}

auto Rect::resize(coord_t mod) const -> Rect {
	Rect ret = {m_leftTop, m_rightBottom};
	ret.m_leftTop.x -= mod;
	ret.m_leftTop.y -= mod;
	ret.m_rightBottom.x += mod;
	ret.m_rightBottom.y += mod;
	return ret;
}

auto Rect::deform(coord_t leftTopMod, coord_t rightBottomMod) const -> Rect {
	Rect ret = {m_leftTop, m_rightBottom};
	ret.m_leftTop.x += leftTopMod;
	ret.m_leftTop.y -= leftTopMod;
	ret.m_rightBottom.x += rightBottomMod;
	ret.m_rightBottom.y -= rightBottomMod;
	return ret;
}

auto Rect::swap(bool swapHorizontal, bool swapVertical) const -> Rect {
	Rect ret = {m_leftTop, m_rightBottom};
	if (swapHorizontal) {
		ret.m_leftTop.x = m_rightBottom.x;
		ret.m_rightBottom.x = m_leftTop.x;
	}
	if (swapVertical) {
		ret.m_leftTop.y = m_rightBottom.y;
		ret.m_rightBottom.y = m_leftTop.y;
	}
	return ret;
}

auto Rect::normalize() const -> Rect {
	return swap(m_leftTop.x > m_rightBottom.x, m_leftTop.y > m_rightBottom.y);
}

auto Rect::intersection(const Rect &other) const -> Rect {
	Rect ret = normalize();
	Rect test = other.normalize();
	ret.m_rightBottom.y = std::min(ret.m_rightBottom.y, test.m_rightBottom.y);
	ret.m_rightBottom.x = std::min(ret.m_rightBottom.x, test.m_rightBottom.x);
	ret.m_leftTop.y = std::max(ret.m_leftTop.y, test.m_leftTop.y);
	ret.m_leftTop.x = std::max(ret.m_leftTop.x, test.m_leftTop.x);
	return ret;
}

auto Rect::combine(const Rect &other) const -> Rect {
	Rect ret = normalize();
	Rect test = other.normalize();
	ret.m_rightBottom.y = std::max(ret.m_rightBottom.y, test.m_rightBottom.y);
	ret.m_rightBottom.x = std::max(ret.m_rightBottom.x, test.m_rightBottom.x);
	ret.m_leftTop.y = std::min(ret.m_leftTop.y, test.m_leftTop.y);
	ret.m_leftTop.x = std::min(ret.m_leftTop.x, test.m_leftTop.x);
	return ret;
}

auto Rect::findOverlap(const Rect &other) const -> SearchResult {
	// Can't overlap when there's nothing to overlap
	if (other.height() == 0 && other.width() == 0) {
		return SearchResult::NotFound;
	}
	if (height() == 0 && width() == 0) {
		return SearchResult::NotFound;
	}

	Rect normalized = normalize();
	Rect test = other.normalize();
	bool noOverlap = normalized.m_leftTop.x > test.m_rightBottom.x &&
		test.m_leftTop.x > normalized.m_rightBottom.x &&
		normalized.m_leftTop.y < test.m_rightBottom.y &&
		test.m_leftTop.y < normalized.m_rightBottom.y;

	return noOverlap ? SearchResult::NotFound : SearchResult::Found;
}