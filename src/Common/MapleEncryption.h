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
#include <cstring>

namespace MapleEncryption {
	extern uint8_t values[256];
	void nextIv(unsigned char *vector);
	void mapleDecrypt(unsigned char *buf, int32_t size);
	void mapleEncrypt(unsigned char *buf, int32_t size);
	void setIv(unsigned char *dest, unsigned char *source);
	uint8_t ror(uint8_t val, int32_t num);
	uint8_t rol(uint8_t val, int32_t num);
};

inline
void MapleEncryption::setIv(unsigned char *dest, unsigned char *source) {
	// The 16 bit IV is the 4 bit IV repeated 4 times
	for (uint8_t i = 0; i < 4; i++) {
		memcpy(dest + i * 4, source, 4);
	}
}
