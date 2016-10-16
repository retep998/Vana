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
#include "player_data_provider.hpp"
#include "common/algorithm.hpp"
#include "common/constant/party.hpp"
#include "common/data/initialize.hpp"
#include "common/inter_header.hpp"
#include "common/inter_helper.hpp"
#include "common/io/database.hpp"
#include "common/packet_wrapper.hpp"
#include "common/util/string.hpp"
#include "world_server/channel.hpp"
#include "world_server/channels.hpp"
#include "world_server/sync_handler.hpp"
#include "world_server/sync_packet.hpp"
#include "world_server/world_server.hpp"
#include "world_server/world_server_accepted_session.hpp"
#include "world_server/world_server_accept_packet.hpp"
#include <iomanip>
#include <iostream>
#include <memory>

namespace vana {
namespace world_server {

player_data_provider::player_data_provider() :
	m_party_ids{1, 100000}
{
}

auto player_data_provider::load_data() -> void {
	game_world_id world_id = world_server::get_instance().get_world_id();
	load_players(world_id);
}

auto player_data_provider::get_channel_connect_packet(packet_builder &builder) -> void {
	builder.add<uint32_t>(m_players.size());
	for (const auto &kvp : m_players) {
		builder.add<player_data>(kvp.second);
	}

	builder.add<uint32_t>(m_parties.size());
	for (const auto &kvp : m_parties) {
		builder.add<party_data>(kvp.second);
	}
}

auto player_data_provider::load_players(game_world_id world_id) -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Players... ";

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT c.character_id, c.name "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.world_id = :world",
		soci::use(world_id, "world"));

	for (const auto &row : rs) {
		player_data data;
		data.id = row.get<game_player_id>("character_id");
		data.name = row.get<string>("name");
		add_player(data);
	}

	std::cout << "DONE" << std::endl;
}

auto player_data_provider::load_player(game_player_id player_id) -> void {
	if (m_players.find(player_id) != std::end(m_players)) {
		return;
	}

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT c.character_id, c.name "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.character_id = :char",
		soci::use(player_id, "char"));

	const auto &row = *rs.begin();
	player_data data;
	data.id = row.get<game_player_id>("character_id");
	data.name = row.get<string>("name");
	add_player(data);
}

auto player_data_provider::add_player(const player_data &data) -> void {
	m_players[data.id] = data;
	auto &element = m_players[data.id];
	m_players_by_name[data.name] = &element;
}

auto player_data_provider::send_sync(const packet_builder &builder) const -> void {
	world_server::get_instance().get_channels().send(builder);
}

auto player_data_provider::channel_disconnect(game_channel_id channel) -> void {
	for (auto &kvp : m_players) {
		auto &player = kvp.second;
		if (player.channel == channel) {
			player.channel.reset();
			remove_pending_player(player.id);
		}
	}
}

auto player_data_provider::send(game_player_id player_id, const packet_builder &builder) -> void {
	auto &data = m_players[player_id];
	if (!data.channel.is_initialized()) {
		return;
	}

	world_server::get_instance().get_channels().send(data.channel.get(), vana::packets::prepend(
		builder, [&](packet_builder &header) {
			header
				.add<packet_header>(IMSG_TO_PLAYER)
				.add<game_player_id>(player_id);
		}));
}

auto player_data_provider::send(const vector<game_player_id> &player_ids, const packet_builder &builder) -> void {
	hash_map<game_channel_id, vector<game_player_id>> send_targets;

	for (const auto &player_id : player_ids) {
		auto &data = m_players[player_id];
		if (!data.channel.is_initialized()) {
			continue;
		}

		auto kvp = send_targets.find(data.channel.get());
		if (kvp == std::end(send_targets)) {
			kvp = send_targets.emplace(data.channel.get(), vector<game_player_id>{}).first;
		}

		kvp->second.push_back(data.id);
	}

	for (const auto &kvp : send_targets) {
		world_server::get_instance().get_channels().send(kvp.first, vana::packets::prepend(
			builder, [&](packet_builder &header) {
				header
					.add<packet_header>(IMSG_TO_PLAYER_LIST)
					.add<vector<game_player_id>>(kvp.second);
			}));
	}
}

auto player_data_provider::send(const packet_builder &builder) -> void {
	hash_map<game_channel_id, vector<game_player_id>> send_targets;

	for (const auto &iter : m_players) {
		auto &data = iter.second;
		if (!data.channel.is_initialized()) {
			continue;
		}
		auto kvp = send_targets.find(data.channel.get());
		if (kvp == std::end(send_targets)) {
			kvp = send_targets.emplace(data.channel.get(), vector<game_player_id>{}).first;
		}

		kvp->second.push_back(data.id);
	}

	for (const auto &kvp : send_targets) {
		world_server::get_instance().get_channels().send(kvp.first, vana::packets::prepend(
			builder, [&](packet_builder &header) {
				header
					.add<packet_header>(IMSG_TO_PLAYER_LIST)
					.add<vector<game_player_id>>(kvp.second);
			}));
	}
}

