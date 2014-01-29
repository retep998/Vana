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

#include <limits>

// Keep track and returns a number between min and max, restarts when number reaches max
template <typename TInteger>
class LoopingId {
public:
	explicit LoopingId(TInteger minimum = 1, TInteger maximum = std::numeric_limits<TInteger>::max()) :
		m_current(minimum),
		m_minimum(minimum),
		m_maximum(maximum)
	{
	}

	auto next() -> TInteger {
		TInteger ret = m_current;
		if (m_current == m_maximum) {
			m_current = m_minimum;
		}
		else {
			m_current++;
		}
		return ret;
	}

	auto reset() -> void { m_current = m_minimum; }
private:
	TInteger m_current;
	TInteger m_minimum;
	TInteger m_maximum;
};