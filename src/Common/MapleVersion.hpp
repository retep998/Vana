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
#include <string>

// Normal servers
#define MAPLE_LOCALE_GLOBAL 0x08
#define MAPLE_LOCALE_KOREA 0x01
#define MAPLE_LOCALE_JAPAN 0x03
#define MAPLE_LOCALE_CHINA 0x05
#define MAPLE_LOCALE_SEA 0x07
#define MAPLE_LOCALE_THAILAND 0x07
#define MAPLE_LOCALE_EUROPE 0x09
#define MAPLE_LOCALE_BRAZIL 0x09
#define MAPLE_LOCALE_TAIWAN 0x3C

// Test servers
#define MAPLE_LOCALE_KOREA_TEST 0x02
#define MAPLE_LOCALE_CHINA_TEST 0x04
#define MAPLE_LOCALE_TAIWAN_TEST 0x06
#define MAPLE_LOCALE_GLOBAL_TEST 0x05

// MCDB constants - used for verification
#define MAPLE_LOCALE_STRING_GLOBAL "global"
#define MAPLE_LOCALE_STRING_KOREA "korea"
#define MAPLE_LOCALE_STRING_JAPAN "japan"
#define MAPLE_LOCALE_STRING_CHINA "china"
#define MAPLE_LOCALE_STRING_EUROPE "europe"
#define MAPLE_LOCALE_STRING_THAILAND "thailand"
#define MAPLE_LOCALE_STRING_TAIWAN "taiwan"
#define MAPLE_LOCALE_STRING_SEA "sea"
#define MAPLE_LOCALE_STRING_BRAZIL "brazil"

// Use preprocessors ONLY for version-specific code purposes, prefer the strongly typed members of MapleVersion namespace otherwise
#define MAPLE_LOCALE MAPLE_LOCALE_GLOBAL
#define MAPLE_LOCALE_STRING MAPLE_LOCALE_STRING_GLOBAL
#define MAPLE_VERSION 75

namespace MapleVersion {
	const version_t Version = MAPLE_VERSION;
	const string_t LoginSubversion = "0";
	const string_t ChannelSubversion = "";
	const locale_t Locale = MAPLE_LOCALE;
	const string_t LocaleString = MAPLE_LOCALE_STRING;
	const bool TestServer = false;
}