// Handlers
auto player_data_provider::handle_sync(ref_ptr<world_server_accepted_session> session, protocol_sync type, packet_reader &reader) -> void {
	switch (type) {
		case sync::sync_types::player: handle_player_sync(session, reader); break;
		case sync::sync_types::party: handle_party_sync(reader); break;
		case sync::sync_types::buddy: handle_buddy_sync(reader); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "sync type");
	}
}

auto player_data_provider::handle_sync(ref_ptr<login_server_session> session, protocol_sync type, packet_reader &reader) -> void {
	switch (type) {
		case sync::sync_types::player: handle_player_sync(session, reader); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "sync type");
	}
}

auto player_data_provider::handle_player_sync(ref_ptr<world_server_accepted_session> session, packet_reader &reader) -> void {
	switch (reader.get<protocol_sync>()) {
		case sync::player::change_channel_request: handle_change_channel_request(session, reader); break;
		case sync::player::change_channel_go: handle_change_channel(reader); break;
		case sync::player::connect: handle_player_connect(session->get_channel(), reader); break;
		case sync::player::disconnect: handle_player_disconnect(session->get_channel(), reader); break;
		case sync::player::update_player: handle_player_update(reader); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "player_sync type");
	}
}

auto player_data_provider::handle_player_sync(ref_ptr<login_server_session> session, packet_reader &reader) -> void {
	switch (reader.get<protocol_sync>()) {
		case sync::player::character_created: handle_character_created(reader); break;
		case sync::player::character_deleted: handle_character_deleted(reader); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "player_sync type");
	}
}

auto player_data_provider::handle_party_sync(packet_reader &reader) -> void {
	protocol_sync type = reader.get<protocol_sync>();
	game_player_id player_id = reader.get<game_player_id>();
	switch (type) {
		case party_actions::create: handle_create_party(player_id); break;
		case party_actions::leave: handle_party_leave(player_id); break;
		case party_actions::expel: handle_party_remove(player_id, reader.get<game_player_id>()); break;
		case party_actions::join: handle_party_add(player_id, reader.get<game_party_id>()); break;
		case party_actions::set_leader: handle_party_transfer(player_id, reader.get<game_player_id>()); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "party_sync type");
	}
}

auto player_data_provider::handle_buddy_sync(packet_reader &reader) -> void {
	switch (reader.get<protocol_sync>()) {
		case sync::buddy::invite: buddy_invite(reader); break;
		case sync::buddy::accept_invite: accept_buddy_invite(reader); break;
		case sync::buddy::remove_buddy: remove_buddy(reader); break;
		case sync::buddy::readd_buddy: readd_buddy(reader); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "buddy_sync type");
	}
}

// Players
auto player_data_provider::remove_pending_player(game_player_id id) -> game_channel_id {
	game_channel_id channel = -1;
	auto kvp = m_channel_switches.find(id);
	if (kvp != std::end(m_channel_switches)) {
		channel = kvp->second;
		m_channel_switches.erase(kvp);
	}
	return channel;
}

auto player_data_provider::handle_player_update(packet_reader &reader) -> void {
	protocol_update_bits flags = reader.get<protocol_update_bits>();
	game_player_id player_id = reader.get<game_player_id>();
	auto &player = m_players[player_id];

	if (flags & sync::player::update_bits::full) {
		player_data data = reader.get<player_data>();
		player.copy_from(data);
	}
	else{
		if (flags & sync::player::update_bits::level) {
			player.level = reader.get<game_player_level>();
		}
		if (flags & sync::player::update_bits::job) {
			player.job = reader.get<game_job_id>();
		}
		if (flags & sync::player::update_bits::map) {
			player.map = reader.get<game_map_id>();
		}
		if (flags & sync::player::update_bits::channel) {
			player.channel = reader.get<game_channel_id>();
		}
		if (flags & sync::player::update_bits::ip) {
			player.ip = reader.get<ip>();
		}
		if (flags & sync::player::update_bits::cash) {
			player.cash_shop = reader.get<bool>();
		}
		if (flags & sync::player::update_bits::mts) {
			player.mts = reader.get<bool>();
		}
	}

	send_sync(packets::interserver::player::update_player(player, flags));
}

