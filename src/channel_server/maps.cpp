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
#include "maps.hpp"
#include "common/file_utilities.hpp"
#include "common/packet_reader.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/lua/lua_portal.hpp"
#include "channel_server/map_data_provider.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/pet_handler.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/summon_handler.hpp"
#include <string>

namespace vana {
namespace channel_server {

auto maps::get_map(game_map_id map_id) -> map * {
	return channel_server::get_instance().get_map(map_id);
}

auto maps::unload_map(game_map_id map_id) -> void {
	channel_server::get_instance().unload_map(map_id);
}

auto maps::use_portal(ref_ptr<player> player, const data::type::portal_info * const portal) -> void {
	if (portal->disabled) {
		player->send(packets::map::portal_blocked());
		player->send(packets::player::show_message("the portal is closed for now.", packets::player::notice_types::red));
		return;
	}

	if (portal->script.size() != 0) {
		// Check for "onlyOnce" portal
		if (portal->only_once && player->used_portal(portal->id)) {
			player->send(packets::map::portal_blocked());
			return;
		}

		string filename = channel_server::get_instance().get_script_data_provider().build_script_path(data::type::script_types::portal, portal->script);

		if (utilities::file::exists(filename)) {
			lua::lua_portal lua_env = {filename, player->get_id(), player->get_map_id(), portal};

			if (!lua_env.player_map_changed()) {
				player->send(packets::map::portal_blocked());
			}
			if (portal->only_once && !lua_env.portal_failed()) {
				player->add_used_portal(portal->id);
			}
		}
		else {
			string message = player->is_gm() ?
				"portal '" + portal->script + "' is currently unavailable." :
				"this portal is currently unavailable.";

			player->send(packets::player::show_message(message, packets::player::notice_types::red));
			player->send(packets::map::portal_blocked());
		}
	}
	else {
		// Normal portal
		map *to_map = get_map(portal->to_map);
		if (to_map == nullptr) {
			player->send(packets::player::show_message("bzzt. the map you're attempting to travel to doesn't exist.", packets::player::notice_types::red));
			player->send(packets::map::portal_blocked());
			return;
		}
		const data::type::portal_info * const next_portal = to_map->get_portal(portal->to_name);
		player->set_map(portal->to_map, next_portal);
	}
}

auto maps::use_portal(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_portal_count>();

	int32_t opcode = reader.get<int32_t>();
	switch (opcode) {
		case 0: // Dead
			if (player->get_stats()->is_dead()) {
				reader.unk<string>(); // Seemingly useless
				reader.unk<int8_t>(); // Seemingly useless
				bool wheel = reader.get<bool>();
				if (wheel) {
					if (player->get_inventory()->get_item_amount(constant::item::wheel_of_destiny) <= 0) {
						player->accept_death(false);
						return;
					}
					inventory::take_item(player, constant::item::wheel_of_destiny, 1);
				}
				player->accept_death(wheel);
			}
			break;
		case -1: {
			string portal_name = reader.get<string>();

			map *to_map = player->get_map();
			if (to_map == nullptr) {
				return;
			}
			const data::type::portal_info * const portal = to_map->get_portal(portal_name);
			if (portal == nullptr) {
				return;
			}
			use_portal(player, portal);
			break;
		}
		default: {
			// GM Map change (command "/m")
			if (player->is_gm()) {
				if (get_map(opcode) != nullptr) {
					player->set_map(opcode);
				}
				else {
					// TODO FIXME message
				}
			}
			else {
				// Hacking
				return;
			}
		}
	}
}

auto maps::use_scripted_portal(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_portal_count>();
	string portal_name = reader.get<string>();

	const data::type::portal_info * const portal = player->get_map()->get_portal(portal_name);
	if (portal == nullptr) {
		return;
	}
	use_portal(player, portal);
}

auto maps::add_player(ref_ptr<player> player, game_map_id map_id) -> void {
	get_map(map_id)->add_player(player);
	get_map(map_id)->show_objects(player);
	pet_handler::show_pets(player);
	summon_handler::show_summon(player);
	// Bug in global - would be fixed here:
	// Berserk doesn't display properly when switching maps with it activated - client displays, but no message is sent to any client
	// player->get_active_buffs()->check_berserk(true) would override the default of only displaying changes
}

}
}