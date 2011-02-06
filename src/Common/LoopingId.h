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
class LoopingId {
public:
	LoopingId(int32_t minimum = 1, int32_t maximum = std::numeric_limits<int32_t>::max());
	int32_t next();
	void reset();
private:
	int32_t current;
	int32_t minimum;
	int32_t maximum;
};

inline LoopingId::LoopingId(int32_t minimum, int32_t maximum) : current(minimum), minimum(minimum), maximum(maximum) { }

inline int32_t LoopingId::next() {
	int32_t ret = current;
	if (current == maximum) {
		current = minimum;
	}
	else {
		current++;
	}
	return ret;
}

inline void LoopingId::reset() {
	current = minimum;
}
