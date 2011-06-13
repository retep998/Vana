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

namespace BitUtilities {
	template <typename T> T RotateRight(T val, int32_t shifts) {
		size_t size = sizeof(T) * 8;
		shifts %= size;
		return static_cast<T>((val >> shifts) | (val << (size - shifts)));
	}
	template <typename T> T RotateLeft(T val, int32_t shifts) {
		size_t size = sizeof(T) * 8;
		shifts %= size;
		return static_cast<T>((val << shifts) | (val >> (size - shifts)));
	}
}