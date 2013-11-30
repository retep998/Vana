/*
Copyright (C) 2008-2013 Vana Development Team

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
#include <string>

using std::string;

#define MAPLE_LOCALE_GLOBAL 0x08
#define MAPLE_LOCALE_KOREA 0x01
#define MAPLE_LOCALE_JAPAN 0x03
#define MAPLE_LOCALE_CHINA 0x05
#define MAPLE_LOCALE_SEA 0x07
#define MAPLE_LOCALE_THAILAND 0x07
#define MAPLE_LOCALE_EUROPE 0x09
#define MAPLE_LOCALE_BRAZIL 0x09
#define MAPLE_LOCALE_TAIWAN 0x3C

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

namespace Locales {
	const int8_t Korea = MAPLE_LOCALE_KOREA;
	const int8_t Japan = MAPLE_LOCALE_JAPAN;
	const int8_t China = MAPLE_LOCALE_CHINA;
	const int8_t Sea = MAPLE_LOCALE_SEA;
	const int8_t Thailand = MAPLE_LOCALE_THAILAND;
	const int8_t Global = MAPLE_LOCALE_GLOBAL;
	const int8_t Europe = MAPLE_LOCALE_EUROPE;
	const int8_t Brazil = MAPLE_LOCALE_BRAZIL;
	const int8_t Taiwan = MAPLE_LOCALE_TAIWAN;

	const int8_t KoreaTest = MAPLE_LOCALE_KOREA_TEST;
	const int8_t ChinaTest = MAPLE_LOCALE_CHINA_TEST;
	const int8_t GlobalTest = MAPLE_LOCALE_GLOBAL_TEST;
	const int8_t TaiwanTest = MAPLE_LOCALE_TAIWAN_TEST;
}

namespace PatchLocations {
	const string None = "";
	const string Global = "0";

	const string GlobalTest = "0";
}

namespace MapleVersion {
	const uint16_t Version = MAPLE_VERSION;
	const int8_t Locale = MAPLE_LOCALE;
	const string LocaleString = MAPLE_LOCALE_STRING;
	const bool TestServer = false;
	const string PatchLocation = PatchLocations::Global;
}