auto player_data_provider::handle_player_connect(game_channel_id channel, packet_reader &reader) -> void {
	bool first_connect = reader.get<bool>();
	game_player_id player_id = reader.get<game_player_id>();
	auto &player = m_players[player_id];

	if (first_connect) {
		player_data data = reader.get<player_data>();
		player.copy_from(data);
		player.initialized = true;
		player.transferring = false;
		send_sync(packets::interserver::player::update_player(player, sync::player::update_bits::full));
	}
	else {
		// Only the map/channel are relevant
		player.map = reader.get<game_map_id>();
		player.channel = reader.get<game_channel_id>();
		player.ip = reader.get<ip>();
		player.transferring = false;

		send_sync(packets::interserver::player::update_player(player, sync::player::update_bits::map | sync::player::update_bits::channel | sync::player::update_bits::transfer | sync::player::update_bits::ip));
	}

	world_server::get_instance().get_channels().increase_population(channel);
}

auto player_data_provider::handle_player_disconnect(game_channel_id channel, packet_reader &reader) -> void {
	game_player_id id = reader.get<game_player_id>();

	auto &player = m_players.find(id)->second;
	if (channel == -1 || player.channel == channel) {
		player.channel.reset();
		send_sync(packets::interserver::player::update_player(player, sync::player::update_bits::channel));
	}

	world_server::get_instance().get_channels().decrease_population(channel);

	game_channel_id old_channel = remove_pending_player(id);
	if (old_channel != -1) {
		world_server::get_instance().get_channels().send(old_channel, packets::interserver::player::delete_connectable(id));
		player.transferring = false;
		send_sync(packets::interserver::player::update_player(player, sync::player::update_bits::transfer));
	}
}

auto player_data_provider::handle_character_created(packet_reader &reader) -> void {
	game_player_id id = reader.get<game_player_id>();
	load_player(id);
	send_sync(packets::interserver::player::character_created(m_players[id]));
}

auto player_data_provider::handle_character_deleted(packet_reader &reader) -> void {
	game_player_id id = reader.get<game_player_id>();
	// TODO FIXME interserver must handle this state when a character is deleted
	// To my knowledge, the player takes up space in buddies, parties, AND guilds until the player is kicked from those or the social grouping disappears
	// This means we can't delete the info when the character is deleted and must also take care to preserve it for buddies/guilds
	// This design is not in place yet
	send_sync(packets::interserver::player::character_deleted(id));
}

auto player_data_provider::handle_change_channel_request(ref_ptr<world_server_accepted_session> session, packet_reader &reader) -> void {
	game_player_id player_id = reader.get<game_player_id>();
	channel *channel = world_server::get_instance().get_channels().get_channel(reader.get<game_channel_id>());
	ip ip_value{0};
	connection_port port = -1;
	if (channel != nullptr) {
		m_channel_switches[player_id] = channel->get_id();

		auto &player = m_players[player_id];
		player.transferring = true;
		send_sync(packets::interserver::player::update_player(player, sync::player::update_bits::transfer));

		ip_value = reader.get<ip>();
		channel->send(packets::interserver::player::new_connectable(player_id, ip_value, reader));
	}
	else {
		session->send(packets::interserver::player::player_change_channel(player_id, -1, ip_value, port));
	}
}

auto player_data_provider::handle_change_channel(packet_reader &reader) -> void {
	game_player_id player_id = reader.get<game_player_id>();

	auto &player = m_players.find(player_id)->second;
	channel *current_channel = world_server::get_instance().get_channels().get_channel(player.channel.get());
	if (current_channel == nullptr) {
		return;
	}

	game_channel_id channel_id = m_channel_switches[player_id];
	channel *destination_channel = world_server::get_instance().get_channels().get_channel(channel_id);
	ip ip{0};
	connection_port port = -1;
	if (destination_channel != nullptr) {
		ip = destination_channel->match_ip_to_subnet(player.ip);
		port = destination_channel->get_port();
	}

	current_channel->send(packets::interserver::player::player_change_channel(player_id, channel_id, ip, port));
	remove_pending_player(player_id);
}

// Parties
auto player_data_provider::handle_create_party(game_player_id player_id) -> void {
	auto &player = m_players[player_id];
	if (player.party > 0) {
		// Hacking
		return;
	}

	party_data party;
	party.id = m_party_ids.lease();
	party.leader = player.id;
	party.members.push_back(player.id);
	m_parties[party.id] = party;

	player.party = party.id;

	send_sync(packets::interserver::party::create_party(party.id, player_id));
}

