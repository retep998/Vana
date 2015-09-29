/*
Copyright (C) 2008-2015 Vana Development Team

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
	struct QuestRewardInfo {
		bool isMesos = false;
		bool isItem = false;
		bool isExp = false;
		bool isFame = false;
		bool isSkill = false;
		bool isBuff = false;
		bool masterLevelOnly = false;
		gender_id_t gender = 0;
		int16_t count = 0;
		int16_t masterLevel = 0;
		int32_t prop = 0;
		int32_t id = 0;
	};
}