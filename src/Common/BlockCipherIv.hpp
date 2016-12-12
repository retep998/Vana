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

#include "Types.hpp"
#include <cstring>

using iv_t = uint32_t;

class BlockCipherIv {
public:
	BlockCipherIv();
	explicit BlockCipherIv(iv_t iv);
private:
	friend class Decoder;

	auto getBytes() -> unsigned char * { return m_iv; }
	auto getIv() const -> iv_t { return *reinterpret_cast<const iv_t *>(m_iv); }
	auto shuffle() -> void;
	auto updateIv(iv_t iv) -> void;

	inline
	auto setIv(unsigned char *dest, unsigned char *source) -> void {
		// The 16 byte IV is the 4 byte IV repeated 4 times
		for (uint8_t i = 0; i < 4; ++i) {
			memcpy(dest + i * 4, source, 4);
		}
	}

	unsigned char m_iv[16];
};