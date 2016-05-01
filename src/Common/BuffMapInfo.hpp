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

#include "Common/BuffSkillValue.hpp"
#include "Common/hash_combine.hpp"
#include "Common/Types.hpp"

namespace Vana {
	class BuffMapInfo {
	public:
		BuffMapInfo();
		BuffMapInfo(BuffSkillValue value);
		BuffMapInfo(uint8_t size, BuffSkillValue value);
		BuffMapInfo(uint8_t size, int64_t predefinedValue);

		auto getPredefinedValue() const -> int64_t;
		auto getValue() const -> BuffSkillValue;
		auto getSize() const -> uint8_t;
		auto withValue(BuffSkillValue value) const -> BuffMapInfo;
		auto withPredefinedValue(int16_t value) const ->BuffMapInfo;
	private:
		int64_t m_predefinedValue = 0;
		uint8_t m_size = 0;
		BuffSkillValue m_value = BuffSkillValue::Predefined;
	};
}