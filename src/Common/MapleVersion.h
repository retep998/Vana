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
#include <string>

using std::string;

#define MAPLE_VERSION 75 // Kept in for VersionConstants.cpp
// Use ONLY for version-specific code purposes
// Incidentally, your locale may have vastly different version-specific code

namespace Locales {
	const int8_t Korea = 0x01;
	const int8_t Japan = 0x03;
	const int8_t China = 0x05;
	const int8_t Sea = 0x07;
	const int8_t Thailand = 0x07;
	const int8_t Global = 0x08;
	const int8_t Europe = 0x09;
	const int8_t Brazil = 0x09;
	const int8_t Taiwan = 0x3C;
	const int8_t Vietnam = 0x00; // Unk

	const int8_t KoreaTest = 0x02;
	const int8_t ChinaTest = 0x04;
	const int8_t GlobalTest = 0x05;
	const int8_t TaiwanTest = 0x06;
	const int8_t EuropeTest = 0x00; // Unk
	const int8_t JapanTest = 0x00; // Unk
}

namespace PatchLocations {
	const string None = "";
	const string Global = "0";
	const string Korea = ""; // Unk
	const string Japan = ""; // Unk
	const string China = ""; // Unk
	const string Sea = ""; // Unk
	const string Thailand = ""; // Unk
	const string Europe = ""; // Unk
	const string Brazil = ""; // Unk
	const string Taiwan = ""; // Unk
	const string Vietnam = ""; // Unk

	const string GlobalTest = "0";
	const string KoreaTest = ""; // Unk
	const string ChinaTest = ""; // Unk
	const string TaiwanTest = ""; // Unk
	const string EuropeTest = ""; // Unk
	const string JapanTest = ""; // Unk
}

namespace MapleVersion {
	const int16_t Version = MAPLE_VERSION;
	const int8_t Locale = Locales::Global;
	const string LocaleString = "global"; // Used for MCDB verification
	const bool TestServer = false;

	const string PatchLocation = PatchLocations::Global;
}