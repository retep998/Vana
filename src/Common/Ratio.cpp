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
#pragma once
#include "Ratio.hpp"
#include <cmath>

namespace Vana {

Ratio::Ratio(coord_t top, coord_t bottom) :
	m_isDefined{top != 0 && bottom != 0},
	m_isNegative{(top < 0) != (bottom < 0)}
{
	m_top = static_cast<coord_t>(std::abs(top));
	m_bottom = static_cast<coord_t>(std::abs(bottom));

	if (m_top > 0 && m_bottom > 0) {
		bool simplified = false;
		do {
			simplified = false;
			coord_t min = std::min(m_top, m_bottom);
			for (coord_t i = 2; i <= min; i++) {
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

auto Ratio::isUnit() const -> bool {
	return m_isDefined && m_top == m_bottom;
}

auto Ratio::isZero() const -> bool {
	return m_top == 0 && m_bottom == 0;
}

auto Ratio::isDefined() const -> bool {
	return m_isDefined;
}

auto Ratio::isNegative() const -> bool {
	return m_isNegative;
}

auto Ratio::top() const -> coord_t {
	return m_top;
}

auto Ratio::bottom() const -> coord_t {
	return m_bottom;
}

auto Ratio::reciprocal() const -> Ratio {
	return m_isNegative ?
		Ratio{-m_bottom, m_top} :
		Ratio{m_bottom, m_top};
}

auto Ratio::invertSign() const -> Ratio {
	return m_isNegative ?
		Ratio{m_top, m_bottom} :
		Ratio{-m_top, m_bottom};
}

auto Ratio::negativeReciprocal() const -> Ratio {
	return m_isNegative ?
		Ratio{m_bottom, m_top} :
		Ratio{-m_bottom, m_top};
}

}