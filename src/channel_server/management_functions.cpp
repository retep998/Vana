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
#include "management_functions.hpp"
#include "common/config/rates.hpp"
#include "common/data/provider/item.hpp"
#include "common/database.hpp"
#include "common/exit_codes.hpp"
#include "common/string_utilities.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mystic_door.hpp"
#include "channel_server/npc_handler.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/sync_packet.hpp"
#include "channel_server/world_server_packet.hpp"

namespace vana {
namespace channel_server {

auto management_functions::user_warp(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+)? ?(\w+|\{(-?\d+)\, ?(-?\d+)\}|\[\d+\])?)", matches) == match_result::any_matches) {
		string raw_map = matches[1];
		string raw_portal = matches[2];
		if (raw_map.empty()) {
			chat_handler_functions::show_info(player, "Current map: " + utilities::str::lexical_cast<string>(player->get_map_id()));
			return chat_result::handled_display;
		}

		game_map_id map_id = chat_handler_functions::get_map(raw_map, player);
		map *map = map_id == -1 ?
			nullptr :
			maps::get_map(map_id);

		if (map != nullptr) {
			// We determine here if we're looking for a position or a portal
			if (!raw_portal.empty() && chat_handler_functions::run_regex_pattern(raw_portal, R"(\{(-?\d+)\, ?(-?\d+)\})", matches) == match_result::any_matches) {
				string x_position = matches[1];
				string y_position = matches[2];
				point pos{
					utilities::str::lexical_cast<game_coord>(x_position),
					utilities::str::lexical_cast<game_coord>(y_position)
				};
				player->set_map(map_id, mystic_door::portal_id, pos);
			}
			else if (!raw_portal.empty() && chat_handler_functions::run_regex_pattern(raw_portal, R"(\[(\d+)\])", matches) == match_result::any_matches) {
				string foothold_id = matches[1];
				game_foothold_id foothold = utilities::str::lexical_cast<game_foothold_id>(foothold_id);
				if (!map->is_valid_foothold(foothold) || map->is_vertical_foothold(foothold)) {
					chat_handler_functions::show_error(player, "Invalid foothold: " + foothold_id);
					return chat_result::handled_display;
				}
				point pos = map->get_position_at_foothold(foothold);
				player->set_map(map_id, mystic_door::portal_id, pos);
			}
			else {
				const data::type::portal_info * const destination_portal = map->query_portal_name(raw_portal);

				if (!raw_portal.empty() && destination_portal == nullptr) {
					chat_handler_functions::show_error(player, "Invalid portal: " + raw_portal);
					return chat_result::handled_display;
				}

				if (raw_portal == "tp") {
					player->set_map(map_id, destination_portal->id, destination_portal->pos);
				}
				else {
					player->set_map(map_id, destination_portal);
				}
			}
		}
		else {
			chat_handler_functions::show_error(player, "Invalid map: " + raw_map);
		}

		return chat_result::handled_display;
	}

	return chat_result::show_syntax;
}

