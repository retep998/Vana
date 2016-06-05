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
#include "SyncPacket.hpp"
#include "common/inter_header.hpp"
#include "common/inter_helper.hpp"
#include "common/rates_config.hpp"
#include "common/session.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Party.hpp"
#include "channel_server/Player.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace interserver {

PACKET_IMPL(config::scrolling_header, const string &message) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::config)
		.add<protocol_sync>(sync::config::scrolling_header)
		.add<string>(message);
	return builder;
}

PACKET_IMPL(config::reset_rates, int32_t flags) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::config)
		.add<protocol_sync>(sync::config::rate_reset)
		.add<int32_t>(flags);
	return builder;
}

PACKET_IMPL(config::modify_rates, const rates_config &rates) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::config)
		.add<protocol_sync>(sync::config::rate_set)
		.add<rates_config>(rates);
	return builder;
}

PACKET_IMPL(player::update_player, const player_data &player, protocol_update_bits flags) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::update_player)
		.add<protocol_update_bits>(flags)
		.add<game_player_id>(player.id);

	if (flags & sync::player::update_bits::full) {
		builder.add<player_data>(player);
	}
	else {
		if (flags & sync::player::update_bits::level) {
			builder.add<int16_t>(player.level.get());
		}
		if (flags & sync::player::update_bits::job) {
			builder.add<int16_t>(player.job.get());
		}
		if (flags & sync::player::update_bits::map) {
			builder.add<int32_t>(player.map.get());
		}
		if (flags & sync::player::update_bits::channel) {
			builder.add<game_channel_id>(player.channel.get());
		}
		if (flags & sync::player::update_bits::ip) {
			builder.add<ip>(player.ip);
		}
		if (flags & sync::player::update_bits::cash) {
			builder.add<bool>(player.cash_shop);
		}
		if (flags & sync::player::update_bits::mts) {
			builder.add<bool>(player.mts);
		}
	}
	return builder;
}

PACKET_IMPL(player::change_channel, ref_ptr<vana::channel_server::player> info, game_channel_id channel) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::change_channel_request)
		.add<game_player_id>(info->get_id())
		.add<game_channel_id>(channel)
		.add<ip>(info->get_ip().get())
		.add_buffer(info->get_transfer_packet());

	return builder;
}

PACKET_IMPL(player::connect, const player_data &player, bool first_connect) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::connect)
		.add<bool>(first_connect)
		.add<game_player_id>(player.id);

	if (first_connect) {
		builder.add<player_data>(player);
	}
	else {
		builder
			.add<game_map_id>(player.map.get())
			.add<game_channel_id>(player.channel.get())
			.add<ip>(player.ip);
	}

	return builder;
}

PACKET_IMPL(player::disconnect, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::disconnect)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(player::connectable_established, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::change_channel_go)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(party::sync, int8_t type, game_player_id player_id, int32_t target) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::party)
		.add<int8_t>(type)
		.add<game_player_id>(player_id);

	if (target != 0) {
		builder.add<int32_t>(target);
	}
	return builder;
}

PACKET_IMPL(buddy::buddy_invite, game_player_id inviter_id, game_player_id invitee_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::buddy)
		.add<protocol_sync>(sync::buddy::invite)
		.add<game_player_id>(inviter_id)
		.add<game_player_id>(invitee_id);
	return builder;
}

PACKET_IMPL(buddy::accept_buddy_invite, game_player_id invitee_id, game_player_id inviter_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::buddy)
		.add<protocol_sync>(sync::buddy::accept_invite)
		.add<game_player_id>(invitee_id)
		.add<game_player_id>(inviter_id);
	return builder;
}

PACKET_IMPL(buddy::remove_buddy, game_player_id list_owner_id, game_player_id removal_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::buddy)
		.add<protocol_sync>(sync::buddy::remove_buddy)
		.add<game_player_id>(list_owner_id)
		.add<game_player_id>(removal_id);
	return builder;
}

PACKET_IMPL(buddy::readd_buddy, game_player_id list_owner_id, game_player_id buddy_id) {
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