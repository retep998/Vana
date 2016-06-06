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

#include "common/buff.hpp"
#include "common/buff_info.hpp"
#include "common/buff_source.hpp"
#include "common/packet_builder.hpp"
#include "common/skill_constants.hpp"
#include "common/types.hpp"
#include <array>
#include <vector>

namespace vana {
	namespace channel_server {
		class player;

		enum class buff_packet_value_type {
			value,
			packet,
			special_packet,
		};

		struct buff_packet_value {
			static buff_packet_value from_value(uint8_t size, int64_t value) {
				buff_packet_value ret;
				ret.value_size = size;
				ret.value = value;
				ret.type = buff_packet_value_type::value;
				return ret;
			}

			static buff_packet_value from_packet(packet_builder builder) {
				buff_packet_value ret;
				ret.builder = builder;
				ret.type = buff_packet_value_type::packet;
				return ret;
			}

			static buff_packet_value from_special_packet(packet_builder builder) {
				buff_packet_value ret;
				ret.builder = builder;
				ret.type = buff_packet_value_type::special_packet;
				return ret;
			}

			uint8_t value_size = 0;
			int64_t value = 0;
			int32_t time = 0;
			buff_packet_value_type type;
			packet_builder builder;
		private:
			buff_packet_value() = default;
		};

		struct buff_packet_structure {
			buff_packet_structure()
			{
				std::fill(std::begin(types), std::end(types), 0);
			}

			bool any_movement_buffs = false;
			buff_array types;
			vector<buff_packet_value> values;
		};

		struct buff_packet_values {
			buff_packet_structure player;
			optional<buff_packet_structure> map;
			milliseconds delay = milliseconds{0};
		};

		namespace buffs {
			auto add_buff(ref_ptr<player> player, game_item_id item_id, const seconds &time) -> result;
			auto add_buff(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level, int16_t added_info, game_map_object map_mob_id = 0) -> result;
			auto add_buff(ref_ptr<player> player, game_mob_skill_id skill_id, game_mob_skill_level level, milliseconds delay) -> result;
			auto end_buff(ref_ptr<player> player, const buff_source &source, bool from_timer = false) -> void;
			auto preprocess_buff(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level, const seconds &time) -> buff;
			auto preprocess_buff(ref_ptr<player> player, game_item_id item_id, const seconds &time) -> buff;
			auto preprocess_buff(ref_ptr<player> player, game_mob_skill_id skill_id, game_mob_skill_level level, const seconds &time) -> buff;
			auto preprocess_buff(ref_ptr<player> player, const buff_source &source, const seconds &time) -> buff;
			auto preprocess_buff(ref_ptr<player> player, const buff_source &source, const seconds &time, const buff &buff_value) -> buff;
			auto preprocess_buff(const buff &buff_value, const vector<uint8_t> &bit_positions_to_include) -> buff;
			auto convert_to_packet_types(const buff &buff) -> buff_packet_values;
			auto convert_to_packet(ref_ptr<player> player, const buff_source &source, const seconds &time, const buff &buff) -> buff_packet_values;
			auto buff_may_apply(ref_ptr<player> player, const buff_source &source, const seconds &time, const buff_info &buff) -> bool;
			auto get_value(ref_ptr<player> player, const buff_source &source, const seconds &time, const buff_info &buff) -> buff_packet_value;
			auto get_value(ref_ptr<player> player, const buff_source &source, const seconds &time, uint8_t bit_position, const buff_map_info &buff) -> buff_packet_value;
			auto get_value(ref_ptr<player> player, const buff_source &source, const seconds &time, uint8_t bit_position, buff_skill_value value, uint8_t buff_value_size) -> buff_packet_value;
		}
	}
}