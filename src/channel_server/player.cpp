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
#include "player.hpp"
#include "common/common_header.hpp"
#include "common/data/provider/item.hpp"
#include "common/database.hpp"
#include "common/enum_utilities.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/randomizer.hpp"
#include "common/session.hpp"
#include "common/split_packet_builder.hpp"
#include "common/string_utilities.hpp"
#include "common/time_utilities.hpp"
#include "channel_server/buddy_list_handler.hpp"
#include "channel_server/buddy_list_packet.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/chat_handler.hpp"
#include "channel_server/cmsg_header.hpp"
#include "channel_server/command_handler.hpp"
#include "channel_server/drop_handler.hpp"
#include "channel_server/fame.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/inventory_handler.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/key_maps.hpp"
#include "channel_server/levels_packet.hpp"
#include "channel_server/map.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mob_handler.hpp"
#include "channel_server/monster_book_packet.hpp"
#include "channel_server/mystic_door.hpp"
#include "channel_server/npc.hpp"
#include "channel_server/npc_handler.hpp"
#include "channel_server/party.hpp"
#include "channel_server/party_handler.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/pet_handler.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_handler.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/quests.hpp"
#include "channel_server/reactor_handler.hpp"
#include "channel_server/server_packet.hpp"
#include "channel_server/skill_macros.hpp"
#include "channel_server/skills.hpp"
#include "channel_server/summon.hpp"
#include "channel_server/summon_handler.hpp"
#include "channel_server/sync_packet.hpp"
#include "channel_server/trade_handler.hpp"
#include "channel_server/world_server_session.hpp"
#include <array>
#include <stdexcept>

