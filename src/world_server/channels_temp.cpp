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
#include "channels_temp.hpp"
#include "common/packet_builder.hpp"
#include "common/session.hpp"
#include "world_server/channel_temp.hpp"
#include "world_server/login_server_connect_packet.hpp"
#include "world_server/world_server.hpp"
#include "world_server/world_server_accepted_session.hpp"

namespace vana {
namespace world_server {

auto channels::register_channel(ref_ptr<world_server_accepted_session> session, game_channel_id channel_id, const ip &channel_ip, const ip_matrix &ext_ip, connection_port port) -> void {
	ref_ptr<channel> chan = make_ref_ptr<channel>(session, channel_id, port);
	chan->set_external_ip_information(channel_ip, ext_ip);
	m_channels[channel_id] = chan;
}

auto channels::remove_channel(game_channel_id channel) -> void {
	m_channels.erase(channel);
}

auto channels::get_channel(game_channel_id num) -> channel * {
	auto kvp = m_channels.find(num);
	return kvp != std::end(m_channels) ? kvp->second.get() : nullptr;
}

auto channels::send(game_channel_id channel_id, const packet_builder &builder) -> void {
	if (channel *channel = get_channel(channel_id)) {
		channel->send(builder);
	}
}

auto channels::send(const vector<game_channel_id> &channels, const packet_builder &builder) -> void {
	for (const auto &channel_id : channels) {
		send(channel_id, builder);
	}
}

auto channels::send(const packet_builder &builder) -> void {
	for (const auto &kvp : m_channels) {
		send(kvp.first, builder);
	}
}

auto channels::increase_population(game_channel_id channel) -> void {
	world_server::get_instance().send_login(packets::update_channel_pop(channel, get_channel(channel)->increase_players()));
}

auto channels::decrease_population(game_channel_id channel) -> void {
	world_server::get_instance().send_login(packets::update_channel_pop(channel, get_channel(channel)->decrease_players()));
}

auto channels::get_first_available_channel_id() -> game_channel_id {
	game_channel_id channel_id = -1;
	game_channel_id max = world_server::get_instance().get_config().max_channels;
	for (game_channel_id i = 0; i < max; ++i) {
		if (m_channels.find(i) == std::end(m_channels)) {
			channel_id = i;
			break;
		}
	}
	return channel_id;
}

auto channels::disconnect() -> void {
	auto copy = m_channels;
	for (const auto &kvp : copy) {
		if (auto ref = kvp.second.get()) {
			ref->disconnect();
		}
	}
}

}
}