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
#include "sync_packet.hpp"
#include "common/config/rates.hpp"
#include "common/constant/map.hpp"
#include "common/inter_header.hpp"
#include "common/inter_helper.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "common/util/time.hpp"
#include "world_server/channel.hpp"
#include "world_server/channels.hpp"
#include "world_server/player_data_provider.hpp"
#include "world_server/world_server_accepted_session.hpp"

namespace vana {
namespace world_server {
namespace packets {
namespace interserver {

PACKET_IMPL(send_sync_data, function<void(packet_builder &)> build_sync_data) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::channel_start);
	build_sync_data(builder);
	return builder;
}

PACKET_IMPL(config::scrolling_header, const string &message) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::config)
		.add<protocol_sync>(sync::config::scrolling_header)
		.add<string>(message);
	return builder;
}

PACKET_IMPL(config::set_rates, const vana::config::rates &rates) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::config)
		.add<protocol_sync>(sync::config::rate_set)
		.add<vana::config::rates>(rates);
	return builder;
}

PACKET_IMPL(player::new_connectable, game_player_id player_id, const ip &ip_value, packet_reader &buffer) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::new_connectable)
		.add<game_player_id>(player_id)
		.add<ip>(ip_value)
		.add<uint16_t>(static_cast<uint16_t>(buffer.get_buffer_length()))
		.add_buffer(buffer);
	return builder;
}

PACKET_IMPL(player::delete_connectable, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::delete_connectable)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(player::player_change_channel, game_player_id player_id, game_channel_id channel_id, const ip &ip_value, connection_port port) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::change_channel_go)
		.add<game_player_id>(player_id)
		.add<game_channel_id>(channel_id)
		.add<ip>(ip_value)
		.add<connection_port>(port);
	return builder;
}

PACKET_IMPL(player::update_player, const player_data &data, protocol_update_bits flags) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::update_player)
		.add<game_player_id>(data.id)
		.add<protocol_update_bits>(flags);

	if (flags & sync::player::update_bits::full) {
		builder.add<player_data>(data);
	}
	else {
		if (flags & sync::player::update_bits::level) {
			builder.add<game_player_level>(data.level.get());
		}
		if (flags & sync::player::update_bits::job) {
			builder.add<game_job_id>(data.job.get());
		}
		if (flags & sync::player::update_bits::map) {
			builder.add<game_map_id>(data.map.get());
		}
		if (flags & sync::player::update_bits::transfer) {
			builder.add<bool>(data.transferring);
		}
		if (flags & sync::player::update_bits::channel) {
			builder.add<optional<game_channel_id>>(data.channel);
		}
		if (flags & sync::player::update_bits::ip) {
			builder.add<ip>(data.ip);
		}
		if (flags & sync::player::update_bits::cash) {
			builder.add<bool>(data.cash_shop);
		}
		if (flags & sync::player::update_bits::mts) {
			builder.add<bool>(data.mts);
		}
	}
	return builder;
}

PACKET_IMPL(player::character_created, const player_data &data) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::character_created)
		.add<player_data>(data);
	return builder;
}

PACKET_IMPL(player::character_deleted, game_player_id id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::character_deleted)
		.add<game_player_id>(id);
	return builder;
}

PACKET_IMPL(party::remove_party_member, game_party_id party_id, game_player_id player_id, bool kicked) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::party)
		.add<protocol_sync>(sync::party::remove_member)
		.add<game_party_id>(party_id)
		.add<game_player_id>(player_id)
		.add<bool>(kicked);
	return builder;
}

PACKET_IMPL(party::add_party_member, game_party_id party_id, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::party)
		.add<protocol_sync>(sync::party::add_member)
		.add<game_party_id>(party_id)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(party::new_party_leader, game_party_id party_id, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::party)
		.add<protocol_sync>(sync::party::switch_leader)
		.add<game_party_id>(party_id)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(party::create_party, game_party_id party_id, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::party)
		.add<protocol_sync>(sync::party::create)
		.add<game_party_id>(party_id)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(party::disband_party, game_party_id party_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::party)
		.add<protocol_sync>(sync::party::disband)
		.add<game_party_id>(party_id);
	return builder;
}

PACKET_IMPL(buddy::send_buddy_invite, game_player_id invitee_id, game_player_id inviter_id, const string &name) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::buddy)
		.add<protocol_sync>(sync::buddy::invite)
		.add<game_player_id>(inviter_id)
		.add<game_player_id>(invitee_id)
		.add<string>(name);
	return builder;
}

PACKET_IMPL(buddy::send_accept_buddy_invite, game_player_id invitee_id, game_player_id inviter_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::buddy)
		.add<protocol_sync>(sync::buddy::accept_invite)
		.add<game_player_id>(inviter_id)
		.add<game_player_id>(invitee_id);
	return builder;
}

PACKET_IMPL(buddy::send_buddy_removal, game_player_id list_owner_id, game_player_id removal_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::buddy)
		.add<protocol_sync>(sync::buddy::remove_buddy)
		.add<game_player_id>(list_owner_id)
		.add<game_player_id>(removal_id);
	return builder;
}

PACKET_IMPL(buddy::send_readd_buddy, game_player_id list_owner_id, game_player_id buddy_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::buddy)
		.add<protocol_sync>(sync::buddy::readd_buddy)
		.add<game_player_id>(list_owner_id)
		.add<game_player_id>(buddy_id);
	return builder;
}

}
}
}
}