auto player_data_provider::handle_party_leave(game_player_id player_id) -> void {
	auto &player = m_players[player_id];
	if (player.party == 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(player.party);
	if (kvp == std::end(m_parties)) {
		// Lag or something
		return;
	}

	auto &party = kvp->second;
	player.party = 0;

	if (party.leader == player_id) {
		for (const auto &member_id : party.members) {
			if (member_id != player_id) {
				auto &member = m_players[member_id];
				member.party = 0;
			}
		}
		send_sync(packets::interserver::party::disband_party(party.id));
		m_party_ids.release(party.id);
		m_parties.erase(kvp);
	}
	else {
		ext::remove_element(party.members, player_id);
		send_sync(packets::interserver::party::remove_party_member(party.id, player_id, false));
	}
}

auto player_data_provider::handle_party_remove(game_player_id player_id, game_player_id target_id) -> void {
	auto &player = m_players[player_id];
	if (player.party == 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(player.party);
	if (kvp == std::end(m_parties)) {
		// Lag or something
		return;
	}

	auto &party = kvp->second;
	if (party.leader != player_id) {
		// Hacking
		return;
	}

	auto &target = m_players[target_id];
	target.party = 0;
	ext::remove_element(party.members, target_id);
	send_sync(packets::interserver::party::remove_party_member(party.id, target_id, true));
}

auto player_data_provider::handle_party_add(game_player_id player_id, game_party_id party_id) -> void {
	auto &player = m_players[player_id];
	if (player.party != 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(party_id);
	if (kvp == std::end(m_parties)) {
		// Lag or hacking
		return;
	}

	auto &party = kvp->second;
	if (party.members.size() >= constant::party::max_members) {
		return;
	}

	player.party = party.id;
	party.members.push_back(player.id);
	send_sync(packets::interserver::party::add_party_member(party.id, player.id));
}

auto player_data_provider::handle_party_transfer(game_player_id player_id, game_player_id new_leader_id) -> void {
	auto &player = m_players[player_id];
	if (player.party == 0) {
		// Hacking
		return;
	}

	auto kvp = m_parties.find(player.party);
	if (kvp == std::end(m_parties)) {
		// Lag or something
		return;
	}

	auto &party = kvp->second;
	if (party.leader != player_id) {
		// Hacking
		return;
	}

	auto &target = m_players[new_leader_id];
	if (target.party != player.party) {
		// ???
		return;
	}

	party.leader = new_leader_id;
	send_sync(packets::interserver::party::new_party_leader(party.id, new_leader_id));
}

// Buddies
auto player_data_provider::buddy_invite(packet_reader &reader) -> void {
	game_player_id inviter_id = reader.get<game_player_id>();
	game_player_id invitee_id = reader.get<game_player_id>();
	auto &inviter = m_players[inviter_id];
	auto &invitee = m_players[invitee_id];

	if (!invitee.channel.is_initialized()) {
		// Make new pending buddy in the database
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		sql.once
			<< "INSERT INTO " << db.make_table(vana::table::buddylist_pending) << " "
			<< "VALUES (:invitee, :name, :inviter)",
			soci::use(invitee_id, "invitee"),
			soci::use(inviter.name, "name"),
			soci::use(inviter_id, "inviter");
	}
	else {
		world_server::get_instance().get_channels().send(invitee.channel.get(), packets::interserver::buddy::send_buddy_invite(invitee_id, inviter_id, inviter.name));
	}
}

auto player_data_provider::accept_buddy_invite(packet_reader &reader) -> void {
	game_player_id invitee_id = reader.get<game_player_id>();
	game_player_id inviter_id = reader.get<game_player_id>();
	auto &invitee = m_players[invitee_id];
	auto &inviter = m_players[inviter_id];

	invitee.mutual_buddies.push_back(inviter_id);
	inviter.mutual_buddies.push_back(invitee_id);

	send_sync(packets::interserver::buddy::send_accept_buddy_invite(invitee_id, inviter_id));
}

auto player_data_provider::remove_buddy(packet_reader &reader) -> void {
	game_player_id list_owner_id = reader.get<game_player_id>();
	game_player_id removal_id = reader.get<game_player_id>();
	auto &list_owner = m_players[list_owner_id];
	auto &removal = m_players[removal_id];

	ext::remove_element(list_owner.mutual_buddies, removal_id);
	ext::remove_element(removal.mutual_buddies, list_owner_id);

	send_sync(packets::interserver::buddy::send_buddy_removal(list_owner_id, removal_id));
}

auto player_data_provider::readd_buddy(packet_reader &reader) -> void {
	game_player_id list_owner_id = reader.get<game_player_id>();
	game_player_id buddy_id = reader.get<game_player_id>();
	auto &list_owner = m_players[list_owner_id];
	auto &buddy = m_players[buddy_id];

	list_owner.mutual_buddies.push_back(buddy_id);
	buddy.mutual_buddies.push_back(list_owner_id);

	send_sync(packets::interserver::buddy::send_readd_buddy(list_owner_id, buddy_id));
}

}
}