namespace vana {
namespace channel_server {

player::player() :
	movable_life{0, point{}, 0}
{
}

auto player::handle(packet_reader &reader) -> result {
	try {
		packet_header header = reader.get<packet_header>();
		if (!m_is_connect) {
			// We don't want to accept any other packet than the one for loading the character
			if (header == CMSG_PLAYER_LOAD) {
				player_connect(reader);
			}
		}
		else {
			switch (header) {
				case CMSG_ADMIN_COMMAND: command_handler::handle_admin_command(shared_from_this(), reader); break;
				case CMSG_ADMIN_MESSENGER: player_handler::handle_admin_messenger(shared_from_this(), reader); break;
				case CMSG_ATTACK_ENERGY_CHARGE: player_handler::use_energy_charge_attack(shared_from_this(), reader); break;
				case CMSG_ATTACK_MAGIC: player_handler::use_spell_attack(shared_from_this(), reader); break;
				case CMSG_ATTACK_MELEE: player_handler::use_melee_attack(shared_from_this(), reader); break;
				case CMSG_ATTACK_RANGED: player_handler::use_ranged_attack(shared_from_this(), reader); break;
				case CMSG_BOMB_SKILL_USE: player_handler::use_bomb_skill(shared_from_this(), reader); break;
				case CMSG_BUDDY: buddy_list_handler::handle_buddy_list(shared_from_this(), reader); break;
				case CMSG_BUFF_ITEM_USE: inventory_handler::use_buff_item(shared_from_this(), reader); break;
				case CMSG_CASH_ITEM_USE: inventory_handler::use_cash_item(shared_from_this(), reader); break;
				case CMSG_CASH_SHOP: send(packets::player::send_blocked_message(packets::player::block_messages::no_cash_shop)); break;
				case CMSG_CHAIR: inventory_handler::handle_chair(shared_from_this(), reader); break;
				case CMSG_CHALKBOARD: send_map(packets::inventory::send_chalkboard_update(m_id, "")); set_chalkboard(""); break;
				case CMSG_CHANNEL_CHANGE: change_channel(reader.get<int8_t>()); break;
				case CMSG_COMMAND: command_handler::handle_command(shared_from_this(), reader); break;
				case CMSG_DROP_MESOS: drop_handler::drop_mesos(shared_from_this(), reader); break;
				case CMSG_EMOTE: player_handler::handle_facial_expression(shared_from_this(), reader); break;
				case CMSG_FAME: fame::handle_fame(shared_from_this(), reader); break;
				case CMSG_FRIENDLY_MOB_DAMAGE: mob_handler::friendly_damaged(shared_from_this(), reader); break;
				case CMSG_HAMMER: inventory_handler::handle_hammer_time(shared_from_this()); break;
				case CMSG_ITEM_CANCEL: inventory_handler::cancel_item(shared_from_this(), reader); break;
				case CMSG_ITEM_EFFECT: inventory_handler::use_item_effect(shared_from_this(), reader); break;
				case CMSG_ITEM_LOOT: drop_handler::loot_item(shared_from_this(), reader); break;
				case CMSG_ITEM_MOVE: inventory_handler::move_item(shared_from_this(), reader); break;
				case CMSG_ITEM_USE: inventory_handler::use_item(shared_from_this(), reader); break;
				case CMSG_KEYMAP: change_key(reader); break;
				case CMSG_MACRO_LIST: change_skill_macros(reader); break;
				case CMSG_MAP_CHANGE: maps::use_portal(shared_from_this(), reader); break;
				case CMSG_MAP_CHANGE_SPECIAL: maps::use_scripted_portal(shared_from_this(), reader); break;
				case CMSG_MESSAGE_GROUP: chat_handler::handle_group_chat(shared_from_this(), reader); break;
				case CMSG_MOB_CONTROL: mob_handler::monster_control(shared_from_this(), reader); break;
				case CMSG_MOB_EXPLOSION: mob_handler::handle_bomb(shared_from_this(), reader); break;
				case CMSG_MOB_TURNCOAT_DAMAGE: mob_handler::handle_turncoats(shared_from_this(), reader); break;
				case CMSG_MONSTER_BOOK: player_handler::handle_monster_book(shared_from_this(), reader); break;
				case CMSG_MTS: send(packets::player::send_blocked_message(packets::player::block_messages::mts_unavailable)); break;
				case CMSG_MULTI_STAT_ADDITION: get_stats()->add_stat_multi(reader); break;
				case CMSG_MYSTIC_DOOR_ENTRY: player_handler::handle_door_use(shared_from_this(), reader); break;
				case CMSG_NPC_ANIMATE: npc_handler::handle_npc_animation(shared_from_this(), reader); break;
				case CMSG_NPC_TALK: npc_handler::handle_npc(shared_from_this(), reader); break;
				case CMSG_NPC_TALK_CONT: npc_handler::handle_npc_in(shared_from_this(), reader); break;
				case CMSG_PARTY: party_handler::handle_request(shared_from_this(), reader); break;
				case CMSG_PET_CHAT: pet_handler::handle_chat(shared_from_this(), reader); break;
				case CMSG_PET_COMMAND: pet_handler::handle_command(shared_from_this(), reader); break;
				case CMSG_PET_CONSUME_POTION: pet_handler::handle_consume_potion(shared_from_this(), reader); break;
				case CMSG_PET_FOOD_USE: pet_handler::handle_feed(shared_from_this(), reader); break;
				case CMSG_PET_LOOT: drop_handler::pet_loot(shared_from_this(), reader); break;
				case CMSG_PET_MOVEMENT: pet_handler::handle_movement(shared_from_this(), reader); break;
				case CMSG_PET_SUMMON: pet_handler::handle_summon(shared_from_this(), reader); break;
				case CMSG_PLAYER_CHAT: chat_handler::handle_chat(shared_from_this(), reader); break;
				case CMSG_PLAYER_DAMAGE: player_handler::handle_damage(shared_from_this(), reader); break;
				case CMSG_PLAYER_HEAL: player_handler::handle_heal(shared_from_this(), reader); break;
				case CMSG_PLAYER_INFO: player_handler::handle_get_info(shared_from_this(), reader); break;
				case CMSG_PLAYER_MOVE: player_handler::handle_moving(shared_from_this(), reader); break;
				case CMSG_PLAYER_ROOM: trade_handler::trade_handler(shared_from_this(), reader); break;
				case CMSG_QUEST_OBTAIN: quests::get_quest(shared_from_this(), reader); break;
				case CMSG_REACTOR_HIT: reactor_handler::hit_reactor(shared_from_this(), reader); break;
				case CMSG_REACTOR_TOUCH: reactor_handler::touch_reactor(shared_from_this(), reader); break;
				case CMSG_REVIVE_EFFECT: inventory_handler::use_item_effect(shared_from_this(), reader); break;
				case CMSG_SCROLL_USE: inventory_handler::use_scroll(shared_from_this(), reader); break;
				case CMSG_SHOP: npc_handler::use_shop(shared_from_this(), reader); break;
				case CMSG_SKILL_ADD: skills::add_skill(shared_from_this(), reader); break;
				case CMSG_SKILL_CANCEL: skills::cancel_skill(shared_from_this(), reader); break;
				case CMSG_SKILL_USE: skills::use_skill(shared_from_this(), reader); break;
				case CMSG_SKILLBOOK_USE: inventory_handler::use_skillbook(shared_from_this(), reader); break;
				case CMSG_SPECIAL_SKILL: player_handler::handle_special_skills(shared_from_this(), reader); break;
				case CMSG_STAT_ADDITION: get_stats()->add_stat(reader); break;
				case CMSG_STORAGE: npc_handler::use_storage(shared_from_this(), reader); break;
				case CMSG_SUMMON_ATTACK: player_handler::use_summon_attack(shared_from_this(), reader); break;
				case CMSG_SUMMON_BAG_USE: inventory_handler::use_summon_bag(shared_from_this(), reader); break;
				case CMSG_SUMMON_DAMAGE: summon_handler::damage_summon(shared_from_this(), reader); break;
				case CMSG_SUMMON_MOVEMENT: summon_handler::move_summon(shared_from_this(), reader); break;
				case CMSG_SUMMON_SKILL: summon_handler::summon_skill(shared_from_this(), reader); break;
				case CMSG_TELEPORT_ROCK: inventory_handler::handle_rock_functions(shared_from_this(), reader); break;
				case CMSG_TELEPORT_ROCK_USE: inventory_handler::handle_rock_teleport(shared_from_this(), constant::item::special_teleport_rock, reader); break;
				case CMSG_TOWN_SCROLL_USE: inventory_handler::use_return_scroll(shared_from_this(), reader); break;
				case CMSG_USE_CHAIR: inventory_handler::use_chair(shared_from_this(), reader); break;
				case CMSG_USE_REWARD_ITEM: inventory_handler::handle_reward_item(shared_from_this(), reader); break;
				case CMSG_USE_SCRIPT_ITEM: inventory_handler::handle_script_item(shared_from_this(), reader); break;
#ifdef DEBUG
				case CMSG_PONG:
				case CMSG_PLAYER_MOB_DISTANCE:
				case CMSG_PLAYER_UNK_MAP:
				case CMSG_PLAYER_BOAT_MAP:
				case CMSG_PLAYER_UNK_MAP2:
				case CMSG_ADMIN_COMMAND_LOG:
				case CMSG_BUFF_ACKNOWLEDGE:
				case CMSG_MAP_TELEPORT:
					// Do nothing
					break;
				default:
					std::cout << "Unhandled 0x" << std::hex << std::setw(4) << header << std::dec << ": " << reader << std::endl;
#endif
			}
		}
	}
	catch (const packet_content_exception &e) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		reader.reset();
		channel_server::get_instance().log(log_type::malformed_packet, [&](out_stream &log) {
			log << "Player ID: " << get_id()
				<< "; Packet: " << reader
				<< "; Error: " << e.what();
		});
		disconnect();
		return result::failure;
	}

