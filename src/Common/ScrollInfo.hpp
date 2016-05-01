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

#include "Common/Types.hpp"

namespace Vana {
	struct ScrollInfo {
		bool warmSupport = false;
		bool randStat = false;
		bool preventSlip = false;
		int8_t recover = 0;
		stat_t istr = 0;
		stat_t idex = 0;
		stat_t iint = 0;
		stat_t iluk = 0;
		health_t ihp = 0;
		health_t imp = 0;
		stat_t iwAtk = 0;
		stat_t imAtk = 0;
		stat_t iwDef = 0;
		stat_t imDef = 0;
		stat_t iacc = 0;
		stat_t iavo = 0;
		stat_t ihand = 0;
		stat_t ijump = 0;
		stat_t ispeed = 0;
		uint16_t success = 0;
		uint16_t cursed = 0;
	};
}