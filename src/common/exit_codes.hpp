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

#include "common/types.hpp"

namespace vana {
	using exit_code_underlying = int;
	enum class exit_code : exit_code_underlying {
		ok = 0,
		config_error = 1,
		config_file_missing = 2,
		mcdb_error = 3,
		mcdb_incompatible = 4,
		mcdb_locale_incompatible = 5,
		info_database_error = 6,
		sql_directory_not_found = 7,
		server_version_mismatch = 8,
		server_malformed_iv_packet = 9,
		server_connection_error = 10,
		server_disconnection = 11,
		program_exception = 12,
		query_error = 13,
		forced_by_gm = 14,
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

	auto exit(exit_code code) -> void;
}