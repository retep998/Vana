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
#include <cmath>

struct Pos {
	Pos(int16_t x, int16_t y) : x(x), y(y) { }
	Pos() : x(0), y(0) { }
	int16_t x;
	int16_t y;
	int32_t operator-(const Pos &p) {
		return static_cast<int32_t>(sqrt(pow(static_cast<float>(x - p.x), 2) + pow(static_cast<float>(y - p.y), 2)));
	}
};
