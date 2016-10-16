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
#include "maple_tvs.hpp"
#include "common/timer/timer.hpp"
#include "common/util/time.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maple_tv_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet_helper.hpp"
#include "channel_server/smsg_header.hpp"
#include <functional>

namespace vana {
namespace channel_server {

auto maple_tvs::add_map(map *map) -> void {
	m_maps[map->get_id()] = map;
}

auto maple_tvs::add_message(ref_ptr<player> sender, ref_ptr<player> receiver, const string &msg, const string &msg2, const string &msg3, const string &msg4, const string &msg5, game_item_id megaphone_id, int32_t time) -> void {
	maple_tv_message message;
	message.has_receiver = (receiver != nullptr);
	message.megaphone_id = megaphone_id;
	message.sender_id = sender->get_id();
	message.time = time;
	message.counter = get_counter();
	message.msg1 = msg;
	message.msg2 = msg2;
	message.msg3 = msg3;
	message.msg4 = msg4;
	message.msg5 = msg5;
	message.send_display.add_buffer(packets::helpers::add_player_display(sender)); // We need to save the packet since it gets buffered and there's no guarantee the player will exist later
	message.send_name = sender->get_name();
	if (receiver != nullptr) {
		message.recv_display.add_buffer(packets::helpers::add_player_display(receiver));
		message.recv_name = receiver->get_name();
	}

	m_buffer.push_back(message);

	if (!m_has_message) {
		// First element pushed
		parse_buffer();
		m_has_message = true;
	}
}

auto maple_tvs::parse_buffer() -> void {
	if (m_buffer.size() > 0) {
		maple_tv_message message = m_buffer.front();
		m_buffer.pop_front();

		send(packets::maple_tv::show_message(message, get_message_time()));

		m_current_message = message;

		vana::timer::id id{timer::type::maple_tv_timer, message.sender_id, message.counter};
		vana::timer::timer::create(
			[this](const time_point &now) { this->parse_buffer(); },
			id, get_timers(), seconds{message.time});
	}
	else {
		m_has_message = false;
		send(packets::maple_tv::end_display());
	}
}

auto maple_tvs::send(const packet_builder &builder) -> void {
	for (const auto &kvp : m_maps) {
		kvp.second->send(builder);
	}
}

auto maple_tvs::get_message_time() const -> seconds {
	vana::timer::id id{timer::type::maple_tv_timer, m_current_message.sender_id, m_current_message.counter};
	return get_timers()->get_remaining_time<seconds>(id);
}

auto maple_tvs::is_maple_tv_map(game_map_id id) const -> bool {
	return m_maps.find(id) != std::end(m_maps);
}

auto maple_tvs::has_message() const -> bool {
	return m_has_message;
}

auto maple_tvs::get_counter() -> uint32_t {
	return ++m_counter;
}

auto maple_tvs::get_current_message() const -> const maple_tv_message & {
	return m_current_message;
}

}
}