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
#include "common/inter_header.hpp"
#include "common/inter_helper.hpp"
#include "common/io/database.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/party_data.hpp"
#include "common/session.hpp"
#include "common/util/string.hpp"
#include "common/util/time.hpp"
#include "channel_server/buddy_list_packet.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/party.hpp"
#include "channel_server/party_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/players_packet.hpp"
#include "channel_server/smsg_header.hpp"
#include "channel_server/sync_packet.hpp"
#include <algorithm>
#include <cstring>

namespace vana {
namespace channel_server {

auto player_data_provider::parse_channel_connect_packet(packet_reader &reader) -> void {
	// Players
	uint32_t quantity = reader.get<uint32_t>();
	for (uint32_t i = 0; i < quantity; i++) {
		player_data player = reader.get<player_data>();
		add_player_data(player);
	}

	// Parties
	quantity = reader.get<uint32_t>();
	for (uint32_t i = 0; i < quantity; i++) {
		party_data data = reader.get<party_data>();
		ref_ptr<party> party_value = make_ref_ptr<party>(data.id);
		party_value->set_leader(data.leader);

		for (const auto &member : data.members) {
			auto &player = m_player_data[member];
			player.party = data.id;
			party_value->add_member(member, player.name, true);
		}

		m_parties[data.id] = party_value;
	}
}

auto player_data_provider::send_sync(const packet_builder &builder) const -> void {
	channel_server::get_instance().send_world(builder);
}

auto player_data_provider::handle_sync(protocol_sync type, packet_reader &reader) -> void {
	switch (type) {
		case sync::sync_types::channel_start: parse_channel_connect_packet(reader); break;
		case sync::sync_types::player: handle_player_sync(reader); break;
		case sync::sync_types::party: handle_party_sync(reader); break;
		case sync::sync_types::buddy: handle_buddy_sync(reader); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "sync type");
	}
}

// Players
auto player_data_provider::add_player_data(const player_data &data) -> void {
	m_player_data[data.id] = data;
	auto &player = m_player_data[data.id];
	m_player_data_by_name[data.name] = &player;

	if (data.gm_level > 0 || data.admin) {
		m_gm_list.insert(data.id);
	}
}

auto player_data_provider::add_player(ref_ptr<player> player) -> void {
	m_players[player->get_id()] = player;
	m_players_by_name[player->get_name()] = player;
	auto &player_data = m_player_data[player->get_id()];
	if (player_data.party > 0) {
		party *party = get_party(player_data.party);
		player->set_party(party);
		party->set_member(player_data.id, player);
	}
}

auto player_data_provider::remove_player(ref_ptr<player> player) -> void {
	m_players.erase(player->get_id());
	m_players_by_name.erase(player->get_name());

	if (party *party = player->get_party()) {
		party->set_member(player->get_id(), nullptr);
	}

	if (auto followed = player->get_follow()) {
		stop_following(followed);
	}

	auto kvp = m_followers.find(player->get_id());
	if (kvp != std::end(m_followers)) {
		for (auto follower : kvp->second) {
			follower->send(packets::player::show_message("player " + player->get_name() + " has disconnected", packets::player::notice_types::red));
			follower->set_follow(nullptr);
		}
		m_followers.erase(kvp);
	}
}

auto player_data_provider::update_player_level(ref_ptr<player> player) -> void {
	auto &data = m_player_data[player->get_id()];
	data.level = player->get_stats()->get_level();
	send_sync(packets::interserver::player::update_player(data, sync::player::update_bits::level));
	if (data.party != 0) {
		get_party(data.party)->silent_update();
	}
}

auto player_data_provider::update_player_map(ref_ptr<player> player) -> void {
	auto &data = m_player_data[player->get_id()];
	data.map = player->get_map_id();
	send_sync(packets::interserver::player::update_player(data, sync::player::update_bits::map));
	if (data.party != 0) {
		get_party(data.party)->silent_update();
	}

	auto kvp = m_followers.find(data.id);
	if (kvp != std::end(m_followers)) {
		for (auto player : kvp->second) {
			player->set_map(data.map.get());
		}
	}
}

auto player_data_provider::update_player_job(ref_ptr<player> player) -> void {
	auto &data = m_player_data[player->get_id()];
	data.job = player->get_stats()->get_job();
	send_sync(packets::interserver::player::update_player(data, sync::player::update_bits::job));
	if (data.party != 0) {
		get_party(data.party)->silent_update();
	}
}

auto player_data_provider::get_player(game_player_id id) -> ref_ptr<player> {
	auto kvp = m_players.find(id);
	return kvp != std::end(m_players) ? kvp->second : nullptr;
}

auto player_data_provider::get_player(const string &name) -> ref_ptr<player> {
	auto kvp = m_players_by_name.find(name);
	return kvp != std::end(m_players_by_name) ? kvp->second : nullptr;
}

auto player_data_provider::run(function<void(ref_ptr<player>)> func) -> void {
	for (const auto &kvp : m_players) {
		func(kvp.second);
	}
}

auto player_data_provider::get_party(game_party_id id) -> party * {
	return m_parties.find(id) == std::end(m_parties) ? nullptr : m_parties[id].get();
}

auto player_data_provider::get_player_data(game_player_id id) const -> const player_data * const {
	return &m_player_data.find(id)->second;
}

auto player_data_provider::get_player_data_by_name(const string &name) const -> const player_data * const {
	auto kvp = m_player_data_by_name.find(name);
	if (kvp == std::end(m_player_data_by_name)) {
		return nullptr;
	}
	return kvp->second;
}

auto player_data_provider::send(game_player_id player_id, const packet_builder &builder) -> void {
	auto kvp = m_players.find(player_id);
	if (kvp == std::end(m_players)) {
		return;
	}

	kvp->second->send(builder);
}

auto player_data_provider::send(const vector<game_player_id> &player_ids, const packet_builder &builder) -> void {
	for (const auto &player_id : player_ids) {
		auto kvp = m_players.find(player_id);
		if (kvp != std::end(m_players)) {
			kvp->second->send(builder);
		}
	}
}

auto player_data_provider::send(const packet_builder &builder) -> void {
	for (const auto &kvp : m_players) {
		auto player = kvp.second;
		player->send(builder);
	}
}

auto player_data_provider::add_follower(ref_ptr<player> follower, ref_ptr<player> target) -> void {
	auto kvp = m_followers.find(target->get_id());
	if (kvp == std::end(m_followers)) {
		kvp = m_followers.emplace(target->get_id(), vector<ref_ptr<player>>{}).first;
	}
	kvp->second.push_back(follower);
	follower->set_follow(target);
}

auto player_data_provider::stop_following(ref_ptr<player> follower) -> void {
	auto kvp = m_followers.find(follower->get_follow()->get_id());
	ext::remove_element(kvp->second, follower);
	if (kvp->second.size() == 0) {
		m_followers.erase(kvp);
	}
	follower->set_follow(nullptr);
}

auto player_data_provider::disconnect() -> void {
	auto m_players_copy = m_players;

	for (auto &kvp : m_players_copy) {
		if (auto player = kvp.second) {
			player->disconnect();
		}
	}
}

auto player_data_provider::handle_group_chat(int8_t chat_type, game_player_id player_id, const vector<game_player_id> &receivers, const game_chat &chat) -> void {
	auto &builder = packets::player::group_chat(m_player_data[player_id].name, chat, chat_type);

	vector<game_player_id> non_present_receivers;
	for (const auto &player_id : receivers) {
		if (auto player = get_player(player_id)) {
			player->send(builder);
		}
		else {
			non_present_receivers.push_back(player_id);
		}
	}

	if (non_present_receivers.size() > 0) {
		channel_server::get_instance().send_world(vana::packets::prepend(builder, [&non_present_receivers](packet_builder &header) {
			header.add<packet_header>(IMSG_TO_PLAYER_LIST);
			header.add<vector<game_player_id>>(non_present_receivers);
		}));
	}
}

auto player_data_provider::handle_gm_chat(ref_ptr<player> player, const game_chat &chat) -> void {
	game_chat_stream message;
	message << player->get_name() << " [ch"
		<< static_cast<int32_t>(channel_server::get_instance().get_channel_id() + 1)
		<< "] : " << chat;

	auto &builder = packets::player::show_message(message.str(), packets::player::notice_types::blue);

	vector<game_player_id> non_present_receivers;
	for (const auto &player_id : m_gm_list) {
		if (auto player = get_player(player_id)) {
			player->send(builder);
		}
		else {
			non_present_receivers.push_back(player_id);
		}
	}

	if (non_present_receivers.size() > 0) {
		channel_server::get_instance().send_world(vana::packets::prepend(builder, [&non_present_receivers](packet_builder &header) {
			header.add<packet_header>(IMSG_TO_PLAYER_LIST);
			header.add<vector<game_player_id>>(non_present_receivers);
		}));
	}
}

auto player_data_provider::new_player(game_player_id id, const ip &ip, packet_reader &reader) -> void {
	connecting_player player;
	player.connect_ip = ip;
	player.connect_time = vana::util::time::get_now();
	uint16_t packet_size = reader.get<uint16_t>();
	player.packet_size = packet_size;
	if (packet_size > 0) {
		player.held_packet.reset(new unsigned char[packet_size]);
		memcpy(player.held_packet.get(), reader.get_buffer(), packet_size);
	}

	m_connections[id] = player;
}

auto player_data_provider::check_player(game_player_id id, const ip &ip, bool &has_packet) const -> result {
	result result = result::failure;
	has_packet = false;
	auto kvp = m_connections.find(id);
	if (kvp != std::end(m_connections)) {
		auto &test = kvp->second;
		auto distance = vana::util::time::get_distance<milliseconds>(vana::util::time::get_now(), test.connect_time);
		if (test.connect_ip == ip && distance < max_connection_milliseconds) {
			result = result::success;
			if (test.packet_size > 0) {
				has_packet = true;
			}
		}
	}
	return result;
}

auto player_data_provider::get_packet(game_player_id id) const -> packet_reader {
	auto kvp = m_connections.find(id);
	auto &player = kvp->second;
	return packet_reader(player.held_packet.get(), player.packet_size);
}

auto player_data_provider::player_established(game_player_id id) -> void {
	m_connections.erase(id);
}

auto player_data_provider::handle_player_sync(packet_reader &reader) -> void {
	switch (reader.get<protocol_sync>()) {
		case sync::player::new_connectable: handle_new_connectable(reader); break;
		case sync::player::delete_connectable: handle_delete_connectable(reader); break;
		case sync::player::change_channel_go: handle_change_channel(reader); break;
		case sync::player::update_player: handle_update_player(reader); break;
		case sync::player::character_created: handle_character_created(reader); break;
		case sync::player::character_deleted: handle_character_deleted(reader); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "player_sync type");
	}
}

