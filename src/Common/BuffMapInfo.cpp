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
#include "BuffMapInfo.hpp"

BuffMapInfo::BuffMapInfo()
{
}

BuffMapInfo::BuffMapInfo(BuffSkillValue value) :
	m_value{value}
{
}

BuffMapInfo::BuffMapInfo(uint8_t size, BuffSkillValue value) :
	m_size{size},
	m_value{value}
{
}

BuffMapInfo::BuffMapInfo(uint8_t size, int64_t predefinedValue) :
	m_size{size},
	m_predefinedValue{predefinedValue}
{
}

auto BuffMapInfo::getPredefinedValue() const -> int64_t {
	return m_predefinedValue;
}

auto BuffMapInfo::getSize() const -> uint8_t {
	return m_size;
}

auto BuffMapInfo::getValue() const -> BuffSkillValue {
	return m_value;
}

auto BuffMapInfo::withValue(BuffSkillValue value) const -> BuffMapInfo {
	BuffMapInfo result{*this};
	result.m_value= value;
	return result;
}

auto BuffMapInfo::withPredefinedValue(int16_t value) const -> BuffMapInfo {
	BuffMapInfo result{*this};
	result.m_value = BuffSkillValue::Predefined;
	result.m_predefinedValue = value;
	return result;
}