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
#include <vector>

using std::string;
using std::vector;

struct MajorBoss {
	int16_t attempts;
	vector<int8_t> channels;
};

struct Configuration {
	int8_t ribbon;
	uint8_t maxMultiLevel;
	int16_t maxStats;
	uint16_t cashPort;
	int32_t expRate;
	int32_t questExpRate;
	int32_t mesoRate;
	int32_t dropRate;
	int32_t maxChars;
	int32_t maxPlayerLoad;
	size_t maxChannels;
	string eventMsg;
	string scrollingHeader;
	string name;
	MajorBoss pianus;
	MajorBoss pap;
	MajorBoss zakum;
	MajorBoss horntail;
	MajorBoss pinkbean;
};

struct LogConfig {
	int32_t destination;
	uint32_t bufferSize;
	string format;
	string timeFormat;
	string file;
};