auto player_data_provider::handle_party_sync(packet_reader &reader) -> void {
	protocol_sync type = reader.get<protocol_sync>();
	game_party_id party_id = reader.get<game_party_id>();
	switch (type) {
		case sync::party::create: handle_create_party(party_id, reader.get<game_player_id>()); break;
		case sync::party::disband: handle_disband_party(party_id); break;
		case sync::party::switch_leader: handle_party_transfer(party_id, reader.get<game_player_id>()); break;
		case sync::party::add_member: handle_party_add(party_id, reader.get<game_player_id>()); break;
		case sync::party::remove_member: {
			game_player_id player_id = reader.get<game_player_id>();
			handle_party_remove(party_id, player_id, reader.get<bool>());
			break;
		}
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

auto player_data_provider::handle_change_channel(packet_reader &reader) -> void {
	game_player_id player_id = reader.get<game_player_id>();
	game_channel_id channel_id = reader.get<game_channel_id>();
	ip ip_value = reader.get<ip>();
	connection_port port = reader.get<connection_port>();

	if (auto player = get_player(player_id)) {
		if (!ip_value.is_initialized()) {
			player->send(packets::player::send_blocked_message(packets::player::block_messages::cannot_go));
		}
		else {
			auto kvp = m_followers.find(player_id);
			if (kvp != std::end(m_followers)) {
				for (auto follower : kvp->second) {
					follower->change_channel(channel_id);
					follower->set_follow(nullptr);
				}

				m_followers.erase(kvp);
			}

			player->set_online(false); // Set online to false BEFORE CC packet is sent to player
			player->send(packets::player::change_channel(ip_value, port));
			player->save_all(true);
			player->set_save_on_dc(false);
		}
	}
}

auto player_data_provider::handle_new_connectable(packet_reader &reader) -> void {
	game_player_id player_id = reader.get<game_player_id>();
	ip ip_value = reader.get<ip>();
	new_player(player_id, ip_value, reader);
	send_sync(packets::interserver::player::connectable_established(player_id));
}

auto player_data_provider::handle_delete_connectable(packet_reader &reader) -> void {
	game_player_id id = reader.get<game_player_id>();
	player_established(id);
}

auto player_data_provider::handle_update_player(packet_reader &reader) -> void {
	game_player_id player_id = reader.get<game_player_id>();
	auto &player = m_player_data[player_id];

	protocol_update_bits flags = reader.get<protocol_update_bits>();
	bool update_party = false;
	bool update_buddies = false;
	bool update_guild = false;
	bool update_alliance = false;
	auto old_job = player.job;
	auto old_level = player.level;
	auto old_map = player.map;
	auto old_channel = player.channel;
	auto old_cash = player.cash_shop;
	auto old_mts = player.mts;

	if (flags & sync::player::update_bits::full) {
		update_party = true;
		update_guild = true;
		update_alliance = true;
		update_buddies = true;

		player_data data = reader.get<player_data>();
		player.copy_from(data);
		if (data.gm_level > 0 || data.admin) {
			m_gm_list.insert(data.id);
		}

		player.initialized = true;
	}
	else {
		if (flags & sync::player::update_bits::job) {
			player.job = reader.get<game_job_id>();
			update_party = true;
			update_guild = true;
			update_alliance = true;
		}
		if (flags & sync::player::update_bits::level) {
			player.level = reader.get<game_player_level>();
			update_party = true;
			update_guild = true;
			update_alliance = true;
		}
		if (flags & sync::player::update_bits::map) {
			player.map = reader.get<game_map_id>();
			update_party = true;
		}
		if (flags & sync::player::update_bits::transfer) {
			player.transferring = reader.get<bool>();
		}
		if (flags & sync::player::update_bits::channel) {
			player.channel = reader.get<optional<game_channel_id>>();
			update_party = true;
			update_buddies = true;
		}
		if (flags & sync::player::update_bits::ip) {
			player.ip = reader.get<ip>();
		}
		if (flags & sync::player::update_bits::cash) {
			player.cash_shop = reader.get<bool>();
			update_party = true;
			update_buddies = true;
		}
		if (flags & sync::player::update_bits::mts) {
			player.mts = reader.get<bool>();
			update_party = true;
			update_buddies = true;
		}
	}

	bool actually_updated =
		old_job != player.job ||
		old_level != player.level ||
		old_map != player.map ||
		old_channel != player.channel ||
		old_cash != player.cash_shop ||
		old_mts != player.mts;

	if (actually_updated && !player.transferring) {
		if (update_party && player.party != 0) {
			get_party(player.party)->silent_update();
		}
		if (update_buddies && player.mutual_buddies.size() > 0) {
			for (auto player_id : player.mutual_buddies) {
				if (auto list_player = get_player(player_id)) {
					list_player->send(packets::buddy::online(player.id, player.channel.get(-1), player.cash_shop));
				}
			}
		}
	}
}

auto player_data_provider::handle_character_created(packet_reader &reader) -> void {
	player_data data = reader.get<player_data>();
	add_player_data(data);
}

auto player_data_provider::handle_character_deleted(packet_reader &reader) -> void {
	game_player_id id = reader.get<game_player_id>();
	// Intentionally blank for now
}

// Parties
auto player_data_provider::handle_create_party(game_party_id id, game_player_id leader_id) -> void {
	ref_ptr<party> p = make_ref_ptr<party>(id);
	auto leader = get_player(leader_id);
	auto &data = m_player_data[leader_id];
	data.party = id;

	if (leader == nullptr) {
		p->add_member(leader_id, data.name, true);
	}
	else {
		p->add_member(leader, true);
		leader->send(packets::party::create_party(p.get(), leader));
	}

	p->set_leader(leader_id);
	m_parties[id] = p;
}

auto player_data_provider::handle_disband_party(game_party_id id) -> void {
	if (party *party = get_party(id)) {
		auto &members = party->get_members();
		for (const auto &kvp : members) {
			auto &member = m_player_data[kvp.first];
			member.party = 0;
		}

		party->disband();
		m_parties.erase(id);
	}
}

auto player_data_provider::handle_party_transfer(game_party_id id, game_player_id leader_id) -> void {
	if (party *party = get_party(id)) {
		party->set_leader(leader_id, true);
	}
}

auto player_data_provider::handle_party_remove(game_party_id id, game_player_id player_id, bool kicked) -> void {
	if (party *party = get_party(id)) {
		auto &data = m_player_data[player_id];
		data.party = 0;
		if (auto member = get_player(player_id)) {
			party->delete_member(member, kicked);
		}
		else {
			party->delete_member(player_id, data.name, kicked);
		}
	}
}

auto player_data_provider::handle_party_add(game_party_id id, game_player_id player_id) -> void {
	if (party *party = get_party(id)) {
		auto &data = m_player_data[player_id];
		data.party = id;
		if (auto member = get_player(player_id)) {
			party->add_member(member);
		}
		else {
			party->add_member(player_id, data.name);
		}
	}
}

// Buddies
auto player_data_provider::buddy_invite(packet_reader &reader) -> void {
	game_player_id inviter_id = reader.get<game_player_id>();
	game_player_id invitee_id = reader.get<game_player_id>();
	if (auto invitee = get_player(invitee_id)) {
		vana::channel_server::buddy_invite invite;
		invite.id = inviter_id;
		invite.name = reader.get<string>();
		invitee->get_buddy_list()->add_buddy_invite(invite);
		invitee->get_buddy_list()->check_for_pending_buddy();
	}
}

auto player_data_provider::accept_buddy_invite(packet_reader &reader) -> void {
	game_player_id invitee_id = reader.get<game_player_id>();
	game_player_id inviter_id = reader.get<game_player_id>();
	auto &invitee = m_player_data[invitee_id];
	auto &inviter = m_player_data[inviter_id];

	invitee.mutual_buddies.push_back(inviter_id);
	inviter.mutual_buddies.push_back(invitee_id);

	if (auto player = get_player(inviter_id)) {
		player->send(packets::buddy::online(invitee_id, invitee.channel.get(-1), invitee.cash_shop));
		player->get_buddy_list()->buddy_accepted(invitee_id);
	}
	if (auto player = get_player(invitee_id)) {
		player->send(packets::buddy::online(inviter_id, inviter.channel.get(-1), inviter.cash_shop));
	}
}

auto player_data_provider::remove_buddy(packet_reader &reader) -> void {
	game_player_id list_owner_id = reader.get<game_player_id>();
	game_player_id removal_id = reader.get<game_player_id>();
	auto &list_owner = m_player_data[list_owner_id];
	auto &removal = m_player_data[removal_id];

	ext::remove_element(list_owner.mutual_buddies, removal_id);
	ext::remove_element(removal.mutual_buddies, list_owner_id);

	if (auto player = get_player(removal.id)) {
		player->send(packets::buddy::online(list_owner.id, -1, false));
	}
}

auto player_data_provider::readd_buddy(packet_reader &reader) -> void {
	game_player_id list_owner_id = reader.get<game_player_id>();
	game_player_id buddy_id = reader.get<game_player_id>();
	auto &list_owner = m_player_data[list_owner_id];
	auto &buddy = m_player_data[buddy_id];

	list_owner.mutual_buddies.push_back(buddy_id);
	buddy.mutual_buddies.push_back(list_owner_id);

	if (auto player = get_player(buddy_id)) {
		player->get_buddy_list()->buddy_accepted(list_owner_id);
	}
}

}
}
