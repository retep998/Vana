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
#include "player_summons.hpp"
#include "common/algorithm.hpp"
#include "common/game_constants.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/packet_reader.hpp"
#include "common/timer.hpp"
#include "common/time_utilities.hpp"
#include "channel_server/map.hpp"
#include "channel_server/player.hpp"
#include "channel_server/summon.hpp"
#include "channel_server/summon_handler.hpp"
#include "channel_server/summons_packet.hpp"
#include <functional>

namespace vana {
namespace channel_server {

player_summons::player_summons(player *player) :
	m_player{player}
{
}

auto player_summons::add_summon(summon *summon, seconds time) -> void {
	game_summon_id summon_id = summon->get_id();
	vana::timer::id id{timer_type::buff_timer, summon_id, 1};
	vana::timer::timer::create(
		[this, summon_id](const time_point &now) {
			summon_handler::remove_summon(
				ref_ptr<player>{m_player},
				summon_id,
				false,
				summon_messages::out_of_time,
				true);
		},
		id,
		m_player->get_timer_container(),
		time);

	m_summons.push_back(summon);
}

auto player_summons::remove_summon(game_summon_id summon_id, bool from_timer) -> void {
	summon *value = get_summon(summon_id);
	if (value != nullptr) {
		if (!from_timer) {
			vana::timer::id id{timer_type::buff_timer, summon_id, 1};
			m_player->get_timer_container()->remove_timer(id);
		}
		ext::remove_element(m_summons, value);
		delete value;
		summon_handler::g_summon_ids.release(summon_id);
	}
}

auto player_summons::get_summon(game_summon_id summon_id) -> summon * {
	for (auto &value : m_summons) {
		if (value->get_id() == summon_id) {
			return value;
		}
	}
	return nullptr;
}

auto player_summons::for_each(function<void(summon *)> func) -> void {
	auto copy = m_summons;
	for (auto &summon : copy) {
		func(summon);
	}
}

auto player_summons::changed_map() -> void {
	auto copy = m_summons;
	for (auto &summon : copy) {
		if (summon->get_movement_type() == summon::fixed) {
			summon_handler::remove_summon(
				ref_ptr<player>{m_player},
				summon->get_id(),
				false,
				summon_messages::none);
		}
	}
}

auto player_summons::get_summon_time_remaining(game_summon_id summon_id) const -> seconds {
	vana::timer::id id{timer_type::buff_timer, summon_id, 1};
	return m_player->get_timer_container()->get_remaining_time<seconds>(id);
}

auto player_summons::get_transfer_packet() const -> packet_builder {
	packet_builder builder;
	builder.add<uint8_t>(static_cast<uint8_t>(m_summons.size()));
	if (m_summons.size() > 0) {
		for (const auto &summon : m_summons) {
			if (summon->get_movement_type() == summon::fixed) {
				continue;
			}
			builder.add<game_skill_id>(summon->get_skill_id());
			builder.add<seconds>(get_summon_time_remaining(summon->get_id()));
			builder.add<game_skill_level>(summon->get_skill_level());
		}
	}
	return builder;
}

auto player_summons::parse_transfer_packet(packet_reader &reader) -> void {
	uint8_t size = reader.get<uint8_t>();
	if (size > 0) {
		for (uint8_t i = 0; i < size; ++i) {
			game_skill_id skill_id = reader.get<game_skill_id>();
			seconds time_left = reader.get<seconds>();
			game_skill_level level = reader.get<game_skill_level>();

			summon *value = new summon{summon_handler::g_summon_ids.lease(), skill_id, level, m_player->is_facing_left(), m_player->get_pos()};
			value->set_pos(m_player->get_pos());
			add_summon(value, time_left);
			m_player->send_map(packets::show_summon(m_player->get_id(), value, false));
		}
	}
}

}
}