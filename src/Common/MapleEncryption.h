/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef MAPLEENC_H
#define MAPLEENC_H

#include "Types.h"

namespace MapleEncryption {
	void nextIV(unsigned char *vector);
	void mapleDecrypt(unsigned char *buf, int32_t size);
	void mapleEncrypt(unsigned char *buf, int32_t size);
	uint8_t ror(uint8_t val, int32_t num);
	uint8_t rol(uint8_t val, int32_t num);
};

#endif
