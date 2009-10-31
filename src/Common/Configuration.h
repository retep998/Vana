/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct Configuration {
	int8_t ribbon;
	uint8_t maxMultiLevel;
	int16_t pianusAttempts;
	int16_t papAttempts;
	int16_t zakumAttempts;
	int16_t horntailAttempts;
	int16_t pinkbeanAttempts;
	int16_t maxStats;
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
	vector<int8_t> pianusChannels;
	vector<int8_t> papChannels;
	vector<int8_t> zakumChannels;
	vector<int8_t> horntailChannels;
	vector<int8_t> pinkbeanChannels;
};

#endif