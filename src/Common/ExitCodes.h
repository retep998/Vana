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

#include <iomanip>
#include <iostream>

namespace ExitCodes {
	enum : int32_t {
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
		ProgramException = 11,
		QueryError = 12,
	};
	// Comments for easy searching
	// exit(0) exit(1) exit(2) exit(3) exit(4)
	// exit(5) exit(6) exit(7) exit(8) exit(9)
	// exit(10) exit(11) exit(12)

	inline
	auto exit(int32_t code) -> void {
#ifndef DAEMON
		std::cout << "Please press enter to quit..." << std::endl;
		getchar();
#endif
		::exit(code);
	}
}