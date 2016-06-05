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
#include "ratio_temp.hpp"
#include <cmath>

namespace vana {

ratio::ratio(game_coord top, game_coord bottom) :
	m_is_defined{top != 0 && bottom != 0},
	m_is_negative{(top < 0) != (bottom < 0)}
{
	m_top = static_cast<game_coord>(std::abs(top));
	m_bottom = static_cast<game_coord>(std::abs(bottom));

	if (m_top > 0 && m_bottom > 0) {
		bool simplified = false;
		do {
			simplified = false;
			game_coord min = std::min(m_top, m_bottom);
			for (game_coord i = 2; i <= min; i++) {
				if (m_top % i == 0 && m_bottom % i == 0) {
					m_top /= i;
					m_bottom /= i;
					simplified = true;
					break;
				}
			}
		} while (simplified);
	}
}

auto ratio::is_unit() const -> bool {
	return m_is_defined && m_top == m_bottom;
}

auto ratio::is_zero() const -> bool {
	return m_top == 0 && m_bottom == 0;
}

auto ratio::is_defined() const -> bool {
	return m_is_defined;
}

auto ratio::is_negative() const -> bool {
	return m_is_negative;
}

auto ratio::top() const -> game_coord {
	return m_top;
}

auto ratio::bottom() const -> game_coord {
	return m_bottom;
}

auto ratio::reciprocal() const -> ratio {
	return m_is_negative ?
		ratio{-m_bottom, m_top} :
		ratio{m_bottom, m_top};
}

auto ratio::invert_sign() const -> ratio {
	return m_is_negative ?
		ratio{m_top, m_bottom} :
		ratio{-m_top, m_bottom};
}

auto ratio::negative_reciprocal() const -> ratio {
	return m_is_negative ?
		ratio{m_bottom, m_top} :
		ratio{-m_bottom, m_top};
}

}