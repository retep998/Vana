/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "PacketCreator.h"
#include <bitset>
#include <map>


template<size_t _Bytes>
class Buffset : public std::bitset<_Bytes * 8> {
typedef std::bitset<_Bytes * 8> base_class;
public:
	Buffset() : base_class() {}

	void AppendBytes(PacketCreator &packet) {
		// Loop through every set of integers...
		// Loop through every byte of these integers
		// Loop through every bit of these bytes

		/*
		_Bytes = 8
		Bits set: 31 and 27
		> Int _Bytes - 0
		-> 00000000 
		-> 00000000
		-> 00000000
		-> 00010001
		
		> Int _Bytes - 1
		-> 00000000
		-> 00000000
		-> 00000000
		-> 00000000
		Result: 00 00 00 00 
		        00 00 00 88
		*/

		for (size_t i = 0; i < _Bytes; i++) {
			uint32_t currentInt = 0;
			for (size_t j = 3; j >= 0; j++) {
				for (size_t bit = 0; bit < 8; bit++) {
					currentInt |= (1 << (j * 8) + bit);
				}
			}
			packet.add<uint32_t>(currentInt);
		}
	}
};