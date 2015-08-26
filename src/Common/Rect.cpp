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
#include "Rect.hpp"
#include "Line.hpp"
#include "Types.hpp"
#include <algorithm>
#include <cmath>

Rect::Rect(const Point &leftTop, const Point &rightBottom) :
	m_leftTop{leftTop},
	m_rightBottom{rightBottom}
{
}

Rect::Rect(const Point &leftTop, coord_t width, coord_t height) :
	m_leftTop{leftTop},
	m_rightBottom{leftTop.x + width, leftTop.y + height}
{
}

Rect::Rect(coord_t leftTopX, coord_t leftTopY, coord_t width, coord_t height) :
	m_leftTop{leftTopX, leftTopY},
	m_rightBottom{leftTopX + width, leftTopY + height}
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

auto Rect::diagonalLeftTopRightBottom() const -> Line {
	return Line{m_leftTop, m_rightBottom};
}

auto Rect::diagonalLeftBottomRightTop() const -> Line {
	return Line{leftBottom(), rightTop()};
}

auto Rect::leftTop() const -> Point {
	return m_leftTop;
}

auto Rect::rightBottom() const -> Point {
	return m_rightBottom;
}

auto Rect::rightTop() const -> Point {
	return Point{m_rightBottom.x, m_leftTop.y};
}

auto Rect::leftBottom() const -> Point {
	return Point{m_leftTop.x, m_rightBottom.y};
}

auto Rect::center() const -> Point {
	return Point{
		(m_leftTop.x + m_rightBottom.x) / 2,
		(m_leftTop.y + m_rightBottom.y) / 2
	};;
}

auto Rect::area() const -> int32_t {
	return height() * width();
}

auto Rect::perimeter() const -> int32_t {
	return height() * 2 + width() * 2;
}

auto Rect::hypotenuse() const -> int32_t {
	return std::abs(m_leftTop - m_rightBottom);
}

auto Rect::height() const -> int32_t {
	return std::abs(static_cast<int32_t>(m_leftTop.y) - static_cast<int32_t>(m_rightBottom.y));
}

auto Rect::width() const -> int32_t {
	return std::abs(static_cast<int32_t>(m_rightBottom.x) - static_cast<int32_t>(m_leftTop.x));
}

auto Rect::contains(const Point &pos) const -> bool {
	return
		m_leftTop.y <= pos.y && pos.y <= m_rightBottom.y &&
		m_leftTop.x <= pos.x && pos.x <= m_rightBottom.x;
}

auto Rect::containsFullLine(const Line &line) const -> bool {
	return contains(line.pt1) && contains(line.pt2);
}

auto Rect::containsAnyPartOfLine(const Line &line) const -> bool {
	return containsFullLine(line) || intersects(line);
}

auto Rect::intersects(const Line &line) const -> bool {
	// Uses Cohen-Sutherland clipping to determine whether or not intersection takes place
	const int32_t bitsInside = 0x00;
	const int32_t bitsLeft = 0x01;
	const int32_t bitsRight = 0x02;
	const int32_t bitsBottom = 0x04;
	const int32_t bitsTop = 0x08;

	auto compute = [&](coord_t x, coord_t y) -> int32_t {
		int32_t result = bitsInside;
		if (x < m_leftTop.x) result |= bitsLeft;
		else if (x > m_rightBottom.x) result |= bitsRight;

		if (y < m_leftTop.y) result |= bitsTop;
		else if (y > m_rightBottom.y) result |= bitsBottom;
		return result;
	};

	coord_t x1 = line.pt1.x;
	coord_t x2 = line.pt2.x;
	coord_t y1 = line.pt1.y;
	coord_t y2 = line.pt2.y;
	int32_t testResultPt1 = compute(x1, y1);
	int32_t testResultPt2 = compute(x2, y2);
	bool hasAny = false;

	do {
		if (testResultPt1 == 0 && testResultPt2 == 0) {
			// Both are contained within the area(s), they do not intersect
			hasAny = true;
			break;
		}
		if ((testResultPt1 & testResultPt2) != 0) {
			// Both are outside of the area on sides incompatible with intersection, they do not intersect
			break;
		}

		coord_t x;
		coord_t y;
		int32_t outsideResult = testResultPt1 != 0 ?
			testResultPt1 :
			testResultPt2;

		if ((outsideResult & bitsBottom) != 0) {
			x = x1 + (x2 - x1) * (m_rightBottom.y - y1) / (y2 - y1);
			y = m_rightBottom.y;
		}
		else if ((outsideResult & bitsTop) != 0) {
			x = x1 + (x2 - x1) * (m_leftTop.y - y1) / (y2 - y1);
			y = m_leftTop.y;
		}
		else if ((outsideResult & bitsRight) != 0) {
			x = m_rightBottom.x;
			y = y1 + (y2 - y1) * (m_rightBottom.x - x1) / (x1 - x2);
		}
		else if ((outsideResult & bitsLeft) != 0) {
			x = m_leftTop.x;
			y = y1 + (y2 - y1) * (m_leftTop.x - x1) / (x1 - x2);
		}
		else throw NotImplementedException{"Should not reach here"};

		if (outsideResult == testResultPt1) {
			x1 = x;
			y1 = y;
			testResultPt1 = compute(x, y);
		}
		else {
			x2 = x;
			y2 = y;
			testResultPt2 = compute(x, y);
		}
	} while (true);

	return hasAny;
}

auto Rect::move(coord_t xMod, coord_t yMod) const -> Rect {
	Rect ret{m_leftTop, m_rightBottom};
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
	Rect ret{m_leftTop, m_rightBottom};
	ret.m_leftTop.x -= mod;
	ret.m_leftTop.y -= mod;
	ret.m_rightBottom.x += mod;
	ret.m_rightBottom.y += mod;
	return ret;
}

auto Rect::deform(coord_t leftTopMod, coord_t rightBottomMod) const -> Rect {
	Rect ret{m_leftTop, m_rightBottom};
	ret.m_leftTop.x += leftTopMod;
	ret.m_leftTop.y -= leftTopMod;
	ret.m_rightBottom.x += rightBottomMod;
	ret.m_rightBottom.y -= rightBottomMod;
	return ret;
}

auto Rect::swap(bool swapHorizontal, bool swapVertical) const -> Rect {
	Rect ret{m_leftTop, m_rightBottom};
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