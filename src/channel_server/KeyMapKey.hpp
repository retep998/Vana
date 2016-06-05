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

#include "common_temp/Types.hpp"

namespace vana {
	enum class key_map_key {
		none = 0,
		// ??
		one = 2,
		two = 3,
		three = 4,
		four = 5,
		five = 6,
		six = 7,
		seven = 8,
		eight = 9,
		nine = 10,
		zero = 11,
		minus = 12,
		equals = 13,
		// ??
		q = 16,
		w = 17,
		e = 18,
		r = 19,
		t = 20,
		y = 21,
		u = 22,
		i = 23,
		o = 24,
		p = 25,
		bracket_left = 26,
		bracket_right = 27,
		// ??
		ctrl = 29,
		a = 30,
		s = 31,
		d = 32,
		f = 33,
		g = 34,
		h = 35,
		j = 36,
		k = 37,
		l = 38,
		semicolon = 39,
		quote = 40,
		backtick = 41,
		shift = 42,
		backslash = 43,
		z = 44,
		x = 45,
		c = 46,
		v = 47,
		b = 48,
		n = 49,
		m = 50,
		comma = 51,
		dot = 52,
		// ??
		alt = 56,
		space = 57,
		// ??
		f1 = 59,
		f2 = 60,
		f3 = 61,
		f4 = 62,
		f5 = 63,
		f6 = 64,
		f7 = 65,
		f8 = 66,
		f9 = 67,
		f10 = 68,
		f11 = 69,
		f12 = 70,
		home = 71,
		// ??
		pageup = 73,
		// ??
		end = 79,
		// ??
		pagedown = 81,
		insert = 82,
		delete_key = 83,
	};
}