auto management_functions::warp(ref_ptr<player> player_value, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) (\w+) (\w+) ?(\w+)? ?(\w+)?)", matches) == match_result::any_matches) {
		string source_type = matches[1];
		string destination_type = matches[2];
		string raw_map = matches[3];
		string optional = matches[4];
		string more_optional = matches[5];
		game_map_id source_map_id = -1;
		game_map_id destination_map_id = -1;
		string portal;
		auto source_player = ref_ptr<player>{nullptr};
		bool valid_combo = true;
		bool only_source = true;
		bool single_argument_destination = false;
		bool portal_specified = false;

		auto resolve_player = [](const string &player_arg) { return channel_server::get_instance().get_player_data_provider().get_player(player_arg); };
		auto resolve_map_arg = [player_value](const string &map_arg) { return chat_handler_functions::get_map(map_arg, player_value); };
		auto resolve_map_portal = [&portal, player_value](const string &portal_arg) {
			if (!portal_arg.empty()) {
				portal = portal_arg;
				return true;
			}
			return false;
		};
		auto resolve_map_current = [player_value]() { return player_value->get_map_id(); };
		auto resolve_map_player = [player_value](const string &player_arg) {
			if (auto target = channel_server::get_instance().get_player_data_provider().get_player(player_arg)) {
				return target->get_map_id();
			}
			return -2;
		};

		if (source_type == "map") {
			only_source = false;

			source_map_id = resolve_map_arg(raw_map);
			if (source_map_id == -1 || maps::get_map(source_map_id) == nullptr) {
				chat_handler_functions::show_error(player_value, "Invalid source map: " + raw_map);
				return chat_result::handled_display;
			}

			if (destination_type == "map") {
				destination_map_id = resolve_map_arg(optional);
				portal_specified = resolve_map_portal(more_optional);
			}
			else if (destination_type == "current" || destination_type == "self") {
				destination_map_id = resolve_map_current();
				portal_specified = resolve_map_portal(optional);
			}
			else if (destination_type == "player") {
				destination_map_id = resolve_map_player(optional);
				portal_specified = resolve_map_portal(more_optional);
			}
			else {
				valid_combo = false;
			}
		}
		else if (source_type == "player") {
			only_source = false;

			source_player = channel_server::get_instance().get_player_data_provider().get_player(raw_map);
			if (source_player == nullptr) {
				chat_handler_functions::show_error(player_value, "Invalid source player: " + raw_map);
				return chat_result::handled_display;
			}

			if (destination_type == "map") {
				destination_map_id = resolve_map_arg(optional);
				portal_specified = resolve_map_portal(more_optional);
			}
			else if (destination_type == "current" || destination_type == "self") {
				destination_map_id = resolve_map_current();
				portal_specified = resolve_map_portal(optional);
			}
			else if (destination_type == "player") {
				destination_map_id = resolve_map_player(optional);
				portal_specified = resolve_map_portal(more_optional);
			}
			else {
				valid_combo = false;
			}
		}
		else if (source_type == "current" || source_type == "self") {
			source_map_id = resolve_map_current();
			if (destination_type == "map") {
				destination_map_id = resolve_map_arg(raw_map);
				portal_specified = resolve_map_portal(optional);
			}
			else if (destination_type == "player") {
				destination_map_id = resolve_map_player(raw_map);
				portal_specified = resolve_map_portal(optional);
			}
			else {
				valid_combo = false;
			}
		}
		else if (source_type == "channel") {
			if (destination_type == "map") {
				destination_map_id = resolve_map_arg(raw_map);
				portal_specified = resolve_map_portal(optional);
			}
			else if (destination_type == "current" || destination_type == "self") {
				destination_map_id = resolve_map_current();
				portal_specified = resolve_map_portal(raw_map);
				single_argument_destination = true;
			}
			else if (destination_type == "player") {
				destination_map_id = resolve_map_player(raw_map);
				portal_specified = resolve_map_portal(optional);
			}
			else {
				valid_combo = false;
			}
		}
		else {
			chat_handler_functions::show_error(player_value, "Invalid source type: " + source_type);
			return chat_result::handled_display;
		}

		if (!valid_combo) {
			chat_handler_functions::show_error(player_value, "Invalid destination type for source type \"" + source_type + "\": " + destination_type);
			return chat_result::handled_display;
		}

		if (destination_map_id == -2) {
			chat_handler_functions::show_error(player_value, "Invalid destination player: " + (only_source ? raw_map : optional));
			return chat_result::handled_display;
		}

		map *destination = destination_map_id == -1 ?
			nullptr :
			maps::get_map(destination_map_id);

		if (destination_map_id == -1 || destination == nullptr) {
			chat_handler_functions::show_error(player_value, "Invalid destination map: " + (only_source ? raw_map : optional));
			return chat_result::handled_display;
		}

		const data::type::portal_info * const destination_portal = destination->query_portal_name(portal);
		if (portal_specified && destination_portal == nullptr) {
			chat_handler_functions::show_error(player_value, "Invalid destination portal: " + (single_argument_destination ? raw_map : (only_source ? optional : more_optional)));
			return chat_result::handled_display;
		}

		auto warp_to_map = [&](ref_ptr<player> target) {
			if (target->get_map_id() != destination_map_id) {
				if (portal == "tp") {
					target->set_map(destination_map_id, destination_portal->id, destination_portal->pos);
				}
				else {
					target->set_map(destination_map_id, destination_portal);
				}
			}
		};

		if (source_type == "map" || source_type == "current") {
			out_stream message;
			message << "Warped all players on map ID " << source_map_id << " to map ID " << destination_map_id;
			if (!portal.empty()) {
				message << " (portal " << portal << ")";
			}

			maps::get_map(source_map_id)->run_function_players(warp_to_map);
			chat_handler_functions::show_info(player_value, message.str());
		}
		else if (source_type == "player") {
			out_stream message;
			message << "Warped player " << raw_map << " to map ID " << destination_map_id;
			if (!portal.empty()) {
				message << " (portal " << portal << ")";
			}

			warp_to_map(source_player);
			chat_handler_functions::show_info(player_value, message.str());
		}
		else if (source_type == "self") {
			warp_to_map(player_value);
		}
		else if (source_type == "channel") {
			out_stream message;
			message << "Warped everyone in the channel to map ID " << destination_map_id;
			if (!portal.empty()) {
				message << " (portal " << portal << ")";
			}

			channel_server::get_instance().get_player_data_provider().run(warp_to_map);
			chat_handler_functions::show_info(player_value, message.str());
		}

		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::follow(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+)?)", matches) == match_result::any_matches) {
		string player_name = matches[1];
		if (auto follow = player->get_follow()) {
			if (!player_name.empty()) {
				chat_handler_functions::show_error(player, "You're already following player " + follow->get_name());
			}
			else {
				channel_server::get_instance().get_player_data_provider().stop_following(player);
				chat_handler_functions::show_info(player, "No longer following " + follow->get_name());
			}
		}
		else {
			if (player_name.size() != 0) {
				if (auto target = channel_server::get_instance().get_player_data_provider().get_player(player_name)) {
					channel_server::get_instance().get_player_data_provider().add_follower(player, target);
					chat_handler_functions::show_info(player, "Now following player " + target->get_name());
				}
				else {
					chat_handler_functions::show_error(player, "Invalid player: " + player_name);
				}
			}
			else {
				chat_handler_functions::show_error(player, "You must specify a player to follow");
			}
		}
		return chat_result::handled_display;
	}

	return chat_result::show_syntax;
}

