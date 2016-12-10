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
#include <string>

// Normal servers
#define MAPLE_LOCALE_GLOBAL 0x08
#define MAPLE_LOCALE_KOREA 0x01
#define MAPLE_LOCALE_JAPAN 0x03
#define MAPLE_LOCALE_CHINA 0x05
#define MAPLE_LOCALE_TAIWAN 0x06
#define MAPLE_LOCALE_SEA 0x07
#define MAPLE_LOCALE_THAILAND 0x07
#define MAPLE_LOCALE_EUROPE 0x09
#define MAPLE_LOCALE_BRAZIL 0x09

// Test servers
#define MAPLE_LOCALE_KOREA_TEST 0x02
#define MAPLE_LOCALE_CHINA_TEST 0x04
#define MAPLE_LOCALE_TAIWAN_TEST 0x06
#define MAPLE_LOCALE_GLOBAL_TEST 0x05

// Use preprocessors ONLY for version-specific code purposes, prefer the strongly typed members of MapleVersion namespace otherwise
#define MAPLE_LOCALE MAPLE_LOCALE_GLOBAL
#define MAPLE_VERSION 75

namespace vana {
	namespace maple_version {
		const game_version version = static_cast<game_version>(MAPLE_VERSION);
		const string login_subversion = "0";
		const string channel_subversion = "";
		const game_locale locale = static_cast<game_locale>(MAPLE_LOCALE);
	}
}