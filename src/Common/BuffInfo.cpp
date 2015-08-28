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
#include "BuffInfo.hpp"

namespace Vana {

BuffInfo::BuffInfo()
{
}

auto BuffInfo::fromPlayerOnly(uint8_t bitPosition) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	return ret;
}

auto BuffInfo::fromPlayerOnly(uint8_t bitPosition, BuffSkillValue value) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_value = value;
	return ret;
}

auto BuffInfo::fromPlayerOnly(uint8_t bitPosition, int16_t predefinedValue) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_predefinedValue = predefinedValue;
	return ret;
}

auto BuffInfo::fromPlayerOnlyMovement(uint8_t bitPosition) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_isMovementAffecting = true;
	return ret;
}

auto BuffInfo::fromPlayerOnlyMovement(uint8_t bitPosition, BuffSkillValue value) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_value = value;
	ret.m_isMovementAffecting = true;
	return ret;
}

auto BuffInfo::fromPlayerOnlyMovement(uint8_t bitPosition, int16_t predefinedValue) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_predefinedValue = predefinedValue;
	ret.m_isMovementAffecting = true;
	return ret;
}

auto BuffInfo::fromMapNoMovement(uint8_t bitPosition, BuffMapInfo mapInfo) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_hasMapInfo = true;
	ret.m_mapInfo = mapInfo;
	return ret;
}

auto BuffInfo::fromMapNoMovement(uint8_t bitPosition, BuffSkillValue value, BuffMapInfo mapInfo) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_hasMapInfo = true;
	ret.m_mapInfo = mapInfo;
	ret.m_value = value;
	return ret;
}

auto BuffInfo::fromMapNoMovement(uint8_t bitPosition, int16_t predefinedValue, BuffMapInfo mapInfo) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_hasMapInfo = true;
	ret.m_mapInfo = mapInfo;
	ret.m_predefinedValue = predefinedValue;
	return ret;
}

auto BuffInfo::fromMapMovement(uint8_t bitPosition, BuffMapInfo mapInfo) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_hasMapInfo = true;
	ret.m_mapInfo = mapInfo;
	ret.m_isMovementAffecting = true;
	return ret;
}

auto BuffInfo::fromMapMovement(uint8_t bitPosition, BuffSkillValue value, BuffMapInfo mapInfo) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_hasMapInfo = true;
	ret.m_mapInfo = mapInfo;
	ret.m_value = value;
	ret.m_isMovementAffecting = true;
	return ret;
}

auto BuffInfo::fromMapMovement(uint8_t bitPosition, int16_t predefinedValue, BuffMapInfo mapInfo) -> BuffInfo {
	BuffInfo ret;
	ret.m_bitPosition = bitPosition;
	ret.m_hasMapInfo = true;
	ret.m_mapInfo = mapInfo;
	ret.m_predefinedValue = predefinedValue;
	ret.m_isMovementAffecting = true;
	return ret;
}

auto BuffInfo::getPredefinedValue() const -> int16_t {
	return m_predefinedValue;
}

auto BuffInfo::getMapInfo() const -> BuffMapInfo {
	return m_mapInfo;
}

auto BuffInfo::getValue() const -> BuffSkillValue {
	return m_value;
}

auto BuffInfo::getChance() const -> int16_t {
	return m_chance;
}

auto BuffInfo::getBitPosition() const -> uint8_t {
	return m_bitPosition;
}

auto BuffInfo::getBuffByte() const -> uint8_t {
	return getBuffByte(m_bitPosition);
}

auto BuffInfo::getBuffType() const -> uint8_t {
	return getBuffType(m_bitPosition);
}

auto BuffInfo::getAct() const -> BuffAction {
	return m_act;
}

auto BuffInfo::getActValue() const -> BuffSkillValue {
	return m_actValue;
}

auto BuffInfo::getActInterval() const -> duration_t {
	return m_actInterval;
}

auto BuffInfo::isMovementAffecting() const -> bool {
	return m_isMovementAffecting;
}

auto BuffInfo::hasMapInfo() const -> bool {
	return m_hasMapInfo;
}

auto BuffInfo::hasAct() const -> bool {
	return m_act != BuffAction::None;
}

auto BuffInfo::withValue(BuffSkillValue value) const -> BuffInfo {
	BuffInfo result{*this};
	result.m_value = value;
	return result;
}

auto BuffInfo::withMapInfo(BuffMapInfo value) const -> BuffInfo {
	BuffInfo result{*this};
	result.m_mapInfo = value;
	result.m_hasMapInfo = true;
	return result;
}

auto BuffInfo::withChance(int16_t value) const -> BuffInfo {
	BuffInfo result{*this};
	result.m_chance = value;
	return result;
}

auto BuffInfo::withPredefinedValue(int16_t value) const -> BuffInfo {
	BuffInfo result{*this};
	result.m_value = BuffSkillValue::Predefined;
	result.m_predefinedValue = value;
	return result;
}

auto BuffInfo::withAct(BuffAction action, BuffSkillValue value, duration_t interval) const -> BuffInfo {
	BuffInfo result{*this};
	result.m_act = action;
	result.m_actValue = value;
	result.m_actInterval = interval;
	return result;
}

auto BuffInfo::getBuffByte(uint8_t bitPosition) -> uint8_t {
	return (bitPosition - 1) / 8;
}

auto BuffInfo::getBuffType(uint8_t bitPosition) -> uint8_t {
	return 1U << ((bitPosition - 1) % 8);
}

}