auto management_functions::change_channel(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+))", matches) == match_result::any_matches) {
		string target_channel = matches[1];
		game_channel_id channel = atoi(target_channel.c_str()) - 1;
		player->change_channel(channel);
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::lag(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+))", matches) == match_result::any_matches) {
		string target = matches[1];
		if (auto p = channel_server::get_instance().get_player_data_provider().get_player(target)) {
			chat_handler_functions::show_info(player, p->get_name() + "'s lag: " + utilities::str::lexical_cast<string>(p->get_latency().count()) + "ms");
		}
		else {
			chat_handler_functions::show_error(player, "Invalid player: " + target);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::header(ref_ptr<player> player, const game_chat &args) -> chat_result {
	channel_server::get_instance().send_world(packets::interserver::config::scrolling_header(args));
	return chat_result::handled_display;
}

auto management_functions::shutdown(ref_ptr<player> player, const game_chat &args) -> chat_result {
	chat_handler_functions::show_info(player, "Shutting down the server");
	channel_server::get_instance().log(log_type::gm_command, "GM shutdown the server. GM: " + player->get_name());
	// TODO FIXME remove this or figure out a better way to post a shutdown than just doing the shutdown here
	exit(exit_code::forced_by_gm);
	return chat_result::handled_display;
}

auto management_functions::kick(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		if (auto target = channel_server::get_instance().get_player_data_provider().get_player(args)) {
			if (player->get_gm_level() > target->get_gm_level()) {
				target->disconnect();
				chat_handler_functions::show_info(player, "Kicked " + args + " from the server");
			}
			else {
				chat_handler_functions::show_error(player, "Player " + args + " is your peer or outranks you");
			}
		}
		else {
			chat_handler_functions::show_error(player, "Invalid player: " + args);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::relog(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->change_channel(channel_server::get_instance().get_channel_id());
	return chat_result::handled_display;
}

auto management_functions::calculate_ranks(ref_ptr<player> player, const game_chat &args) -> chat_result {
	channel_server::get_instance().send_world(packets::interserver::ranking_calculation());
	chat_handler_functions::show_info(player, "Sent a signal to force the calculation of rankings");
	return chat_result::handled_display;
}

auto management_functions::item(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+) ?(\d*)?)", matches) == match_result::any_matches) {
		string raw_item = matches[1];
		game_item_id item_id = atoi(raw_item.c_str());
		if (channel_server::get_instance().get_item_data_provider().get_item_info(item_id) != nullptr) {
			string count_string = matches[2];
			uint16_t count = count_string.empty() ? 1 : atoi(count_string.c_str());
			inventory::add_new_item(player, item_id, count, stat_variance::gachapon);
		}
		else {
			chat_handler_functions::show_error(player, "Invalid item: " + raw_item);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::storage(ref_ptr<player> player, const game_chat &args) -> chat_result {
	npc_handler::show_storage(player, 1012009);
	return chat_result::handled_display;
}

auto management_functions::shop(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		game_shop_id shop_id = -1;
		if (args == "gear") shop_id = 9999999;
		else if (args == "scrolls") shop_id = 9999998;
		else if (args == "nx") shop_id = 9999997;
		else if (args == "face") shop_id = 9999996;
		else if (args == "ring") shop_id = 9999995;
		else if (args == "chair") shop_id = 9999994;
		else if (args == "mega") shop_id = 9999993;
		else if (args == "pet") shop_id = 9999992;
		else shop_id = atoi(args.c_str());

		if (npc_handler::show_shop(player, shop_id) == result::successful) {
			return chat_result::handled_display;
		}
	}
	return chat_result::show_syntax;
}

auto management_functions::reload(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		if (args == "items" || args == "drops" || args == "shops" ||
			args == "mobs" || args == "beauty" || args == "scripts" ||
			args == "skills" || args == "reactors" || args == "pets" ||
			args == "quests" || args == "maps" || args == "all") {
			channel_server::get_instance().send_world(packets::interserver::reload_mcdb(args));
			chat_handler_functions::show_info(player, "Reloading message for " + args + " sent to all channels");
		}
		else {
			chat_handler_functions::show_error(player, "Invalid reload type: " + args);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::run_npc(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+))", matches) == match_result::any_matches) {
		auto &provider = channel_server::get_instance().get_npc_data_provider();
		game_npc_id npc_id = atoi(args.c_str());
		if (provider.is_valid_npc_id(npc_id)) {
			npc *value = new npc{npc_id, player};
			value->run();
		}
		else {
			chat_handler_functions::show_error(player, "Invalid NPC ID: " + args);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::add_npc(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+))", matches) == match_result::any_matches) {
		auto &provider = channel_server::get_instance().get_npc_data_provider();
		game_npc_id npc_id = atoi(args.c_str());
		if (provider.is_valid_npc_id(npc_id)) {
			data::type::npc_spawn_info npc;
			npc.id = npc_id;
			npc.foothold = 0;
			npc.pos = player->get_pos();
			npc.rx0 = npc.pos.x - 50;
			npc.rx1 = npc.pos.x + 50;
			game_map_object id = player->get_map()->add_npc(npc);
			chat_handler_functions::show_info(player, "Spawned NPC " + args + " with object ID " + utilities::str::lexical_cast<string>(id));
		}
		else {
			chat_handler_functions::show_error(player, "Invalid NPC ID: " + args);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::kill_npc(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->set_npc(nullptr);
	return chat_result::handled_display;
}

auto management_functions::kill(ref_ptr<player> player_value, const game_chat &args) -> chat_result {
	if (player_value->get_gm_level() == 1) {
		player_value->get_stats()->set_hp(0);
	}
	else {
		bool proceed = true;
		auto iterate = [&player_value](function<bool(ref_ptr<player> p)> func) -> int {
			int32_t kills = 0;
			map *map = player_value->get_map();
			for (size_t i = 0; i < map->get_num_players(); ++i) {
				auto t = map->get_player(i);
				if (t != player_value) {
					if (func(t)) kills++;
				}
			}
			return kills;
		};
		if (args == "all") {
			proceed = false;
			int32_t kills = iterate([](ref_ptr<player> p) -> bool {
				p->get_stats()->set_hp(0);
				return true;
			});
			chat_handler_functions::show_info(player_value, "Killed " + utilities::str::lexical_cast<string>(kills) + " players in the current map");
		}
		else if (args == "gm" || args == "players") {
			proceed = false;
			int32_t kills = iterate([&args](ref_ptr<player> p) -> bool {
				if ((args == "gm" && p->is_gm()) || (args == "players" && !p->is_gm())) {
					p->get_stats()->set_hp(0);
					return true;
				}
				return false;
			});
			chat_handler_functions::show_info(player_value, "Killed " + utilities::str::lexical_cast<string>(kills) + " " + (args == "gm" ? "GMs" : "players") + " in the current map");
		}
		if (proceed) {
			if (args == "me") {
				player_value->get_stats()->set_hp(0);
				chat_handler_functions::show_info(player_value, "Killed yourself");
			}
			else if (auto kill = channel_server::get_instance().get_player_data_provider().get_player(args)) {
				// Kill by name
				kill->get_stats()->set_hp(0);
				chat_handler_functions::show_info(player_value, "Killed " + args);
			}
			else {
				// Nothing valid
				return chat_result::show_syntax;
			}
		}
	}
	return chat_result::handled_display;
}

auto management_functions::ban(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) ?(\d+)?)", matches) == match_result::any_matches) {
		string target_name = matches[1];
		if (auto target = channel_server::get_instance().get_player_data_provider().get_player(target_name)) {
			target->disconnect();
		}
		string reason_string = matches[2];
		int8_t reason = reason_string.empty() ? 1 : atoi(reason_string.c_str());

		// Ban account
		string expire{"2130-00-00 00:00:00"};

		auto &db = database::get_char_db();
		auto &sql = db.get_session();
		soci::statement st = (sql.prepare
			<< "UPDATE " << db.make_table("accounts") << " u "
			<< "INNER JOIN " << db.make_table("characters") << " c ON u.account_id = c.account_id "
			<< "SET "
			<< "	u.banned = 1, "
			<< "	u.ban_expire = :expire, "
			<< "	u.ban_reason = :reason "
			<< "WHERE c.name = :name ",
			soci::use(target_name, "name"),
			soci::use(expire, "expire"),
			soci::use(reason, "reason"));

		st.execute();

		if (st.get_affected_rows() > 0) {
			string ban_message = target_name + " has been banned" + chat_handler_functions::get_ban_string(reason);
			channel_server::get_instance().get_player_data_provider().send(packets::player::show_message(ban_message, packets::player::notice_types::notice));
			channel_server::get_instance().log(log_type::gm_command, [&](out_stream &log) {
				log << "GM " << player->get_name()
					<< " banned a player with reason " << static_cast<int32_t>(reason)
					<< ", player: " << target_name;
			});
		}
		else {
			chat_handler_functions::show_error(player, "Invalid player: " + target_name);
		}

		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::temp_ban(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) (\d+) (\d+))", matches) == match_result::any_matches) {
		string target_name = matches[1];
		if (auto target = channel_server::get_instance().get_player_data_provider().get_player(target_name)) {
			target->disconnect();
		}
		string reason_string = matches[2];
		string length = matches[3];
		int8_t reason = reason_string.empty() ? 1 : atoi(reason_string.c_str());

		// Ban account
		auto &db = database::get_char_db();
		auto &sql = db.get_session();
		soci::statement st = (sql.prepare
			<< "UPDATE " << db.make_table("accounts") << " u "
			<< "INNER JOIN " << db.make_table("characters") << " c ON u.account_id = c.account_id "
			<< "SET "
			<< "	u.banned = 1, "
			<< "	u.ban_expire = DATE_ADD(NOW(), INTERVAL :expire DAY), "
			<< "	u.ban_reason = :reason "
			<< "WHERE c.name = :name ",
			soci::use(target_name, "name"),
			soci::use(length, "expire"),
			soci::use(reason, "reason"));

		st.execute();

		if (st.get_affected_rows() > 0) {
			string ban_message = target_name + " has been banned" + chat_handler_functions::get_ban_string(reason);
			channel_server::get_instance().get_player_data_provider().send(packets::player::show_message(ban_message, packets::player::notice_types::notice));

			channel_server::get_instance().log(log_type::gm_command, [&](out_stream &log) {
				log << "GM " << player->get_name()
					<< " temporary banned a player with reason " << static_cast<int32_t>(reason)
					<< " for " << length
					<< " days, player: " << target_name;
			});
		}
		else {
			chat_handler_functions::show_error(player, "Invalid player: " + target_name);
		}

		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::ip_ban(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) ?(\d+)?)", matches) == match_result::any_matches) {
		string target_name = matches[1];
		if (auto target = channel_server::get_instance().get_player_data_provider().get_player(target_name)) {
			string target_ip = target->get_ip().get().to_string();
			target->disconnect();

			string reason_string = matches[2];
			int8_t reason = reason_string.empty() ? 1 : atoi(reason_string.c_str());

			// IP ban
			auto &db = database::get_char_db();
			auto &sql = db.get_session();
			soci::statement st = (sql.prepare << "INSERT INTO " << db.make_table("ip_bans") << " (ip) VALUES (:ip)",
				soci::use(target_ip, "ip"));

			st.execute();

			if (st.get_affected_rows() > 0) {
				string ban_message = target_name + " has been banned" + chat_handler_functions::get_ban_string(reason);

				channel_server::get_instance().get_player_data_provider().send(packets::player::show_message(ban_message, packets::player::notice_types::notice));
				channel_server::get_instance().log(log_type::gm_command, [&](out_stream &log) {
					log << "GM " << player->get_name()
						<< " IP banned a player with reason " << static_cast<int32_t>(reason)
						<< ", player: " << target_name;
				});
			}
			else {
				chat_handler_functions::show_error(player, "Unknown error, couldn't ban " + target_ip);
			}
		}
		else {
			// TODO FIXME add raw IP banning
			chat_handler_functions::show_error(player, "Invalid player: " + target_name);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::unban(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		// Unban account
		auto &db = database::get_char_db();
		auto &sql = db.get_session();
		soci::statement st = (sql.prepare
			<< "UPDATE " << db.make_table("accounts") << " u "
			<< "INNER JOIN " << db.make_table("characters") << " c ON u.account_id = c.account_id "
			<< "SET "
			<< "	u.banned = 0, "
			<< "	u.ban_reason = NULL, "
			<< "	u.ban_expire = NULL "
			<< "WHERE c.name = :name ",
			soci::use(args, "name"));

		st.execute();

		if (st.get_affected_rows() > 0) {
			string banMessage = args + " has been unbanned";
			chat_handler_functions::show_info(player, banMessage);
			channel_server::get_instance().log(log_type::gm_command, [&](out_stream &log) {
				log << "GM " << player->get_name()
					<< " unbanned a player: " << args;
			});
		}
		else {
			chat_handler_functions::show_error(player, "Invalid player: " + args);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto management_functions::rehash(ref_ptr<player> player, const game_chat &args) -> chat_result {
	channel_server::get_instance().send_world(packets::interserver::rehash_config());
	chat_handler_functions::show_info(player, "Sent a signal to force rehashing world configurations");
	return chat_result::handled_display;
}

auto management_functions::rates(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (!args.empty()) {
		if (chat_handler_functions::run_regex_pattern(args, R"((\w+) ?(\w+)? ?(\-?\d+)?)", matches) == match_result::no_matches) {
			return chat_result::show_syntax;
		}
		string type = matches[1];
		string classification = matches[2];
		if (!classification.empty()) {
			if (classification != "mobexp" &&
				classification != "questexp" &&
				classification != "drop" &&
				classification != "dropmeso" &&
				classification != "globaldrop" &&
				classification != "globaldropmeso") {
				return chat_result::show_syntax;
			}
		}
		string value = matches[3];
		if (type == "view") {
			auto display = [player](const string &type, int32_t rate) {
				chat_handler_functions::show_info(player, type + " rate: " + utilities::str::lexical_cast<string>(rate) + "x");
			};

			chat_handler_functions::show_info(player, "Current Rates");
			auto &config = channel_server::get_instance().get_config();
			if (classification.empty() || classification == "mobexp") display("Mob EXP", config.rates.mob_exp_rate);
			if (classification.empty() || classification == "questexp") display("Quest EXP", config.rates.quest_exp_rate);
			if (classification.empty() || classification == "drop") display("Drop", config.rates.drop_rate);
			if (classification.empty() || classification == "dropmeso") display("Drop meso", config.rates.drop_meso);
			if (classification.empty() || classification == "globaldrop") {
				display("Global drop", config.rates.is_global_drop_consistent_with_regular_drop_rate() ?
					config.rates.drop_rate :
					config.rates.global_drop_rate);
			}
			if (classification.empty() || classification == "globaldropmeso") {
				display("Global drop meso", config.rates.is_global_drop_meso_consistent_with_regular_drop_meso_rate() ?
					config.rates.drop_meso :
					config.rates.global_drop_meso);
			}
		}
		else if (type == "reset") {
			if (classification.empty()) {
				chat_handler_functions::show_info(player, "Sent request to reset all rates");
				channel_server::get_instance().send_world(packets::interserver::config::reset_rates(config::rate_type::all));
			}
			else {
				int32_t rate_type = 0;
				if (classification == "mobexp") rate_type = config::rate_type::mob_exp_rate;
				else if (classification == "drop") rate_type = config::rate_type::drop_rate;
				else if (classification == "dropmeso") rate_type = config::rate_type::drop_meso;
				else if (classification == "questexp") rate_type = config::rate_type::quest_exp_rate;
				else if (classification == "globaldrop") rate_type = config::rate_type::global_drop_rate;
				else if (classification == "globaldropmeso") rate_type = config::rate_type::global_drop_meso;
				chat_handler_functions::show_info(player, "Sent request to reset specified rate");
				channel_server::get_instance().send_world(packets::interserver::config::reset_rates(rate_type));
			}
		}
		else if (type == "set") {
			if (classification.empty()) {
				return chat_result::show_syntax;
			}

			int32_t rate_type = 0;
			if (classification == "mobexp") rate_type = config::rate_type::mob_exp_rate;
			else if (classification == "drop") rate_type = config::rate_type::drop_rate;
			else if (classification == "dropmeso") rate_type = config::rate_type::drop_meso;
			else if (classification == "questexp") rate_type = config::rate_type::quest_exp_rate;
			else if (classification == "globaldrop") rate_type = config::rate_type::global_drop_rate;
			else if (classification == "globaldropmeso") rate_type = config::rate_type::global_drop_meso;
			int32_t new_amount = value.empty() ?
				((rate_type & config::rate_type::global) != 0 ?
					config::rates::consistent_rate_between_global_and_regular :
					1) :
				atoi(value.c_str());

			channel_server::get_instance().modify_rate(rate_type, new_amount);
			chat_handler_functions::show_info(player, "Sent request to modify rate");
		}
		else {
			return chat_result::show_syntax;
		}
	}
	else {
		return chat_result::show_syntax;
	}
	return chat_result::handled_display;
}

auto management_functions::packet(ref_ptr<player> player, const game_chat &args) -> chat_result {
	auto is_hex = [](char character) {
		return (character >= 'a' && character <= 'f') ||
				(character >= 'A' && character <= 'F') ||
				(character >= '0' && character <= '9');
	};

	using utilities::str::lexical_cast;
	if (!args.empty()) {
		// Build packet
		packet_builder packet;

		// Build raw hex string from cool format
		// iNNNN = add int
		// sNNNN = add short
		// lNNNN = add long
		// bNN   = add byte
		// "...." = add string

		size_t args_length = args.length();
		for (size_t i = 0; i < args_length; i++) {
			char character = args.at(i);

			size_t next_space = args.find(' ', i + 1);
			if (next_space == game_chat::npos) {
				next_space = args_length;
			}
			size_t string_length = (next_space - 1) - (i - 1);

			if (character == ' ') continue;

			// Check for integer value

			if (character == 'i' ||
				character == 's' ||
				character == 'l' ||
				character == 'b') {

				game_chat value_string;
				int64_t value = 0;
				if (string_length != 0) {
					value_string = args.substr(i + 1, string_length - 1);
					value = utilities::str::atoli(value_string.c_str());
				}

				if (character == 'l') {
					packet.add<int64_t>(value);
				}
				else if (character == 'i') {
					if (value < 0 || value >= UINT32_MAX) {
						chat_handler_functions::show_info(player, "Number '" + value_string + "' is not between 0 and " + lexical_cast<string>(UINT32_MAX));
						return chat_result::handled_display;
					}
					packet.add<int32_t>(static_cast<int32_t>(value));
				}
				else if (character == 's') {
					if (value < 0 || value >= UINT16_MAX) {
						chat_handler_functions::show_info(player, "Number '" + value_string + "' is not between 0 and " + lexical_cast<string>(UINT16_MAX));
						return chat_result::handled_display;
					}
					packet.add<int16_t>(static_cast<int16_t>(value));
				}
				else if (character == 'b') {
					if (value < 0 || value >= UINT8_MAX) {
						chat_handler_functions::show_info(player, "Number '" + value_string + "' is not between 0 and " + lexical_cast<string>(UINT8_MAX));
						return chat_result::handled_display;
					}
					packet.add<int8_t>(static_cast<int8_t>(value));
				}

				i += string_length;
				continue;
			}

			if (character == '"') {
				size_t next_quote = args.find('"', i + 1);
				if (next_quote == game_chat::npos) {
					chat_handler_functions::show_info(player, "String not terminated");
					return chat_result::handled_display;
				}

				size_t string_length = (next_quote - 1) - i;

				if (string_length == 0) {
					// no text
					packet.add<game_chat>("");
				}
				else {
					packet.add<game_chat>(args.substr(i + 1, string_length));
				}

				i = next_quote;
				continue;
			}

			if (is_hex(character)) {
				if ((string_length % 2) != 0) {
					chat_handler_functions::show_info(player, "Hex is invalid length at " + lexical_cast<string>(i));
					return chat_result::handled_display;
				}

				// quick check
				for (size_t j = i; j < (i + string_length); j++) {
					char hex_char = args.at(j);
					if (!is_hex(hex_char)) {
						chat_handler_functions::show_info(player, "Hex is invalid length at " + lexical_cast<string>(j));
						return chat_result::handled_display;
					}
				}

				packet.add_bytes(args.substr(i, string_length));

				i += string_length;
				continue;
			}

			// Parsed nothing. huh?

			chat_handler_functions::show_info(player, "Character is invalid at " + lexical_cast<string>(i));
			return chat_result::handled_display;
		}

		// Make sure we are not consuming an empty string
		// or a string that does not match a correct packet length
		if (packet.get_size() < 2) {
			return chat_result::show_syntax;
		}

		channel_server::get_instance().log(log_type::gm_command, [&](out_stream &log) {
			log << "GM " << player->get_name()
				<< " sent packet to self: " << std::endl
				<< "raw: " << packet << std::endl
				<< "cmd: " << args;
		});

		player->send(packet);
		return chat_result::handled_display;
	}

	return chat_result::show_syntax;
}

}
}