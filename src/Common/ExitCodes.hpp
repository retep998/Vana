/*
Copyright (C) 2008-2015 Vana Development Team

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

namespace Vana {
	using exit_code_t = int;
	namespace ExitCodes {
		enum : exit_code_t {
			Ok = 0,
			ConfigError = 1,
			ConfigFileMissing = 2,
			McdbError = 3,
			McdbIncompatible = 4,
			McdbLocaleIncompatible = 5,
			InfoDatabaseError = 6,
			SqlDirectoryNotFound = 7,
			ServerVersionMismatch = 8,
			ServerMalformedIvPacket = 9,
			ServerConnectionError = 10,
			ServerDisconnection = 11,
			ProgramException = 12,
			QueryError = 13,
			ForcedByGm = 14,
		};
		// Comments for easy searching
		// exit(0) exit(0x0) exit(0x00) exit(0x00000000)
		// exit(1) exit(0x1) exit(0x01) exit(0x00000001)
		// exit(2) exit(0x2) exit(0x02) exit(0x00000002)
		// exit(3) exit(0x3) exit(0x03) exit(0x00000003)
		// exit(4) exit(0x4) exit(0x04) exit(0x00000004)
		// exit(5) exit(0x5) exit(0x05) exit(0x00000005)
		// exit(6) exit(0x6) exit(0x06) exit(0x00000006)
		// exit(7) exit(0x7) exit(0x07) exit(0x00000007)
		// exit(8) exit(0x8) exit(0x08) exit(0x00000008)
		// exit(9) exit(0x9) exit(0x09) exit(0x00000009)
		// exit(10) exit(0xA) exit(0x0A) exit(0x0000000A)
		// exit(11) exit(0xB) exit(0x0B) exit(0x0000000B)
		// exit(12) exit(0xC) exit(0x0C) exit(0x0000000C)
		// exit(13) exit(0xD) exit(0x0D) exit(0x0000000D)
		// exit(14) exit(0xE) exit(0x0E) exit(0x0000000E)

		auto exit(exit_code_t code) -> void;
	}
}