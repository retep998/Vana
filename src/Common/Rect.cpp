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
#include "Rect.hpp"
#include "Common/Line.hpp"
#include "Common/Types.hpp"
#include <algorithm>
#include <cmath>

namespace vana {

rect::rect(const point &left_top, const point &right_bottom) :
	m_left_top{left_top},
	m_right_bottom{right_bottom}
{
}

rect::rect(const point &left_top, game_coord width, game_coord height) :
	m_left_top{left_top},
	m_right_bottom{left_top.x + width, left_top.y + height}
{
}

rect::rect(game_coord left_top_x, game_coord left_top_y, game_coord width, game_coord height) :
	m_left_top{left_top_x, left_top_y},
	m_right_bottom{left_top_x + width, left_top_y + height}
{
}

auto rect::left() const -> line {
	return line{m_left_top, left_bottom()};
}

auto rect::right() const -> line {
	return line{right_top(), m_right_bottom};
}

auto rect::top() const -> line {
	return line{m_left_top, right_top()};
}

auto rect::bottom() const -> line {
	return line{left_bottom(), m_right_bottom};
}

auto rect::diagonal_left_top_right_bottom() const -> line {
	return line{m_left_top, m_right_bottom};
}

auto rect::diagonal_left_bottom_right_top() const -> line {
	return line{left_bottom(), right_top()};
}

auto rect::left_top() const -> point {
	return m_left_top;
}

auto rect::right_bottom() const -> point {
	return m_right_bottom;
}

auto rect::right_top() const -> point {
	return point{m_right_bottom.x, m_left_top.y};
}

auto rect::left_bottom() const -> point {
	return point{m_left_top.x, m_right_bottom.y};
}

auto rect::center() const -> point {
	return point{
		(m_left_top.x + m_right_bottom.x) / 2,
		(m_left_top.y + m_right_bottom.y) / 2
	};;
}

auto rect::area() const -> int32_t {
	return height() * width();
}

auto rect::perimeter() const -> int32_t {
	return height() * 2 + width() * 2;
}

auto rect::hypotenuse() const -> int32_t {
	return std::abs(m_left_top - m_right_bottom);
}

auto rect::height() const -> int32_t {
	return std::abs(static_cast<int32_t>(m_left_top.y) - static_cast<int32_t>(m_right_bottom.y));
}

auto rect::width() const -> int32_t {
	return std::abs(static_cast<int32_t>(m_right_bottom.x) - static_cast<int32_t>(m_left_top.x));
}

auto rect::contains(const point &pos) const -> bool {
	return
		m_left_top.y <= pos.y && pos.y <= m_right_bottom.y &&
		m_left_top.x <= pos.x && pos.x <= m_right_bottom.x;
}

auto rect::contains_full_line(const line &line) const -> bool {
	return contains(line.pt1) && contains(line.pt2);
}

auto rect::contains_any_part_of_line(const line &line) const -> bool {
	return contains_full_line(line) || intersects(line);
}

auto rect::intersects(const line &line) const -> bool {
	// Uses Cohen-Sutherland clipping to determine whether or not intersection takes place
	const int32_t bits_inside = 0x00;
	const int32_t bits_left = 0x01;
	const int32_t bits_right = 0x02;
	const int32_t bits_bottom = 0x04;
	const int32_t bits_top = 0x08;

	auto compute = [&](game_coord x, game_coord y) -> int32_t {
		int32_t result = bits_inside;
		if (x < m_left_top.x) result |= bits_left;
		else if (x > m_right_bottom.x) result |= bits_right;

		if (y < m_left_top.y) result |= bits_top;
		else if (y > m_right_bottom.y) result |= bits_bottom;
		return result;
	};

	game_coord x1 = line.pt1.x;
	game_coord x2 = line.pt2.x;
	game_coord y1 = line.pt1.y;
	game_coord y2 = line.pt2.y;
	int32_t test_result_pt1 = compute(x1, y1);
	int32_t test_result_pt2 = compute(x2, y2);
	bool has_any = false;

	do {
		if (test_result_pt1 == 0 && test_result_pt2 == 0) {
			// Both are contained within the area(s), they do not intersect
			has_any = true;
			break;
		}
		if ((test_result_pt1 & test_result_pt2) != 0) {
			// Both are outside of the area on sides incompatible with intersection, they do not intersect
			break;
		}

		game_coord x;
		game_coord y;
		int32_t outside_result = test_result_pt1 != 0 ?
			test_result_pt1 :
			test_result_pt2;

		if ((outside_result & bits_bottom) != 0) {
			x = x1 + (x2 - x1) * (m_right_bottom.y - y1) / (y2 - y1);
			y = m_right_bottom.y;
		}
		else if ((outside_result & bits_top) != 0) {
			x = x1 + (x2 - x1) * (m_left_top.y - y1) / (y2 - y1);
			y = m_left_top.y;
		}
		else if ((outside_result & bits_right) != 0) {
			x = m_right_bottom.x;
			y = y1 + (y2 - y1) * (m_right_bottom.x - x1) / (x1 - x2);
		}
		else if ((outside_result & bits_left) != 0) {
			x = m_left_top.x;
			y = y1 + (y2 - y1) * (m_left_top.x - x1) / (x1 - x2);
		}
		else {
			throw codepath_invalid_exception{"If there are no bits set, that case should be taken care of earlier"};
		}

		if (outside_result == test_result_pt1) {
			x1 = x;
			y1 = y;
			test_result_pt1 = compute(x, y);
		}
		else {
			x2 = x;
			y2 = y;
			test_result_pt2 = compute(x, y);
		}
	} while (true);

	return has_any;
}

auto rect::move(game_coord offset_x, game_coord offset_y) const -> rect {
	rect ret{m_left_top, m_right_bottom};
	ret.m_left_top.x += offset_x;
	ret.m_left_top.y += offset_y;
	ret.m_right_bottom.x += offset_x;
	ret.m_right_bottom.y += offset_y;
	return ret;
}

auto rect::move(const point &pos) const -> rect {
	return move(pos.x, pos.y);
}

auto rect::resize(game_coord mod) const -> rect {
	rect ret{m_left_top, m_right_bottom};
	ret.m_left_top.x -= mod;
	ret.m_left_top.y -= mod;
	ret.m_right_bottom.x += mod;
	ret.m_right_bottom.y += mod;
	return ret;
}

auto rect::deform(game_coord left_top_mod, game_coord right_bottom_mod) const -> rect {
	rect ret{m_left_top, m_right_bottom};
	ret.m_left_top.x += left_top_mod;
	ret.m_left_top.y -= left_top_mod;
	ret.m_right_bottom.x += right_bottom_mod;
	ret.m_right_bottom.y -= right_bottom_mod;
	return ret;
}

auto rect::swap(bool swap_horizontal, bool swap_vertical) const -> rect {
	rect ret{m_left_top, m_right_bottom};
	if (swap_horizontal) {
		ret.m_left_top.x = m_right_bottom.x;
		ret.m_right_bottom.x = m_left_top.x;
	}
	if (swap_vertical) {
		ret.m_left_top.y = m_right_bottom.y;
		ret.m_right_bottom.y = m_left_top.y;
	}
	return ret;
}

auto rect::normalize() const -> rect {
	return swap(m_left_top.x > m_right_bottom.x, m_left_top.y > m_right_bottom.y);
}

auto rect::intersection(const rect &other) const -> rect {
	rect ret = normalize();
	rect test = other.normalize();
	ret.m_right_bottom.y = std::min(ret.m_right_bottom.y, test.m_right_bottom.y);
	ret.m_right_bottom.x = std::min(ret.m_right_bottom.x, test.m_right_bottom.x);
	ret.m_left_top.y = std::max(ret.m_left_top.y, test.m_left_top.y);
	ret.m_left_top.x = std::max(ret.m_left_top.x, test.m_left_top.x);
	return ret;
}

auto rect::combine(const rect &other) const -> rect {
	rect ret = normalize();
	rect test = other.normalize();
	ret.m_right_bottom.y = std::max(ret.m_right_bottom.y, test.m_right_bottom.y);
	ret.m_right_bottom.x = std::max(ret.m_right_bottom.x, test.m_right_bottom.x);
	ret.m_left_top.y = std::min(ret.m_left_top.y, test.m_left_top.y);
	ret.m_left_top.x = std::min(ret.m_left_top.x, test.m_left_top.x);
	return ret;
}

auto rect::find_overlap(const rect &other) const -> search_result {
	// Can't overlap when there's nothing to overlap
	if (other.height() == 0 && other.width() == 0) {
		return search_result::not_found;
	}
	if (height() == 0 && width() == 0) {
		return search_result::not_found;
	}

	rect normalized = normalize();
	rect test = other.normalize();
	bool noOverlap = normalized.m_left_top.x > test.m_right_bottom.x &&
		test.m_left_top.x > normalized.m_right_bottom.x &&
		normalized.m_left_top.y < test.m_right_bottom.y &&
		test.m_left_top.y < normalized.m_right_bottom.y;

	return noOverlap ? search_result::not_found : search_result::found;
}

}