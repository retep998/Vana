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

#include "Types.hpp"

namespace Vana {
	enum class BuffSkillValue {
		Predefined,
		// SpecialProcessing indicates an unusual value (e.g. combo orbs) or set of values
		SpecialProcessing,
		// SpecialPacket buffs share the same packet for first person/third person views
		SpecialPacket,
		BitpackedXy16,
		BitpackedSkillAndLevel32,
		SkillId,
		Level,
		X,
		Y,
		Speed,
		Jump,
		Watk,
		Wdef,
		Matk,
		Mdef,
		Accuracy,
		Avoid,
		Prop,
		Morph,
		GenderSpecificMorph,
		MobCount,
		Range,
		Damage,
		Time,
	};
}