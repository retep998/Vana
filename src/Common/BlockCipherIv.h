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

#include "Types.h"
#include <cstring>

class BlockCipherIv {
public:
	BlockCipherIv();
	explicit BlockCipherIv(uint32_t iv);

	auto updateIv(uint32_t iv) -> void;
	auto shuffle() -> void;
	auto getIv() const -> uint32_t { return *(uint32_t *)(m_iv); }
private:
	// Friended because the Decoder has a legit need for bytes (to do encryption) while nothing else does
	friend class Decoder;

	auto getBytes() -> unsigned char * { return m_iv; }

	inline
	auto setIv(unsigned char *dest, unsigned char *source) -> void {
		// The 16 byte IV is the 4 byte IV repeated 4 times
		for (uint8_t i = 0; i < 4; ++i) {
			memcpy(dest + i * 4, source, 4);
		}
	}

	unsigned char m_iv[16];
};