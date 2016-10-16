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

#include "common/data/type/buff_action.hpp"
#include "common/data/type/buff_map_info.hpp"
#include "common/data/type/buff_skill_value.hpp"
#include "common/types.hpp"
#include "common/util/hash_combine.hpp"

namespace vana {
	namespace data {
		namespace type {
			class buff_info {
			public:
				// Provided for default-construction
				// Not intended to be used directly
				buff_info() = default;

				static auto from_player_only(uint8_t bit_position) -> buff_info;
				static auto from_player_only(uint8_t bit_position, buff_skill_value value) -> buff_info;
				static auto from_player_only(uint8_t bit_position, int16_t predefined_value) -> buff_info;
				static auto from_player_only_movement(uint8_t bit_position) -> buff_info;
				static auto from_player_only_movement(uint8_t bit_position, buff_skill_value value) -> buff_info;
				static auto from_player_only_movement(uint8_t bit_position, int16_t predefined_value) -> buff_info;
				static auto from_map_no_movement(uint8_t bit_position, buff_map_info map_value) -> buff_info;
				static auto from_map_no_movement(uint8_t bit_position, buff_skill_value value, buff_map_info map_value) -> buff_info;
				static auto from_map_no_movement(uint8_t bit_position, int16_t predefined_value, buff_map_info map_value) -> buff_info;
				static auto from_map_movement(uint8_t bit_position, buff_map_info map_value) -> buff_info;
				static auto from_map_movement(uint8_t bit_position, buff_skill_value value, buff_map_info map_value) -> buff_info;
				static auto from_map_movement(uint8_t bit_position, int16_t predefined_value, buff_map_info map_value) -> buff_info;

				auto get_predefined_value() const -> int16_t;
				auto get_map_info() const -> buff_map_info;
				auto get_value() const -> buff_skill_value;
				auto get_chance() const -> int16_t;
				auto get_bit_position() const -> uint8_t;
				auto get_buff_byte() const -> uint8_t;
				auto get_buff_type() const -> uint8_t;
				auto get_act() const -> buff_action;
				auto get_act_value() const -> buff_skill_value;
				auto get_act_interval() const -> duration;
				auto is_movement_affecting() const -> bool;
				auto has_map_info() const -> bool;
				auto has_act() const -> bool;
				auto with_value(buff_skill_value value) const -> buff_info;
				auto with_map_info(buff_map_info value) const -> buff_info;
				auto with_chance(int16_t chance) const -> buff_info;
				auto with_predefined_value(int16_t value) const -> buff_info;
				auto with_act(buff_action action, buff_skill_value value, duration interval) const -> buff_info;
			private:
				auto static get_buff_byte(uint8_t bit_position) -> uint8_t;
				auto static get_buff_type(uint8_t bit_position) -> uint8_t;

				friend auto operator ==(const buff_info &a, const buff_info &b) -> bool;
				friend auto operator !=(const buff_info &a, const buff_info &b) -> bool;
				friend auto operator <(const buff_info &a, const buff_info &b) -> bool;
				friend auto operator <=(const buff_info &a, const buff_info &b) -> bool;
				friend auto operator >(const buff_info &a, const buff_info &b) -> bool;
				friend auto operator >=(const buff_info &a, const buff_info &b) -> bool;
				friend auto operator ==(uint8_t a, const buff_info &b) -> bool;
				friend auto operator !=(uint8_t a, const buff_info &b) -> bool;
				friend auto operator <(uint8_t a, const buff_info &b) -> bool;
				friend auto operator <=(uint8_t a, const buff_info &b) -> bool;
				friend auto operator >(uint8_t a, const buff_info &b) -> bool;
				friend auto operator >=(uint8_t a, const buff_info &b) -> bool;
				friend auto operator ==(const buff_info &a, uint8_t b) -> bool;
				friend auto operator !=(const buff_info &a, uint8_t b) -> bool;
				friend auto operator <(const buff_info &a, uint8_t b) -> bool;
				friend auto operator <=(const buff_info &a, uint8_t b) -> bool;
				friend auto operator >(const buff_info &a, uint8_t b) -> bool;
				friend auto operator >=(const buff_info &a, uint8_t b) -> bool;

				bool m_is_movement_affecting = false;
				bool m_has_map_info = false;
				uint8_t m_bit_position = 0;
				int16_t m_predefined_value = 0;
				int16_t m_chance = -1;
				duration m_act_interval = seconds{0};
				buff_action m_act = buff_action::none;
				buff_skill_value m_act_value = buff_skill_value::x;
				buff_skill_value m_value = buff_skill_value::predefined;
				buff_map_info m_map_info;
			};

			inline
			auto operator ==(const buff_info &a, const buff_info &b) -> bool {
				return a.m_bit_position == b.m_bit_position;
			}

			inline
			auto operator !=(const buff_info &a, const buff_info &b) -> bool {
				return !(a == b);
			}

			inline
			auto operator >(const buff_info &a, const buff_info &b) -> bool {
				return a.m_bit_position > b.m_bit_position;
			}

			inline
			auto operator >=(const buff_info &a, const buff_info &b) -> bool {
				return a.m_bit_position >= b.m_bit_position;
			}

			inline
			auto operator <(const buff_info &a, const buff_info &b) -> bool {
				return a.m_bit_position < b.m_bit_position;
			}

			inline
			auto operator <=(const buff_info &a, const buff_info &b) -> bool {
				return a.m_bit_position <= b.m_bit_position;
			}

			inline
			auto operator ==(uint8_t a, const buff_info &b) -> bool {
				return a == b.m_bit_position;
			}

			inline
			auto operator !=(uint8_t a, const buff_info &b) -> bool {
				return !(a == b);
			}

			inline
			auto operator >(uint8_t a, const buff_info &b) -> bool {
				return a > b.m_bit_position;
			}

			inline
			auto operator >=(uint8_t a, const buff_info &b) -> bool {
				return a >= b.m_bit_position;
			}

			inline
			auto operator <(uint8_t a, const buff_info &b) -> bool {
				return a < b.m_bit_position;
			}

			inline
			auto operator <=(uint8_t a, const buff_info &b) -> bool {
				return a <= b.m_bit_position;
			}

			inline
			auto operator ==(const buff_info &a, uint8_t b) -> bool {
				return a.m_bit_position == b;
			}

			inline
			auto operator !=(const buff_info &a, uint8_t b) -> bool {
				return !(a == b);
			}

			inline
			auto operator >(const buff_info &a, uint8_t b) -> bool {
				return a.m_bit_position > b;
			}

			inline
			auto operator >=(const buff_info &a, uint8_t b) -> bool {
				return a.m_bit_position >= b;
			}

			inline
			auto operator <(const buff_info &a, uint8_t b) -> bool {
				return a.m_bit_position < b;
			}

			inline
			auto operator <=(const buff_info &a, uint8_t b) -> bool {
				return a.m_bit_position <= b;
			}
		}
	}
}

namespace std {
	template <>
	struct hash<vana::data::type::buff_info> {
		auto operator()(const vana::data::type::buff_info &v) const -> size_t {
			return vana::util::hash_combinator(v.get_bit_position());
		}
	};
}