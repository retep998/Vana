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

#include "Rect.hpp"
#include "Types.hpp"

namespace Vana {
	struct SkillLevelInfo {
		int8_t mobCount = 0;
		int8_t hitCount = 0;
		int8_t mastery = 0;
		uint8_t criticalDamage = 0;
		skill_level_t level = 0;
		health_t mp = 0;
		health_t hp = 0;
		slot_qty_t itemCount = 0;
		slot_qty_t bulletConsume = 0;
		int16_t moneyConsume = 0;
		int16_t x = 0;
		int16_t y = 0;
		stat_t speed = 0;
		stat_t jump = 0;
		stat_t str = 0;
		stat_t wAtk = 0;
		stat_t wDef = 0;
		stat_t mAtk = 0;
		stat_t mDef = 0;
		stat_t acc = 0;
		stat_t avo = 0;
		int16_t morph = 0;
		int16_t damage = 0;
		int16_t range = 0;
		uint16_t hpProp = 0;
		uint16_t mpProp = 0;
		uint16_t prop = 0;
		damage_t fixedDamage = 0;
		item_id_t item = 0;
		item_id_t optionalItem = 0;
		seconds_t buffTime = seconds_t{0};
		seconds_t coolTime = seconds_t{0};
		Rect dimensions;
	};
}