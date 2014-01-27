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

#include "Types.h"
#include <string>

struct PlayerData {
	bool cashShop = false;
	bool admin = false;
	int16_t level = 0;
	int16_t job = 0;
	int16_t channel = 0;
	int32_t map = 0;
	int32_t party = 0;
	int32_t gmLevel = 0;
	uint32_t connectionTime = 0;
	string_t name;
};