	return result::successful;
}

auto player::on_disconnect() -> void {
	m_disconnecting = true;

	map *cur_map = maps::get_map(m_map);
	if (get_map_chair() != 0) {
		cur_map->player_seated(get_map_chair(), nullptr);
	}

	cur_map->remove_player(shared_from_this());
	m_is_connect = false;

	if (is_trading()) {
		trade_handler::cancel_trade(shared_from_this());
	}

	bool is_leader = false;
	if (party *party = get_party()) {
		is_leader = party->is_leader(get_id());
	}

	get_skills()->on_disconnect();

	if (instance *inst = get_instance()) {
		inst->remove_player(get_id());
		inst->player_disconnect(get_id(), is_leader);
	}
	//if (this->get_stats()->is_dead()) {
	//	this->accept_death();
	//}
	// "Bug" in global, would be fixed here:
	// When disconnecting and dead, you actually go back to forced return map before the death return map
	// (that means that it's parsed while logging in, not while logging out)
	if (const data::type::portal_info * const closest = cur_map->get_nearest_spawn_point(get_pos())) {
		m_map_pos = closest->id;
	}

	if (m_save_on_dc) {
		save_all(true);
		set_online(false);
	}

	if (channel_server::get_instance().is_connected()) {
		// Do not connect to worldserver if the worldserver has disconnected
		channel_server::get_instance().send_world(packets::interserver::player::disconnect(get_id()));
	}

	channel_server::get_instance().get_player_data_provider().remove_player(shared_from_this());

	// Important: Clean up all external pointers that may be pointing to this at the class level before finalizing
	m_npc.reset();
	channel_server::get_instance().finalize_player(shared_from_this());
}

auto player::player_connect(packet_reader &reader) -> void {
	game_player_id id = reader.get<game_player_id>();
	bool has_transfer_packet = false;
	auto &channel = channel_server::get_instance();
	auto &provider = channel.get_player_data_provider();
	if (provider.check_player(id, get_ip().get(), has_transfer_packet) == result::failure) {
		// Hacking
		disconnect();
		return;
	}

	m_id = id;
	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	soci::row row;
	sql.once
		<< "SELECT c.*, u.gm_level, u.admin "
		<< "FROM " << db.make_table("characters") << " c "
		<< "INNER JOIN " << db.make_table("accounts") << " u ON c.account_id = u.account_id "
		<< "WHERE c.character_id = :char",
		soci::use(id, "char"),
		soci::into(row);

	if (!sql.got_data()) {
		// Hacking
		disconnect();
		return;
	}

	m_name = row.get<string>("name");
	m_account_id = row.get<game_account_id>("account_id");
	m_map = row.get<game_map_id>("map");
	m_gm_level = row.get<int32_t>("gm_level");
	m_admin = row.get<bool>("admin");
	m_face = row.get<game_face_id>("face");
	m_hair = row.get<game_hair_id>("hair");
	m_world_id = row.get<game_world_id>("world_id");
	m_gender = row.get<game_gender_id>("gender");
	m_skin = row.get<game_skin_id>("skin");
	m_map_pos = row.get<game_portal_id>("pos");
	m_buddylist_size = row.get<uint8_t>("buddylist_size");

	// Stats
	m_stats = make_owned_ptr<player_stats>(
		shared_from_this(),
		row.get<game_player_level>("level"),
		row.get<game_job_id>("job"),
		row.get<game_fame>("fame"),
		row.get<game_stat>("str"),
		row.get<game_stat>("dex"),
		row.get<game_stat>("int"),
		row.get<game_stat>("luk"),
		row.get<game_stat>("ap"),
		row.get<game_health_ap>("hpmp_ap"),
		row.get<game_stat>("sp"),
		row.get<game_health>("chp"),
		row.get<game_health>("mhp"),
		row.get<game_health>("cmp"),
		row.get<game_health>("mmp"),
		row.get<game_experience>("exp")
	);

	// Inventory
	m_mounts = make_owned_ptr<player_mounts>(shared_from_this());
	m_pets = make_owned_ptr<player_pets>(shared_from_this());
	array<game_inventory_slot_count, constant::inventory::count> max_slots;
	max_slots[0] = row.get<game_inventory_slot_count>("equip_slots");
	max_slots[1] = row.get<game_inventory_slot_count>("use_slots");
	max_slots[2] = row.get<game_inventory_slot_count>("setup_slots");
	max_slots[3] = row.get<game_inventory_slot_count>("etc_slots");
	max_slots[4] = row.get<game_inventory_slot_count>("cash_slots");
	m_inventory = make_owned_ptr<player_inventory>(shared_from_this(), max_slots, row.get<game_mesos>("mesos"));
	m_storage = make_owned_ptr<player_storage>(shared_from_this());

	// Skills
	m_skills = make_owned_ptr<player_skills>(shared_from_this());

	// Buffs/summons
	m_active_buffs = make_owned_ptr<player_active_buffs>(shared_from_this());
	m_summons = make_owned_ptr<player_summons>(shared_from_this());

	bool first_connect = !has_transfer_packet;
	auto &config = channel.get_config();
	if (has_transfer_packet) {
		parse_transfer_packet(provider.get_packet(m_id));
	}
	else {
		// No packet, that means that they're connecting for the first time
		set_connection_time(time(nullptr));
		m_gm_chat = is_gm() && config.default_gm_chat_mode;
	}

	provider.player_established(id);

	// The rest
	m_variables = make_owned_ptr<player_variables>(shared_from_this());
	m_buddy_list = make_owned_ptr<player_buddy_list>(shared_from_this());
	m_quests = make_owned_ptr<player_quests>(shared_from_this());
	m_monster_book = make_owned_ptr<player_monster_book>(shared_from_this());

	opt_int32_t book_cover = row.get<opt_int32_t>("book_cover");
	get_monster_book()->set_cover(book_cover.get(0));

	// Key Maps and Macros
	key_maps key_maps;
	key_maps.load(id);

	skill_macros skill_macros;
	skill_macros.load(id);

	// Adjust down HP or MP if necessary
	get_stats()->check_hp_mp();

	if (is_gm() || is_admin()) {
		if (first_connect) {
			m_map = constant::map::gm_map;
			m_map_pos = -1;
		}
	}
	else if (maps::get_map(m_map)->get_forced_return() != constant::map::no_map) {
		m_map = maps::get_map(m_map)->get_forced_return();
		m_map_pos = -1;
	}
	else if (get_stats()->is_dead()) {
		m_map = maps::get_map(m_map)->get_return_map();
		m_map_pos = -1;
	}
	m_last_map = m_map;

	m_pos = maps::get_map(m_map)->get_spawn_point(m_map_pos)->pos;
	m_stance = 0;
	m_foothold = 0;

	send(packets::player::connect_data(shared_from_this()));

	if (!config.scrolling_header.empty()) {
		send(packets::show_scrolling_header(config.scrolling_header));
	}

	for (int8_t i = 0; i < constant::inventory::max_pet_count; i++) {
		if (pet *pet = get_pets()->get_summoned(i)) {
			pet->set_pos(maps::get_map(m_map)->get_spawn_point(m_map_pos)->pos);
		}
	}

	send(packets::player::show_keys(&key_maps));

	send(packets::buddy::update(shared_from_this(), packets::buddy::action_types::add));
	get_buddy_list()->check_for_pending_buddy();

	send(packets::player::show_skill_macros(&skill_macros));

	provider.add_player(shared_from_this());
	maps::add_player(shared_from_this(), m_map);

	channel.log(log_type::info, [&](out_stream &log) {
		log << m_name << " (" << m_id << ") connected from " << get_ip();
	});

	set_online(true);
	m_is_connect = true;

	player_data data;
	const player_data * const existing_data = provider.get_player_data(m_id);
	bool first_connection_since_server_started = first_connect && !existing_data->initialized;

	if (first_connection_since_server_started) {
		data.admin = m_admin;
		data.level = get_stats()->get_level();
		data.job = get_stats()->get_job();
		data.gm_level = m_gm_level;
		data.name = m_name;
		data.mutual_buddies = m_buddy_list->get_buddy_ids();
	}

	data.channel = channel.get_channel_id();
	data.map = m_map;
	data.id = m_id;
	data.ip = get_ip().get();

	channel.send_world(packets::interserver::player::connect(data, first_connection_since_server_started));
}

auto player::get_map() const -> map * {
	return maps::get_map(get_map_id());
}

auto player::internal_set_map(game_map_id map_id, game_portal_id portal_id, const point &pos, bool from_position) -> void {
	map *old_map = maps::get_map(m_map);
	map *new_map = maps::get_map(map_id);

	old_map->remove_player(shared_from_this());
	if (m_map != map_id) {
		m_last_map = m_map;
	}
	m_map = map_id;
	m_map_pos = portal_id;
	m_used_portals.clear();
	set_pos(pos);
	set_stance(0);
	set_foothold(0);
	set_fall_counter(0);

	// Prevent chair Denial of Service
	if (get_map_chair() != 0) {
		old_map->player_seated(get_map_chair(), nullptr);
		set_map_chair(0);
	}
	if (get_chair() != 0) {
		set_chair(0);
	}

	for (int8_t i = 0; i < constant::inventory::max_pet_count; i++) {
		if (pet *pet = get_pets()->get_summoned(i)) {
			pet->set_pos(pos);
		}
	}

	get_summons()->changed_map();

	if (!get_chalkboard().empty() && !new_map->can_chalkboard()) {
		set_chalkboard("");
	}

	send(packets::map::change_map(shared_from_this(), from_position, get_pos()));
	maps::add_player(shared_from_this(), map_id);

	channel_server::get_instance().get_player_data_provider().update_player_map(shared_from_this());
}

auto player::set_map(game_map_id map_id, game_portal_id portal_id, const point &pos) -> void {
	internal_set_map(map_id, portal_id, pos, true);
}

auto player::set_map(game_map_id map_id, const data::type::portal_info * const portal, bool is_instance) -> void {
	if (!maps::get_map(map_id)) {
		send(packets::map::portal_blocked());
		return;
	}
	map *old_map = maps::get_map(m_map);
	map *new_map = maps::get_map(map_id);

	const data::type::portal_info * const actual_portal = portal != nullptr ?
		portal :
		new_map->get_spawn_point();

	if (!is_instance) {
		// Only trigger the message for natural map changes not caused by moveAllPlayers, etc.
		bool is_party_leader = get_party() != nullptr && get_party()->is_leader(get_id());
		if (instance *i = old_map->get_instance()) {
			i->player_change_map(get_id(), map_id, m_map, is_party_leader);
		}
		if (instance *i = new_map->get_instance()) {
			i->player_change_map(get_id(), map_id, m_map, is_party_leader);
		}
	}

	internal_set_map(map_id, actual_portal->id, point{actual_portal->pos.x, actual_portal->pos.y - 40}, false);
}

auto player::get_medal_name() -> string {
	out_stream ret;
	if (game_item_id item_id = get_inventory()->get_equipped_id(constant::equip_slot::medal)) {
		// Check if there's an item at that slot
		ret << "<" << channel_server::get_instance().get_item_data_provider().get_item_info(item_id)->name << "> ";
	}
	ret << get_name();
	return ret.str();
}

auto player::change_channel(game_channel_id channel) -> void {
	channel_server::get_instance().send_world(packets::interserver::player::change_channel(shared_from_this(), channel));
}

auto player::get_transfer_packet() const -> packet_builder {
	packet_builder builder;
	builder
		.add<int64_t>(get_connection_time())
		.add<game_player_id>(m_follow != nullptr ? m_follow->get_id() : 0)
		.add<bool>(m_gm_chat)
		.add_buffer(get_active_buffs()->get_transfer_packet())
		.add_buffer(get_summons()->get_transfer_packet());

	return builder;
}

auto player::parse_transfer_packet(packet_reader &reader) -> void {
	set_connection_time(reader.get<int64_t>());
	game_player_id follow_id = reader.get<game_player_id>();
	if (follow_id != 0) {
		auto &provider = channel_server::get_instance().get_player_data_provider();
		if (auto follow = provider.get_player(follow_id)) {
			provider.add_follower(shared_from_this(), follow);
		}
	}

	m_gm_chat = reader.get<bool>();

	get_active_buffs()->parse_transfer_packet(reader);
	get_summons()->parse_transfer_packet(reader);
}

auto player::change_key(packet_reader &reader) -> void {
	int32_t mode = reader.get<int32_t>();
	int32_t how_many = reader.get<int32_t>();

	enum key_modes : int32_t {
		change_keys = 0x00,
		auto_hp_potion = 0x01,
		auto_mp_potion = 0x02
	};

	if (mode == change_keys) {
		if (how_many == 0) {
			return;
		}

		key_maps key_maps;
		key_maps.load(m_id);
		for (int32_t i = 0; i < how_many; i++) {
			int32_t pos = reader.get<int32_t>();
			key_map_type type;
			if (enum_utilities::try_cast_from_underlying(reader.get<int8_t>(), type) != result::successful) {
				// Probably hacking
				return;
			}
			int32_t action = reader.get<int32_t>();
			key_maps.add(pos, key_maps::key_map{type, action});
		}

		key_maps.save(m_id);
	}
	else if (mode == auto_hp_potion) {
		get_inventory()->set_auto_hp_pot(how_many);
	}
	else if (mode == auto_mp_potion) {
		get_inventory()->set_auto_mp_pot(how_many);
	}
}

auto player::change_skill_macros(packet_reader &reader) -> void {
	uint8_t num = reader.get<uint8_t>();
	if (num == 0) {
		return;
	}
	skill_macros macros;
	for (uint8_t i = 0; i < num; i++) {
		string name = reader.get<string>();
		bool shout = reader.get<bool>();
		game_skill_id skill1 = reader.get<game_skill_id>();
		game_skill_id skill2 = reader.get<game_skill_id>();
		game_skill_id skill3 = reader.get<game_skill_id>();

		macros.add(i, new skill_macros::skill_macro{name, shout, skill1, skill2, skill3});
	}
	macros.save(get_id());
}

auto player::set_hair(game_hair_id id) -> void {
	m_hair = id;
	send(packets::player::update_stat(constant::stat::hair, id));
}

auto player::set_face(game_face_id id) -> void {
	m_face = id;
	send(packets::player::update_stat(constant::stat::face, id));
}

auto player::set_skin(game_skin_id id) -> void {
	m_skin = id;
	send(packets::player::update_stat(constant::stat::skin, id));
}

auto player::save_stats() -> void {
	player_stats *s = get_stats();
	player_inventory *i = get_inventory();
	// Need local bindings
	// Stats
	game_player_level level = s->get_level();
	game_job_id job = s->get_job();
	game_stat str = s->get_str();
	game_stat dex = s->get_dex();
	game_stat intl = s->get_int();
	game_stat luk = s->get_luk();
	game_health hp = s->get_hp();
	game_health max_hp = s->get_max_hp(true);
	game_health mp = s->get_mp();
	game_health max_mp = s->get_max_mp(true);
	game_health_ap hp_mp_ap = s->get_hp_mp_ap();
	game_stat ap = s->get_ap();
	game_stat sp = s->get_sp();
	game_fame fame = s->get_fame();
	game_experience exp = s->get_exp();
	// Inventory
	game_inventory_slot_count equip = i->get_max_slots(constant::inventory::equip);
	game_inventory_slot_count use = i->get_max_slots(constant::inventory::use);
	game_inventory_slot_count setup = i->get_max_slots(constant::inventory::setup);
	game_inventory_slot_count etc = i->get_max_slots(constant::inventory::etc);
	game_inventory_slot_count cash = i->get_max_slots(constant::inventory::cash);
	game_mesos money = i->get_mesos();
	// Other
	int32_t raw_cover = get_monster_book()->get_cover();
	opt_int32_t cover;
	if (raw_cover != 0) {
		cover = raw_cover;
	}

	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	sql.once
		<< "UPDATE " << db.make_table("characters") << " "
		<< "SET "
		<< "	level = :level, "
		<< "	job = :job, "
		<< "	str = :str, "
		<< "	dex = :dex, "
		<< "	`int` = :int, "
		<< "	luk = :luk, "
		<< "	chp = :hp, "
		<< "	mhp = :maxhp, "
		<< "	cmp = :mp, "
		<< "	mmp = :maxmp, "
		<< "	hpmp_ap = :hpmpap, "
		<< "	ap = :ap, "
		<< "	sp = :sp, "
		<< "	exp = :exp, "
		<< "	fame = :fame, "
		<< "	map = :map, "
		<< "	pos = :pos, "
		<< "	gender = :gender, "
		<< "	skin = :skin, "
		<< "	face = :face, "
		<< "	hair = :hair, "
		<< "	mesos = :money, "
		<< "	equip_slots = :equip, "
		<< "	use_slots = :use, "
		<< "	setup_slots = :setup, "
		<< "	etc_slots = :etc, "
		<< "	cash_slots = :cash, "
		<< "	buddylist_size = :buddylist, "
		<< "	book_cover = :cover "
		<< "WHERE character_id = :char",
		soci::use(m_id, "char"),
		soci::use(level, "level"),
		soci::use(job, "job"),
		soci::use(str, "str"),
		soci::use(dex, "dex"),
		soci::use(intl, "int"),
		soci::use(luk, "luk"),
		soci::use(hp, "hp"),
		soci::use(max_hp, "maxhp"),
		soci::use(mp, "mp"),
		soci::use(max_mp, "maxmp"),
		soci::use(hp_mp_ap, "hpmpap"),
		soci::use(ap, "ap"),
		soci::use(sp, "sp"),
		soci::use(exp, "exp"),
		soci::use(fame, "fame"),
		soci::use(m_map, "map"),
		soci::use(m_map_pos, "pos"),
		soci::use(m_gender, "gender"),
		soci::use(m_skin, "skin"),
		soci::use(m_face, "face"),
		soci::use(m_hair, "hair"),
		soci::use(money, "money"),
		soci::use(equip, "equip"),
		soci::use(use, "use"),
		soci::use(setup, "setup"),
		soci::use(etc, "etc"),
		soci::use(cash, "cash"),
		soci::use(m_buddylist_size, "buddylist"),
		soci::use(cover, "cover");
}

auto player::save_all(bool save_cooldowns) -> void {
	save_stats();
	get_inventory()->save();
	get_storage()->save();
	get_monster_book()->save();
	get_mounts()->save();
	get_pets()->save();
	get_quests()->save();
	get_skills()->save(save_cooldowns);
	get_variables()->save();
}

auto player::set_online(bool online) -> void {
	int32_t online_id = online ? channel_server::get_instance().get_online_id() : 0;
	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	sql.once
		<< "UPDATE " << db.make_table("accounts") << " u "
		<< "INNER JOIN " << db.make_table("characters") << " c ON u.account_id = c.account_id "
		<< "SET "
		<< "	u.online = :online_id, "
		<< "	c.online = :online "
		<< "WHERE c.character_id = :char",
		soci::use(m_id, "char"),
		soci::use(online, "online"),
		soci::use(online_id, "online_id");
}

auto player::set_level_date() -> void {
	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	sql.once << "UPDATE " << db.make_table("characters") << " c SET c.time_level = NOW() WHERE c.character_id = :char",
		soci::use(m_id, "char");
}

auto player::accept_death(bool wheel) -> void {
	// TODO FIXME verify
	// Is this correct? The old line was:
	// game_map_id to_map = maps::get_map(m_map) ? maps::get_map(m_map)->get_return_map() : m_map;
	// That doesn't seem very useful to me since maps::get_map(m_map) should always be true, otherwise how did the player get to the map?

	game_map_id to_map = maps::get_map(m_map) != nullptr ? maps::get_map(m_map)->get_return_map() : m_map;
	if (wheel) {
		to_map = get_map_id();
	}
	get_active_buffs()->remove_buffs();
	get_stats()->check_hp_mp();
	get_stats()->set_hp(50, false);
	set_map(to_map);
}

auto player::has_gm_equip() const -> bool {
	auto equipped_utility = [this](game_inventory_slot slot, game_item_id item_id) -> bool {
		return this->get_inventory()->get_equipped_id(slot) == item_id;
	};

	if (equipped_utility(constant::equip_slot::helm, constant::item::gm_hat)) {
		return true;
	}
	if (equipped_utility(constant::equip_slot::top, constant::item::gm_top)) {
		return true;
	}
	if (equipped_utility(constant::equip_slot::bottom, constant::item::gm_bottom)) {
		return true;
	}
	if (equipped_utility(constant::equip_slot::weapon, constant::item::gm_weapon)) {
		return true;
	}
	return false;
}

auto player::is_using_gm_hide() const -> bool {
	return m_active_buffs->is_using_gm_hide();
}

auto player::has_gm_benefits() const -> bool {
	return is_using_gm_hide() || has_gm_equip();
}

auto player::set_buddy_list_size(uint8_t size) -> void {
	m_buddylist_size = size;
	send(packets::buddy::show_size(shared_from_this()));
}

auto player::get_portal_count(bool add) -> game_portal_count {
	if (add) {
		m_portal_count++;
	}
	return m_portal_count;
}

auto player::initialize_rng(packet_builder &builder) -> void {
	uint32_t seed1 = randomizer::rand<uint32_t>();
	uint32_t seed2 = randomizer::rand<uint32_t>();
	uint32_t seed3 = randomizer::rand<uint32_t>();

	m_rand_stream = make_owned_ptr<tausworthe_generator>(seed1, seed2, seed3);

	builder.add<uint32_t>(seed1);
	builder.add<uint32_t>(seed2);
	builder.add<uint32_t>(seed3);
}

auto player::send(const packet_builder &builder) -> void {
	// TODO FIXME resource
	if (is_disconnecting()) return;
	packet_handler::send(builder);
}

auto player::send(const split_packet_builder &builder) -> void {
	// TODO FIXME resource
	if (is_disconnecting()) return;
	send(builder.player);
}

auto player::send_map(const packet_builder &builder, bool exclude_self) -> void {
	get_map()->send(builder, exclude_self ? shared_from_this() : nullptr);
}

auto player::send_map(const split_packet_builder &builder) -> void {
	get_map()->send(builder, shared_from_this());
}

}
}
