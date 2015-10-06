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

#include "Common/BuffAction.hpp"
#include "Common/BuffMapInfo.hpp"
#include "Common/BuffSkillValue.hpp"
#include "Common/hash_combine.hpp"
#include "Common/Types.hpp"

namespace Vana {
	class BuffInfo {
	public:
		// Provided for default-construction
		// Not intended to be used directly
		BuffInfo() = default;

		static auto fromPlayerOnly(uint8_t bitPosition) -> BuffInfo;
		static auto fromPlayerOnly(uint8_t bitPosition, BuffSkillValue value) -> BuffInfo;
		static auto fromPlayerOnly(uint8_t bitPosition, int16_t predefinedValue) -> BuffInfo;
		static auto fromPlayerOnlyMovement(uint8_t bitPosition) -> BuffInfo;
		static auto fromPlayerOnlyMovement(uint8_t bitPosition, BuffSkillValue value) -> BuffInfo;
		static auto fromPlayerOnlyMovement(uint8_t bitPosition, int16_t predefinedValue) -> BuffInfo;
		static auto fromMapNoMovement(uint8_t bitPosition, BuffMapInfo mapValue) -> BuffInfo;
		static auto fromMapNoMovement(uint8_t bitPosition, BuffSkillValue value, BuffMapInfo mapValue) -> BuffInfo;
		static auto fromMapNoMovement(uint8_t bitPosition, int16_t predefinedValue, BuffMapInfo mapValue) -> BuffInfo;
		static auto fromMapMovement(uint8_t bitPosition, BuffMapInfo mapValue) -> BuffInfo;
		static auto fromMapMovement(uint8_t bitPosition, BuffSkillValue value, BuffMapInfo mapValue) -> BuffInfo;
		static auto fromMapMovement(uint8_t bitPosition, int16_t predefinedValue, BuffMapInfo mapValue) -> BuffInfo;

		auto getPredefinedValue() const -> int16_t;
		auto getMapInfo() const -> BuffMapInfo;
		auto getValue() const -> BuffSkillValue;
		auto getChance() const -> int16_t;
		auto getBitPosition() const -> uint8_t;
		auto getBuffByte() const -> uint8_t;
		auto getBuffType() const -> uint8_t;
		auto getAct() const -> BuffAction;
		auto getActValue() const -> BuffSkillValue;
		auto getActInterval() const -> duration_t;
		auto isMovementAffecting() const -> bool;
		auto hasMapInfo() const -> bool;
		auto hasAct() const -> bool;
		auto withValue(BuffSkillValue value) const -> BuffInfo;
		auto withMapInfo(BuffMapInfo value) const -> BuffInfo;
		auto withChance(int16_t chance) const -> BuffInfo;
		auto withPredefinedValue(int16_t value) const -> BuffInfo;
		auto withAct(BuffAction action, BuffSkillValue value, duration_t interval) const -> BuffInfo;
	private:
		auto static getBuffByte(uint8_t bitPosition) -> uint8_t;
		auto static getBuffType(uint8_t bitPosition) -> uint8_t;

		friend auto operator ==(const BuffInfo &a, const BuffInfo &b) -> bool;
		friend auto operator !=(const BuffInfo &a, const BuffInfo &b) -> bool;
		friend auto operator <(const BuffInfo &a, const BuffInfo &b) -> bool;
		friend auto operator <=(const BuffInfo &a, const BuffInfo &b) -> bool;
		friend auto operator >(const BuffInfo &a, const BuffInfo &b) -> bool;
		friend auto operator >=(const BuffInfo &a, const BuffInfo &b) -> bool;
		friend auto operator ==(uint8_t a, const BuffInfo &b) -> bool;
		friend auto operator !=(uint8_t a, const BuffInfo &b) -> bool;
		friend auto operator <(uint8_t a, const BuffInfo &b) -> bool;
		friend auto operator <=(uint8_t a, const BuffInfo &b) -> bool;
		friend auto operator >(uint8_t a, const BuffInfo &b) -> bool;
		friend auto operator >=(uint8_t a, const BuffInfo &b) -> bool;
		friend auto operator ==(const BuffInfo &a, uint8_t b) -> bool;
		friend auto operator !=(const BuffInfo &a, uint8_t b) -> bool;
		friend auto operator <(const BuffInfo &a, uint8_t b) -> bool;
		friend auto operator <=(const BuffInfo &a, uint8_t b) -> bool;
		friend auto operator >(const BuffInfo &a, uint8_t b) -> bool;
		friend auto operator >=(const BuffInfo &a, uint8_t b) -> bool;

		bool m_isMovementAffecting = false;
		bool m_hasMapInfo = false;
		uint8_t m_bitPosition = 0;
		int16_t m_predefinedValue = 0;
		int16_t m_chance = -1;
		duration_t m_actInterval = seconds_t{0};
		BuffAction m_act = BuffAction::None;
		BuffSkillValue m_actValue = BuffSkillValue::X;
		BuffSkillValue m_value = BuffSkillValue::Predefined;
		BuffMapInfo m_mapInfo;
	};

	inline
	auto operator ==(const BuffInfo &a, const BuffInfo &b) -> bool {
		return a.m_bitPosition == b.m_bitPosition;
	}

	inline
	auto operator !=(const BuffInfo &a, const BuffInfo &b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(const BuffInfo &a, const BuffInfo &b) -> bool {
		return a.m_bitPosition > b.m_bitPosition;
	}

	inline
	auto operator >=(const BuffInfo &a, const BuffInfo &b) -> bool {
		return a.m_bitPosition >= b.m_bitPosition;
	}

	inline
	auto operator <(const BuffInfo &a, const BuffInfo &b) -> bool {
		return a.m_bitPosition < b.m_bitPosition;
	}

	inline
	auto operator <=(const BuffInfo &a, const BuffInfo &b) -> bool {
		return a.m_bitPosition <= b.m_bitPosition;
	}

	inline
	auto operator ==(uint8_t a, const BuffInfo &b) -> bool {
		return a == b.m_bitPosition;
	}

	inline
	auto operator !=(uint8_t a, const BuffInfo &b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(uint8_t a, const BuffInfo &b) -> bool {
		return a > b.m_bitPosition;
	}

	inline
	auto operator >=(uint8_t a, const BuffInfo &b) -> bool {
		return a >= b.m_bitPosition;
	}

	inline
	auto operator <(uint8_t a, const BuffInfo &b) -> bool {
		return a < b.m_bitPosition;
	}

	inline
	auto operator <=(uint8_t a, const BuffInfo &b) -> bool {
		return a <= b.m_bitPosition;
	}

	inline
	auto operator ==(const BuffInfo &a, uint8_t b) -> bool {
		return a.m_bitPosition == b;
	}

	inline
	auto operator !=(const BuffInfo &a, uint8_t b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(const BuffInfo &a, uint8_t b) -> bool {
		return a.m_bitPosition > b;
	}

	inline
	auto operator >=(const BuffInfo &a, uint8_t b) -> bool {
		return a.m_bitPosition >= b;
	}

	inline
	auto operator <(const BuffInfo &a, uint8_t b) -> bool {
		return a.m_bitPosition < b;
	}

	inline
	auto operator <=(const BuffInfo &a, uint8_t b) -> bool {
		return a.m_bitPosition <= b;
	}
}

namespace std {
	template <>
	struct hash<Vana::BuffInfo> {
		auto operator()(const Vana::BuffInfo &v) const -> size_t {
			return Vana::MiscUtilities::hash_combinator(v.getBitPosition());
		}
	};
}