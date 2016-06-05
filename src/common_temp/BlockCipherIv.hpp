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
#include <cstring>

namespace vana {
	class block_cipher_iv {
	public:
		block_cipher_iv();
		explicit block_cipher_iv(crypto_iv iv);

		auto get_bytes() -> unsigned char const * const { return m_iv; }
		auto get_iv() const -> crypto_iv { return *reinterpret_cast<const crypto_iv *>(m_iv); }
		auto shuffle() -> void;
	private:
		friend class decoder;
		auto update_iv(crypto_iv iv) -> void;

		inline
		auto set_iv(unsigned char *dest, unsigned char *source) -> void {
			// The 16 byte IV is the 4 byte IV repeated 4 times
			for (uint8_t i = 0; i < 4; ++i) {
				memcpy(dest + i * 4, source, 4);
			}
		}

		unsigned char m_iv[16];
	};
}