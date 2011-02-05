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

namespace ExitCodes {
	enum {
		Ok = 0,
		ConfigError = 1,
		ConfigFileMissing = 2,
		McdbError = 3,
		McdbIncompatible = 4,
		InfoDatabaseError = 5,
		ServerVersionMismatch = 6,
		ServerConnectionError = 7,
		UnhandledExceptionHandled = 8,
		UnhandledExceptionStillUnhandled = 9
	};
	// Comments for easy searching
	// exit(0) exit(1) exit(2) exit(3) exit(4)
	// exit(5) exit(6) exit(7)
}