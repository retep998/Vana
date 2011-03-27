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

struct PlayerData {
	PlayerData() : cashShop(false), admin(false), party(0), gmLevel(0), connectionTime(0) { }
	bool cashShop;
	bool admin;
	int16_t level;
	int16_t job;
	int16_t channel;
	int32_t map;
	int32_t party;
	int32_t gmLevel;
	uint32_t connectionTime;
	string name;
};