/*
Copyright (C) 2008-2011 Vana Development Team

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

#include "Types.h"
#include <limits>

// LoopingId, keep track and returns a number between min and max, restarts when number reaches max
template<typename T>
class LoopingId {
public:
	LoopingId(T minimum = 1, T maximum = std::numeric_limits<T>::max());
	T next();
	void reset();
private:
	T m_current;
	T m_minimum;
	T m_maximum;
};

template<typename T>
LoopingId<T>::LoopingId(T minimum, T maximum) :
	m_current(minimum),
	m_minimum(minimum),
	m_maximum(maximum)
{
}

template<typename T>
T LoopingId<T>::next() {
	T ret = m_current;
	if (m_current == m_maximum) {
		m_current = m_minimum;
	}
	else {
		m_current++;
	}
	return ret;
}

template<typename T>
void LoopingId<T>::reset() {
	m_current = m_minimum;
}