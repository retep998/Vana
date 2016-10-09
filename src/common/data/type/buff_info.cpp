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
#include "buff_info.hpp"

namespace vana {
namespace data {
namespace type {

auto buff_info::from_player_only(uint8_t bit_position) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	return ret;
}

auto buff_info::from_player_only(uint8_t bit_position, buff_skill_value value) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_value = value;
	return ret;
}

auto buff_info::from_player_only(uint8_t bit_position, int16_t predefined_value) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_predefined_value = predefined_value;
	return ret;
}

auto buff_info::from_player_only_movement(uint8_t bit_position) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_is_movement_affecting = true;
	return ret;
}

auto buff_info::from_player_only_movement(uint8_t bit_position, buff_skill_value value) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_value = value;
	ret.m_is_movement_affecting = true;
	return ret;
}

auto buff_info::from_player_only_movement(uint8_t bit_position, int16_t predefined_value) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_predefined_value = predefined_value;
	ret.m_is_movement_affecting = true;
	return ret;
}

auto buff_info::from_map_no_movement(uint8_t bit_position, buff_map_info map_info) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_has_map_info = true;
	ret.m_map_info = map_info;
	return ret;
}

auto buff_info::from_map_no_movement(uint8_t bit_position, buff_skill_value value, buff_map_info map_info) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_has_map_info = true;
	ret.m_map_info = map_info;
	ret.m_value = value;
	return ret;
}

auto buff_info::from_map_no_movement(uint8_t bit_position, int16_t predefined_value, buff_map_info map_info) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_has_map_info = true;
	ret.m_map_info = map_info;
	ret.m_predefined_value = predefined_value;
	return ret;
}

auto buff_info::from_map_movement(uint8_t bit_position, buff_map_info map_info) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_has_map_info = true;
	ret.m_map_info = map_info;
	ret.m_is_movement_affecting = true;
	return ret;
}

auto buff_info::from_map_movement(uint8_t bit_position, buff_skill_value value, buff_map_info map_info) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_has_map_info = true;
	ret.m_map_info = map_info;
	ret.m_value = value;
	ret.m_is_movement_affecting = true;
	return ret;
}

auto buff_info::from_map_movement(uint8_t bit_position, int16_t predefined_value, buff_map_info map_info) -> buff_info {
	buff_info ret;
	ret.m_bit_position = bit_position;
	ret.m_has_map_info = true;
	ret.m_map_info = map_info;
	ret.m_predefined_value = predefined_value;
	ret.m_is_movement_affecting = true;
	return ret;
}

auto buff_info::get_predefined_value() const -> int16_t {
	return m_predefined_value;
}

auto buff_info::get_map_info() const -> buff_map_info {
	return m_map_info;
}

auto buff_info::get_value() const -> buff_skill_value {
	return m_value;
}

auto buff_info::get_chance() const -> int16_t {
	return m_chance;
}

auto buff_info::get_bit_position() const -> uint8_t {
	return m_bit_position;
}

auto buff_info::get_buff_byte() const -> uint8_t {
	return get_buff_byte(m_bit_position);
}

auto buff_info::get_buff_type() const -> uint8_t {
	return get_buff_type(m_bit_position);
}

auto buff_info::get_act() const -> buff_action {
	return m_act;
}

auto buff_info::get_act_value() const -> buff_skill_value {
	return m_act_value;
}

auto buff_info::get_act_interval() const -> duration {
	return m_act_interval;
}

auto buff_info::is_movement_affecting() const -> bool {
	return m_is_movement_affecting;
}

auto buff_info::has_map_info() const -> bool {
	return m_has_map_info;
}

auto buff_info::has_act() const -> bool {
	return m_act != buff_action::none;
}

auto buff_info::with_value(buff_skill_value value) const -> buff_info {
	buff_info result{*this};
	result.m_value = value;
	return result;
}

auto buff_info::with_map_info(buff_map_info value) const -> buff_info {
	buff_info result{*this};
	result.m_map_info = value;
	result.m_has_map_info = true;
	return result;
}

auto buff_info::with_chance(int16_t value) const -> buff_info {
	buff_info result{*this};
	result.m_chance = value;
	return result;
}

auto buff_info::with_predefined_value(int16_t value) const -> buff_info {
	buff_info result{*this};
	result.m_value = buff_skill_value::predefined;
	result.m_predefined_value = value;
	return result;
}

auto buff_info::with_act(buff_action action, buff_skill_value value, duration interval) const -> buff_info {
	buff_info result{*this};
	result.m_act = action;
	result.m_act_value = value;
	result.m_act_interval = interval;
	return result;
}

auto buff_info::get_buff_byte(uint8_t bit_position) -> uint8_t {
	return (bit_position - 1) / 8;
}

auto buff_info::get_buff_type(uint8_t bit_position) -> uint8_t {
	return 1U << ((bit_position - 1) % 8);
}

}
}
}