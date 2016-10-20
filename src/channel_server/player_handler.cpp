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
#include "player_handler.hpp"
#include "common/algorithm.hpp"
#include "common/attack_data.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/skill.hpp"
#include "common/data/type/skill_type.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/inter_header.hpp"
#include "common/mp_eater_data.hpp"
#include "common/packet_wrapper.hpp"
#include "common/randomizer.hpp"
#include "common/packet_reader.hpp"
#include "common/return_damage_data.hpp"
#include "common/time_utilities.hpp"
#include "common/timer/timer.hpp"
#include "common/wide_point.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/drop.hpp"
#include "channel_server/drop_handler.hpp"
#include "channel_server/drops_packet.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/maple_tvs.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mist.hpp"
#include "channel_server/mob_handler.hpp"
#include "channel_server/monster_book_packet.hpp"
#include "channel_server/movement_handler.hpp"
#include "channel_server/mystic_door.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/player_skills.hpp"
#include "channel_server/players_packet.hpp"
#include "channel_server/skills.hpp"
#include "channel_server/skills_packet.hpp"
#include "channel_server/status_info.hpp"
#include "channel_server/summon.hpp"
#include "channel_server/summon_handler.hpp"
#include <functional>

namespace vana {
namespace channel_server {

auto player_handler::handle_door_use(ref_ptr<player> player, packet_reader &reader) -> void {
	game_player_id door_player_id = reader.get<game_player_id>();
	bool to_town = !reader.get<bool>();
	auto door_holder = channel_server::get_instance().get_player_data_provider().get_player(door_player_id);
	if (door_holder == nullptr || (door_holder->get_party() != player->get_party() && door_holder != player)) {
		// Hacking or lag
		return;
	}
	ref_ptr<mystic_door> door = door_holder->get_skills()->get_mystic_door();
	if (door == nullptr) {
		// Hacking or lag
		return;
	}

	if (player->get_map_id() != door->get_map_id() && player->get_map_id() != door->get_town_id()) {
		// Hacking
		return;
	}

	if ((player->get_map_id() == door->get_town_id()) == to_town) {
		// Hacking
		return;
	}

	if (to_town) {
		player->set_map(door->get_town_id(), door->get_portal_id(), door->get_town_pos());
	}
	else {
		player->set_map(door->get_map_id(), mystic_door::portal_id, door->get_map_pos());
	}
}

auto player_handler::handle_damage(ref_ptr<player> player, packet_reader &reader) -> void {
	const int8_t bump_damage = -1;
	const int8_t map_damage = -2;

	reader.skip<game_tick_count>();
	int8_t type = reader.get<int8_t>();
	reader.skip<uint8_t>(); // Element - 0x00 = elementless, 0x01 = ice, 0x02 = fire, 0x03 = lightning
	game_damage damage = reader.get<game_damage>();
	bool damage_applied = false;
	bool deadly_attack = false;
	uint8_t hit = 0;
	uint8_t stance = 0;
	game_mob_skill_id disease = 0;
	game_mob_skill_level level = 0;
	game_health mp_burn = 0;
	game_map_object map_mob_id = 0;
	game_mob_id mob_id = 0;
	game_skill_id no_damage_id = 0;
	return_damage_data pgmr;

	if (type != map_damage) {
		mob_id = reader.get<game_mob_id>();
		map_mob_id = reader.get<game_map_object>();
		auto mob = player->get_map()->get_mob(map_mob_id);
		if (mob != nullptr && mob->get_mob_id() != mob_id) {
			// Hacking
			return;
		}
		if (type != bump_damage) {
			if (mob == nullptr) {
				// TODO FIXME: Restructure so the attack works fine even if the mob dies?
				return;
			}

			auto attack = channel_server::get_instance().get_mob_data_provider().get_mob_attack(mob->get_mob_id_or_link(), type);
			if (attack == nullptr) {
				// Hacking
				return;
			}
			disease = attack->disease;
			level = attack->level;
			mp_burn = attack->mp_burn;
			deadly_attack = attack->deadly_attack;
		}

		hit = reader.get<uint8_t>(); // Knock direction
		pgmr.reduction = reader.get<uint8_t>();
		reader.unk<uint8_t>(); // I think reduction is a short, but it's a byte in the S -> C packet, so..
		if (pgmr.reduction != 0) {
			pgmr.is_physical = reader.get<bool>();
			pgmr.map_mob_id = reader.get<game_map_object>();
			if (pgmr.map_mob_id != map_mob_id) {
				// Hacking
				return;
			}
			reader.skip<int8_t>(); // 0x06 for Power Guard, 0x00 for Mana Reflection?
			reader.skip<point>(); // Mob position garbage
			pgmr.pos = reader.get<point>();
			pgmr.damage = damage;
			if (pgmr.is_physical) {
				// Only Power Guard decreases damage
				damage = (damage - (damage * pgmr.reduction / 100));
			}
			mob->apply_damage(player->get_id(), (pgmr.damage * pgmr.reduction / 100));
		}
	}

	if (type == map_damage) {
		level = reader.get<game_mob_skill_level>();
		disease = reader.get<game_mob_skill_id>();
	}
	else {
		stance = reader.get<int8_t>(); // Power Stance
		auto power_stance = player->get_active_buffs()->get_power_stance_source();
		if (stance > 0 && !power_stance.is_initialized()) {
			// Hacking
			return;
		}
	}

	if (damage == -1) {
		if (!player->get_skills()->has_no_damage_skill()) {
			// Hacking
			return;
		}
		no_damage_id = player->get_skills()->get_no_damage_skill();
	}

	if (disease > 0 && damage != 0) {
		// Fake/Guardian don't prevent disease
		buffs::add_buff(player, disease, level, milliseconds{0});
	}

	game_health mp = player->get_stats()->get_mp();
	game_health hp = player->get_stats()->get_hp();

	auto deadly_attack_func = [&player, &mp](bool set_hp) {
		if (mp > 0) {
			player->get_stats()->set_mp(1);
		}
		if (set_hp) {
			player->get_stats()->set_hp(1);
		}
	};

	if (damage > 0 && !player->has_gm_benefits()) {
		auto meso_guard = player->get_active_buffs()->get_meso_guard_source();
		if (meso_guard.is_initialized() && player->get_inventory()->get_mesos() > 0) {
			auto &source = meso_guard.get();
			int16_t meso_rate = player->get_active_buffs()->get_buff_skill_info(source)->x; // Meso guard meso %
			int16_t meso_loss = static_cast<int16_t>(meso_rate * damage / 2 / 100);
			game_mesos mesos = player->get_inventory()->get_mesos();
			game_mesos new_mesos = mesos - meso_loss;

			if (new_mesos < 0) {
				// Special damage calculation for not having enough mesos
				double reduction = 2.0 - static_cast<double>(mesos / meso_loss) / 2.0;
				damage = static_cast<uint16_t>(damage / reduction);
				// This puts us pretty close to the damage observed clientside, needs improvement
				// TODO FIXME formula
			}
			else {
				damage /= 2;
				// Usually displays 1 below the actual damage but is sometimes accurate - no clue why
			}

			player->get_inventory()->set_mesos(new_mesos);
			player->get_stats()->damage_hp(static_cast<uint16_t>(damage));

			if (deadly_attack) {
				deadly_attack_func(false);
			}
			else if (mp_burn > 0) {
				player->get_stats()->damage_mp(mp_burn);
			}
			damage_applied = true;

			player->send_map(packets::skills::show_skill_effect(player->get_id(), source.get_skill_id()));
		}

		auto magic_guard = player->get_active_buffs()->get_magic_guard_source();
		if (magic_guard.is_initialized()) {
			if (deadly_attack) {
				deadly_attack_func(true);
			}
			else if (mp_burn > 0) {
				player->get_stats()->damage_mp(mp_burn);
				player->get_stats()->damage_hp(static_cast<uint16_t>(damage));
			}
			else {
				int16_t reduc = player->get_active_buffs()->get_buff_skill_info(magic_guard.get())->x;
				int32_t mp_damage = (damage * reduc) / 100;
				int32_t hp_damage = damage - mp_damage;

				if (mp_damage < mp || player->get_active_buffs()->has_infinity()) {
					player->get_stats()->damage_mp(mp_damage);
					player->get_stats()->damage_hp(hp_damage);
				}
				else if (mp_damage >= mp) {
					player->get_stats()->set_mp(0);
					player->get_stats()->damage_hp(hp_damage + (mp_damage - mp));
				}
			}
			damage_applied = true;
		}

		if (player->get_skills()->has_achilles()) {
			game_skill_id skill_id = player->get_skills()->get_achilles();
			double red = (2.0 - player->get_skills()->get_skill_info(skill_id)->x / 1000.0);

			player->get_stats()->damage_hp(static_cast<uint16_t>(damage / red));

			if (deadly_attack) {
				deadly_attack_func(false);
			}
			else if (mp_burn > 0) {
				player->get_stats()->damage_mp(mp_burn);
			}

			damage_applied = true;
		}

		if (!damage_applied) {
			if (deadly_attack) {
				deadly_attack_func(true);
			}
			else {
				player->get_stats()->damage_hp(static_cast<uint16_t>(damage));
			}

			if (mp_burn > 0) {
				player->get_stats()->damage_mp(mp_burn);
			}
		}

		player->get_active_buffs()->take_damage(damage);
	}
	player->send_map(packets::players::damage_player(player->get_id(), damage, mob_id, hit, type, stance, no_damage_id, pgmr));
}

auto player_handler::handle_facial_expression(ref_ptr<player> player, packet_reader &reader) -> void {
	int32_t face = reader.get<int32_t>();
	player->send_map(packets::players::face_expression(player->get_id(), face));
}

auto player_handler::handle_get_info(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_player_id player_id = reader.get<game_player_id>();
	if (auto info = channel_server::get_instance().get_player_data_provider().get_player(player_id)) {
		player->send(packets::players::show_info(info, reader.get<bool>()));
	}
}

auto player_handler::handle_heal(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_health hp = reader.get<game_health>();
	game_health mp = reader.get<game_health>();
	if (player->get_stats()->is_dead() || hp > 400 || mp > 1000 || (hp > 0 && mp > 0)) {
		// Hacking
		return;
	}
	player->get_stats()->modify_hp(hp);
	player->get_stats()->modify_mp(mp);
}

auto player_handler::handle_moving(ref_ptr<player> player, packet_reader &reader) -> void {
	if (reader.get<uint8_t>() != player->get_portal_count()) {
		// Portal count doesn't match, usually an indication of hacking
		return;
	}
	reader.reset(11);
	movement_handler::parse_movement(player.get(), reader);
	reader.reset(11);
	player->send_map(packets::players::show_moving(player->get_id(), reader.get_buffer(), reader.get_buffer_length()));

	if (player->get_foothold() == 0 && !player->is_using_gm_hide()) {
		// Player is floating in the air
		// GMs might be legitimately in this state (due to GM fly)
		// We shouldn't mess with them because they have the tools to get out of falling off the map anyway
		game_map_id map_id = player->get_map_id();
		point player_pos = player->get_pos();
		map *map = maps::get_map(map_id);

		point floor;
		if (map->find_floor(player_pos, floor) == search_result::not_found) {
			// There are no footholds below the player
			int8_t count = player->get_fall_counter();
			if (count > 3) {
				player->set_map(map_id);
			}
			else {
				player->set_fall_counter(++count);
			}
		}
	}
	else if (player->get_fall_counter() > 0) {
		player->set_fall_counter(0);
	}

	// Update all stalking players
	packet_builder stalking_addition_packet = packets::player::stalk_add_or_update_player(player);
	player->get_map()->run_function_players([&](ref_ptr<vana::channel_server::player> p) {
		if (p->is_stalking() && p != player) {
			p->send(stalking_addition_packet);
		}
	});

}

auto player_handler::handle_special_skills(ref_ptr<player> player, packet_reader &reader) -> void {
	game_skill_id skill_id = reader.get<game_skill_id>();
	switch (skill_id) {
		case constant::skill::hero::monster_magnet:
		case constant::skill::paladin::monster_magnet:
		case constant::skill::dark_knight::monster_magnet:
		case constant::skill::marksman::piercing_arrow:
		case constant::skill::fp_arch_mage::big_bang:
		case constant::skill::il_arch_mage::big_bang:
		case constant::skill::bishop::big_bang: {
			charge_or_stationary_skill_data info;
			info.skill_id = skill_id;
			info.level = reader.get<game_skill_level>();
			info.direction = reader.get<uint8_t>();
			info.weapon_speed = reader.get<uint8_t>();
			player->set_charge_or_stationary_skill(info);
			player->send_map(packets::skills::end_charge_or_stationary_skill(player->get_id(), info));
			break;
		}
		case constant::skill::chief_bandit::chakra: {
			game_stat dex = player->get_stats()->get_dex(true);
			game_stat luk = player->get_stats()->get_luk(true);
			int16_t recovery = player->get_skills()->get_skill_info(skill_id)->y;
			int16_t maximum = (luk * 66 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			int16_t minimum = (luk * 33 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			// Maximum = (luk * 6.6 + dex) * 0.2 * (recovery% / 100 + 1)
			// Minimum = (luk * 3.3 + dex) * 0.2 * (recovery% / 100 + 1)
			// I used 66 / 10 and 2 / 10 respectively to get 6.6 and 0.2 without using floating points
			player->get_stats()->modify_hp(randomizer::rand<int16_t>(maximum, minimum));
			break;
		}
	}
}

auto player_handler::handle_monster_book(ref_ptr<player> player, packet_reader &reader) -> void {
	game_item_id card_id = reader.get<game_item_id>();
	if (card_id != 0 && player->get_monster_book()->get_card(card_id) == 0) {
		// Hacking
		return;
	}
	game_mob_id new_cover = 0;
	if (card_id != 0) {
		optional<game_mob_id> mob_cover_id =
			channel_server::get_instance().get_item_data_provider().get_mob_id(card_id);

		if (mob_cover_id.is_initialized()) {
			new_cover = mob_cover_id.get();
		}
		else {
			// Hacking?
			new_cover = 0;
		}
	}
	player->get_monster_book()->set_cover(new_cover);
	player->send(packets::monster_book::change_cover(card_id));
}

auto player_handler::handle_admin_messenger(ref_ptr<player> player_value, packet_reader &reader) -> void {
	if (!player_value->is_admin()) {
		// Hacking
		return;
	}
	ref_ptr<player> receiver = nullptr;
	bool has_target = reader.get<int8_t>() == 2;
	int8_t sort = reader.get<int8_t>();
	bool use_whisper = reader.get<bool>();
	int8_t type = reader.get<int8_t>();
	game_player_id character_id = reader.get<game_player_id>();

	if (player_value->get_id() != character_id) {
		return;
	}

	string line1 = reader.get<string>();
	string line2 = reader.get<string>();
	string line3 = reader.get<string>();
	string line4 = reader.get<string>();
	string line5 = reader.get<string>();
	if (has_target) {
		receiver = channel_server::get_instance().get_player_data_provider().get_player(reader.get<string>());
	}

	int32_t time = 15;
	switch (type) {
		case 1: time = 30; break;
		case 2: time = 60; break;
	}

	channel_server::get_instance().get_maple_tvs().add_message(player_value, receiver, line1, line2, line3, line4, line5, 5075000 + type, time);
	if (sort == 1) {
		out_stream output;
		output << player_value->get_medal_name() << " : " << line1 << line2 << line3 << line4 << line5;

		auto &builder = packets::inventory::show_super_megaphone(output.str(), use_whisper);
		channel_server::get_instance().send_world(
			vana::packets::prepend(builder, [](packet_builder &header) {
				header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
			}));
	}
}

auto player_handler::handle_stalk(ref_ptr<player> player, packet_reader &reader) -> void {
	if (!player->is_admin()) {
		return;
	}

	if (player->is_stalking()) {
		// No need to start _again_
		return;
	}

	player->set_stalking(true);
	player->send(packets::player::stalk_result(player));
}

auto player_handler::use_bomb_skill(ref_ptr<player> player, packet_reader &reader) -> void {
	// TODO FIXME packet
	// Ignore this position in favor of player->get_pos()?
	wide_point player_pos = reader.get<wide_point>();
	game_charge_time charge = reader.get<game_charge_time>();
	game_skill_id skill_id = reader.get<game_skill_id>();

	if (player->get_skills()->get_skill_level(skill_id) == 0) {
		// Hacking
		return;
	}
	if (!ext::in_range_inclusive(charge, 0, 1000)) {
		// Hacking
		return;
	}

	player->send_map(packets::players::use_bomb_attack(player->get_id(), charge, skill_id, player_pos));
}

auto player_handler::use_melee_attack(ref_ptr<player> player, packet_reader &reader) -> void {
	attack_data attack = compile_attack(player, reader, data::type::skill_type::melee);
	if (attack.portals != player->get_portal_count()) {
		// Usually evidence of hacking
		return;
	}
	game_skill_id mastery_id = player->get_skills()->get_mastery();
	int8_t damaged_targets = 0;
	game_skill_id skill_id = attack.skill_id;
	game_skill_level level = attack.skill_level;

	if (skill_id != constant::skill::all::regular_attack) {
		if (skills::use_attack_skill(player, skill_id) == result::failure) {
			// Most likely hacking, could feasibly be lag
			return;
		}
	}

	player->send_map(packets::players::use_melee_attack(player->get_id(), mastery_id, player->get_skills()->get_skill_level(mastery_id), attack));

	game_map_id map_id = player->get_map_id();
	map *map = maps::get_map(map_id);
	if (map != player->get_map()) {
		// Hacking or something
		return;
	}

	auto pickpocket = player->get_active_buffs()->get_pickpocket_source();
	bool ppok = !attack.is_meso_explosion && pickpocket.is_initialized();
	point origin;
	vector<game_damage> pp_damages;
	auto picking = !pickpocket.is_initialized() ?
		nullptr :
		player->get_active_buffs()->get_buff_skill_info(pickpocket.get());
	bool reflect_applied = player->has_gm_benefits();

	for (const auto &target : attack.damages) {
		game_damage target_total = 0;
		int8_t connected_hits = 0;
		auto mob = map->get_mob(target.first);
		if (mob == nullptr) {
			continue;
		}
		if (mob->has_weapon_reflection() && !reflect_applied) {
			auto &reflect = mob->get_weapon_reflection().get();
			int32_t amount = randomizer::range<int32_t>(reflect.val, reflect.reflection);
			player->get_stats()->modify_hp(-amount);
			reflect_applied = true;
		}

		origin = mob->get_pos(); // Info for pickpocket before mob is set to nullptr (in the case that mob dies)
		for (const auto &hit : target.second) {
			game_damage damage = hit;
			if (damage != 0) {
				connected_hits++;
				target_total += damage;
			}
			if (ppok && randomizer::percentage<uint16_t>() < picking->prop) {
				 // Make sure this is a melee attack and not meso explosion, plus pickpocket being active
				pp_damages.push_back(damage);
			}
			if (mob == nullptr) {
				if (ppok) {
					// Roll along after the mob is dead to finish getting damage values for pickpocket
					continue;
				}
				break;
			}
			if (skill_id == constant::skill::paladin::heavens_hammer) {
				damage = (mob->is_boss() ? constant::stat::max_damage : (mob->get_hp() - 1));
			}
			else if (skill_id == constant::skill::bandit::steal && !mob->is_boss()) {
				drop_handler::do_drops(player->get_id(), map_id, mob->get_level(), mob->get_mob_id(), mob->get_pos(), false, false, mob->get_taunt_effect(), true);
			}
			int32_t temp_hp = mob->get_hp();
			mob->apply_damage(player->get_id(), damage);
			if (temp_hp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
			}
		}
		if (target_total > 0) {
			if (mob != nullptr && mob->get_hp() > 0) {
				mob_handler::handle_mob_status(player->get_id(), mob, skill_id, level, player->get_inventory()->get_equipped_id(constant::equip_slot::weapon), connected_hits);
				if (mob->get_hp() < mob->get_self_destruct_hp()) {
					mob->explode();
				}
			}
			damaged_targets++;
		}
		uint8_t pp_size = static_cast<uint8_t>(pp_damages.size());
		for (uint8_t pickpocket = 0; pickpocket < pp_size; ++pickpocket) {
			// Drop stuff for Pickpocket
			point pp_pos = origin;
			pp_pos.x += (pp_size % 2 == 0 ? 5 : 0) + (pp_size / 2) - 20 * ((pp_size / 2) - pickpocket);

			int32_t pp_mesos = (pp_damages[pickpocket] * picking->x) / 10000; // TODO FIXME formula
			drop *pp_drop = new drop{player->get_map_id(), pp_mesos, pp_pos, player->get_id(), true};
			pp_drop->set_time(100);
			vana::timer::timer::create(
				[pp_drop, origin](const time_point &now) {
					pp_drop->do_drop(origin);
				},
				vana::timer::id{
					vana::timer::type::pickpocket_timer,
					player->get_id(),
					player->get_active_buffs()->get_pickpocket_counter()
				},
				nullptr,
				milliseconds{175 * pickpocket});
		}
		pp_damages.clear();
	}

	if (player->get_skills()->has_energy_charge()) {
		player->get_active_buffs()->increase_energy_charge_level(damaged_targets);
	}

	if (player->get_skills()->has_dark_sight_interruption_skill()) {
		auto source = player->get_active_buffs()->get_dark_sight_source();
		if (source.is_initialized()) {
			skills::stop_skill(player, source.get());
		}
	}

	switch (skill_id) {
		case constant::skill::chief_bandit::meso_explosion: {
			uint8_t items = reader.get<int8_t>();
			for (uint8_t i = 0; i < items; i++) {
				game_map_object obj_id = reader.get<game_map_object>();
				reader.unk<uint8_t>(); // Not sure what this is for, but it isn't used?
				if (drop *drop = map->get_drop(obj_id)) {
					if (!drop->is_mesos()) {
						// Hacking
						return;
					}

					// Note that officially there's also a main delay used for skills
					// that depends on skillid (and would be 1000 in most cases) and
					// other values (type of weapon used, active buffs)
					int16_t delay = std::min(1000, 100 * (i % 5));
					map->send(packets::drops::explode_drop(drop->get_id(), delay));
					map->remove_drop(drop->get_id());
					delete drop;
				}
			}
			break;
		}
		case constant::skill::marauder::energy_drain:
		case constant::skill::thunder_breaker::energy_drain: {
			int32_t hp_recover = static_cast<int32_t>(attack.total_damage * player->get_skills()->get_skill_info(skill_id)->x / 100);
			if (hp_recover > player->get_stats()->get_max_hp()) {
				player->get_stats()->set_hp(player->get_stats()->get_max_hp());
			}
			else {
				player->get_stats()->modify_hp(static_cast<int16_t>(hp_recover));
			}
			break;
		}
		case constant::skill::crusader::sword_panic: // Crusader finishers
		case constant::skill::crusader::sword_coma:
		case constant::skill::crusader::axe_panic:
		case constant::skill::crusader::axe_coma:
		case constant::skill::dawn_warrior::panic:
		case constant::skill::dawn_warrior::coma:
			player->get_active_buffs()->reset_combo();
			break;
		case constant::skill::night_walker::poison_bomb: {
			auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
			mist *mist_value = new mist{player->get_map_id(), player, skill->buff_time, skill->dimensions.move(attack.projectile_pos), skill_id, level, true};
			break;
		}
		case constant::skill::crusader::shout:
		case constant::skill::gm::super_dragon_roar:
		case constant::skill::super_gm::super_dragon_roar:
			break;
		case constant::skill::dragon_knight::dragon_roar: {
			int16_t x_property = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level)->x;
			uint16_t reduction = (player->get_stats()->get_max_hp() / 100) * x_property;
			if (reduction < player->get_stats()->get_hp()) {
				player->get_stats()->damage_hp(reduction);
			}
			else {
				// Hacking
				return;
			}
			buffs::add_buff(player, constant::skill::dragon_knight::dragon_roar, level, 0);
			break;
		}
		case constant::skill::dragon_knight::sacrifice: {
			if (attack.total_damage > 0) {
				int16_t x_property = player->get_skills()->get_skill_info(skill_id)->x;
				int32_t hp_damage = static_cast<int32_t>(attack.total_damage * x_property / 100);
				if (hp_damage >= player->get_stats()->get_hp()) {
					hp_damage = player->get_stats()->get_hp() - 1;
				}
				if (hp_damage > 0) {
					player->get_stats()->damage_hp(hp_damage);
				}
			}
			break;
		}
		case constant::skill::white_knight::charge_blow: {
			game_skill_level skill_level = player->get_skills()->get_skill_level(constant::skill::paladin::advanced_charge);
			int16_t x_property = 0;
			if (skill_level > 0) {
				x_property = channel_server::get_instance().get_skill_data_provider().get_skill(constant::skill::paladin::advanced_charge, skill_level)->x;
			}
			if ((x_property != 100) && (x_property == 0 || randomizer::percentage<int16_t>() > (x_property - 1))) {
				player->get_active_buffs()->stop_charge();
			}
			break;
		}
		default:
			if (attack.total_damage > 0) {
				player->get_active_buffs()->add_combo();
			}
	}
}

auto player_handler::use_ranged_attack(ref_ptr<player> player, packet_reader &reader) -> void {
	attack_data attack = compile_attack(player, reader, data::type::skill_type::ranged);
	if (attack.portals != player->get_portal_count()) {
		// Usually evidence of hacking
		return;
	}
	game_skill_id mastery_id = player->get_skills()->get_mastery();
	game_skill_id skill_id = attack.skill_id;
	game_skill_level level = attack.skill_level;
	int8_t damaged_targets = 0;

	if (skills::use_attack_skill_ranged(player, skill_id, attack.star_pos, attack.cash_star_pos, attack.star_id) == result::failure) {
		// Most likely hacking, could feasibly be lag
		return;
	}

	player->send_map(packets::players::use_ranged_attack(player->get_id(), mastery_id, player->get_skills()->get_skill_level(mastery_id), attack));

	switch (skill_id) {
		case constant::skill::bowmaster::hurricane:
		case constant::skill::wind_archer::hurricane:
		case constant::skill::corsair::rapid_fire:
			if (!player->has_charge_or_stationary_skill()) {
				charge_or_stationary_skill_data info;
				info.skill_id = skill_id;
				info.direction = attack.animation;
				info.weapon_speed = attack.weapon_speed;
				info.level = level;
				player->set_charge_or_stationary_skill(info);
				player->send_map(packets::skills::end_charge_or_stationary_skill(player->get_id(), info));
			}
			break;
	}

	int32_t max_hp = 0;
	game_damage first_hit = 0;
	bool reflect_applied = player->has_gm_benefits();
	for (const auto &target : attack.damages) {
		game_map_object map_mob_id = target.first;
		auto mob = player->get_map()->get_mob(map_mob_id);
		if (mob == nullptr) {
			continue;
		}
		if (mob->has_weapon_reflection() && !reflect_applied) {
			auto &reflect = mob->get_weapon_reflection().get();
			int32_t amount = randomizer::range<int32_t>(reflect.val, reflect.reflection);
			player->get_stats()->modify_hp(-amount);
			reflect_applied = true;
		}
		game_damage target_total = 0;
		int8_t connected_hits = 0;

		for (const auto &hit : target.second) {
			game_damage damage = hit;

			if (damage != 0) {
				connected_hits++;
				target_total += damage;
			}
			if (first_hit == 0) {
				first_hit = damage;
			}
			if (mob == nullptr) {
				continue;
			}
			max_hp = mob->get_max_hp();
			if (skill_id == constant::skill::ranger::mortal_blow || skill_id == constant::skill::sniper::mortal_blow) {
				auto sk = player->get_skills()->get_skill_info(skill_id);
				int32_t hp_percentage = max_hp * sk->x / 100; // Percentage of HP required for Mortal Blow activation
				if (mob->get_hp() < hp_percentage && randomizer::percentage<int16_t>() < sk->y) {
					damage = mob->get_hp();
				}
			}

			int32_t temp_hp = mob->get_hp();
			mob->apply_damage(player->get_id(), damage);
			if (temp_hp <= damage) {
				mob = nullptr;
			}
			else if (skill_id == constant::skill::outlaw::homing_beacon || skill_id == constant::skill::corsair::bullseye) {
				buffs::add_buff(player, skill_id, level, 0, map_mob_id);
			}
		}
		if (target_total > 0) {
			if (mob != nullptr && mob->get_hp() > 0) {
				mob_handler::handle_mob_status(player->get_id(), mob, skill_id, level, player->get_inventory()->get_equipped_id(constant::equip_slot::weapon), connected_hits, first_hit);
				if (mob->get_hp() < mob->get_self_destruct_hp()) {
					mob->explode();
				}
			}
			damaged_targets++;
		}
	}

	if (player->get_skills()->has_energy_charge()) {
		player->get_active_buffs()->increase_energy_charge_level(damaged_targets);
	}

	if (player->get_skills()->has_dark_sight_interruption_skill()) {
		auto source = player->get_active_buffs()->get_dark_sight_source();
		if (source.is_initialized()) {
			skills::stop_skill(player, source.get());
		}
	}

	switch (skill_id) {
		case constant::skill::night_walker::vampire:
		case constant::skill::assassin::drain: {
			int16_t x_property = player->get_skills()->get_skill_info(skill_id)->x;
			int32_t hp_recover = static_cast<int32_t>(attack.total_damage * x_property / 100);
			game_health player_max_hp = player->get_stats()->get_max_hp();
			if (hp_recover > max_hp) {
				hp_recover = max_hp;
			}
			if (hp_recover > (player_max_hp / 2)) {
				hp_recover = player_max_hp / 2;
			}
			if (hp_recover > player_max_hp) {
				player->get_stats()->set_hp(player_max_hp);
			}
			else {
				player->get_stats()->modify_hp(hp_recover);
			}
			break;
		}
		case constant::skill::dawn_warrior::soul_blade:
			if (attack.total_damage > 0) {
				player->get_active_buffs()->add_combo();
			}
			break;
	}
}

auto player_handler::use_spell_attack(ref_ptr<player> player, packet_reader &reader) -> void {
	const attack_data &attack = compile_attack(player, reader, data::type::skill_type::magic);
	if (attack.portals != player->get_portal_count()) {
		// Usually evidence of hacking
		return;
	}

	game_skill_id skill_id = attack.skill_id;
	game_skill_level level = attack.skill_level;

	if (!attack.is_heal) {
		// Heal is sent as both an attack and as a used skill - it's only sometimes an attack
		if (skills::use_attack_skill(player, skill_id) == result::failure) {
			// Most likely hacking, could feasibly be lag
			return;
		}
	}

	player->send_map(packets::players::use_spell_attack(player->get_id(), attack));

	mp_eater_data eater;
	eater.skill_id = player->get_skills()->get_mp_eater();
	eater.level = player->get_skills()->get_skill_level(eater.skill_id);
	if (eater.level > 0) {
		auto skill_info = channel_server::get_instance().get_skill_data_provider().get_skill(eater.skill_id, eater.level);
		eater.prop = skill_info->prop;
		eater.x = skill_info->x;
	}

	bool reflect_applied = player->has_gm_benefits();
	for (const auto &target : attack.damages) {
		game_damage target_total = 0;
		game_map_object map_mob_id = target.first;
		int8_t connected_hits = 0;
		auto mob = player->get_map()->get_mob(map_mob_id);
		if (mob == nullptr) {
			continue;
		}
		if (attack.is_heal && !mob->is_undead()) {
			// Hacking
			return;
		}
		if (mob->has_magic_reflection() && !reflect_applied) {
			auto &reflect = mob->get_magic_reflection().get();
			int32_t amount = randomizer::range<int32_t>(reflect.val, reflect.reflection);
			player->get_stats()->modify_hp(-amount);
			reflect_applied = true;
		}

		for (const auto &hit : target.second) {
			game_damage damage = hit;
			if (damage != 0) {
				connected_hits++;
				target_total += damage;
			}
			if (damage != 0 && eater.level != 0 && !eater.used) {
				// MP Eater
				mob->mp_eat(player, &eater);
			}
			int32_t temp_hp = mob->get_hp();
			mob->apply_damage(player->get_id(), damage);
			if (temp_hp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && target_total > 0 && mob->get_hp() > 0) {
			mob_handler::handle_mob_status(player->get_id(), mob, skill_id, level, player->get_inventory()->get_equipped_id(constant::equip_slot::weapon), connected_hits);
			if (mob->get_hp() < mob->get_self_destruct_hp()) {
				mob->explode();
			}
		}
	}

	switch (skill_id) {
		case constant::skill::fp_mage::poison_mist:
		case constant::skill::blaze_wizard::flame_gear: {
			auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
			mist *value = new mist{player->get_map_id(), player, skill->buff_time, skill->dimensions.move(player->get_pos()), skill_id, level, true};
			break;
		}
	}
}

auto player_handler::use_energy_charge_attack(ref_ptr<player> player, packet_reader &reader) -> void {
	attack_data attack = compile_attack(player, reader, data::type::skill_type::energy_charge);
	game_skill_id mastery_id = player->get_skills()->get_mastery();
	player->send_map(packets::players::use_energy_charge_attack(player->get_id(), mastery_id, player->get_skills()->get_skill_level(mastery_id), attack));

	game_skill_id skill_id = attack.skill_id;
	game_skill_level level = attack.skill_level;
	bool reflect_applied = player->has_gm_benefits();

	for (const auto &target : attack.damages) {
		game_damage target_total = 0;
		game_map_object map_mob_id = target.first;
		int8_t connected_hits = 0;
		auto mob = player->get_map()->get_mob(map_mob_id);
		if (mob == nullptr) {
			continue;
		}
		if (mob->has_weapon_reflection() && !reflect_applied) {
			auto &reflect = mob->get_weapon_reflection().get();
			int32_t amount = randomizer::range<int32_t>(reflect.val, reflect.reflection);
			player->get_stats()->modify_hp(-amount);
			reflect_applied = true;
		}

		for (const auto &hit : target.second) {
			game_damage damage = hit;
			if (damage != 0) {
				connected_hits++;
				target_total += damage;
			}
			int32_t temp_hp = mob->get_hp();
			mob->apply_damage(player->get_id(), damage);
			if (temp_hp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && target_total > 0 && mob->get_hp() > 0) {
			mob_handler::handle_mob_status(player->get_id(), mob, skill_id, level, player->get_inventory()->get_equipped_id(constant::equip_slot::weapon), connected_hits);
			if (mob->get_hp() < mob->get_self_destruct_hp()) {
				mob->explode();
			}
		}
	}
}

auto player_handler::use_summon_attack(ref_ptr<player> player, packet_reader &reader) -> void {
	attack_data attack = compile_attack(player, reader, data::type::skill_type::summon);
	summon *summon = player->get_summons()->get_summon(attack.summon_id);
	if (summon == nullptr) {
		// Hacking or some other form of tomfoolery
		return;
	}
	player->send_map(packets::players::use_summon_attack(player->get_id(), attack));
	for (const auto &target : attack.damages) {
		game_damage target_total = 0;
		game_map_object map_mob_id = target.first;
		int8_t connected_hits = 0;
		auto mob = player->get_map()->get_mob(map_mob_id);
		if (mob == nullptr) {
			continue;
		}
		for (const auto &hit : target.second) {
			game_damage damage = hit;
			if (damage != 0) {
				connected_hits++;
				target_total += damage;
			}
			int32_t temp_hp = mob->get_hp();
			mob->apply_damage(player->get_id(), damage);
			if (temp_hp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && target_total > 0 && mob->get_hp() > 0) {
			mob_handler::handle_mob_status(player->get_id(), mob, summon->get_skill_id(), summon->get_skill_level(), player->get_inventory()->get_equipped_id(constant::equip_slot::weapon), connected_hits);
			if (mob->get_hp() < mob->get_self_destruct_hp()) {
				mob->explode();
			}
		}
	}

	if (summon->get_skill_id() == constant::skill::outlaw::gaviota) {
		summon_handler::remove_summon(player, attack.summon_id, false, summon_messages::none, false);
	}
}

auto player_handler::compile_attack(ref_ptr<player> player, packet_reader &reader, data::type::skill_type skill_type) -> attack_data {
	attack_data attack;
	int8_t targets = 0;
	int8_t hits = 0;
	game_skill_id skill_id = 0;
	bool meso_explosion = false;
	bool shadow_meso = false;

	if (skill_type != data::type::skill_type::summon) {
		attack.portals = reader.get<uint8_t>();
		uint8_t t_byte = reader.get<uint8_t>();
		skill_id = reader.get<game_skill_id>();
		targets = t_byte / 0x10;
		hits = t_byte % 0x10;

		if (skill_id != constant::skill::all::regular_attack) {
			attack.skill_level = player->get_skills()->get_skill_level(skill_id);
		}

		reader.skip<game_checksum>(); // Unk, strange constant that doesn't seem to change
		// Things atttemped: Map changes, character changes, job changes, skill changes, position changes, hitting enemies
		// It appears as 0xF9B16E60 which is 4189154912 unsigned, -105812384 signed, doesn't seem to be a size, probably a CRC
		reader.skip<game_checksum>();

		switch (skill_id) {
			case constant::skill::hermit::shadow_meso:
				attack.is_shadow_meso = true;
				shadow_meso = true;
				break;
			case constant::skill::chief_bandit::meso_explosion:
				attack.is_meso_explosion = true;
				meso_explosion = true;
				break;
			case constant::skill::cleric::heal:
				attack.is_heal = true;
				break;
			case constant::skill::gunslinger::grenade:
			case constant::skill::brawler::corkscrew_blow:
			case constant::skill::thunder_breaker::corkscrew_blow:
			case constant::skill::bowmaster::hurricane:
			case constant::skill::wind_archer::hurricane:
			case constant::skill::marksman::piercing_arrow:
			case constant::skill::night_walker::poison_bomb:
			case constant::skill::corsair::rapid_fire:
			case constant::skill::fp_arch_mage::big_bang:
			case constant::skill::il_arch_mage::big_bang:
			case constant::skill::bishop::big_bang:
				attack.is_charge_skill = true;
				attack.charge = reader.get<game_charge_time>();
				break;
		}

		attack.display = reader.get<uint8_t>();
		attack.animation = reader.get<uint8_t>();
		attack.weapon_class = reader.get<uint8_t>();
		attack.weapon_speed = reader.get<uint8_t>();
		attack.ticks = reader.get<game_tick_count>();
	}
	else {
		attack.summon_id = reader.get<game_summon_id>(); // game_summon_id, not to be confused with summon game_skill_id
		attack.ticks = reader.get<game_tick_count>();
		attack.animation = reader.get<uint8_t>();
		targets = reader.get<int8_t>();
		hits = 1;
	}

	if (skill_type == data::type::skill_type::ranged) {
		game_inventory_slot star_slot = reader.get<game_inventory_slot>();
		game_inventory_slot cs_star = reader.get<game_inventory_slot>();
		attack.star_pos = star_slot;
		attack.cash_star_pos = cs_star;
		reader.unk<uint8_t>(); // 0x00 = AoE?
		if (!shadow_meso) {
			if (player->get_active_buffs()->has_shadow_stars() && skill_id != constant::skill::night_lord::taunt) {
				attack.star_id = reader.get<int32_t>();
			}
			else if (cs_star > 0) {
				if (item *item = player->get_inventory()->get_item(constant::inventory::cash, cs_star)) {
					attack.star_id = item->get_id();
				}
			}
			else if (star_slot > 0) {
				if (item *item = player->get_inventory()->get_item(constant::inventory::use, star_slot)) {
					attack.star_id = item->get_id();
				}
			}
		}
	}

	attack.targets = targets;
	attack.hits = hits;
	attack.skill_id = skill_id;

	for (int8_t i = 0; i < targets; ++i) {
		game_map_object map_mob_id = reader.get<game_map_object>();
		// hitAction is calculated using: rand() % hitAnimation + 7
		// However, it doesn't match with the standard 6 outcome (when there's only 1 animation)
		// This should be -1 when there's no hit animation
		auto hitAction = reader.get<int8_t>();
		
		auto tmp = reader.get<uint8_t>();
		// The imgActionNodeIndex is the wz property node index of the action/animation of the mob
		// This would be used for mob position checking (in combination with frameIdx)
		auto imgActionNodeIndex = (tmp & 0x7F);
		auto facingLeft = (int)((tmp >> 7) & 1);

		auto frameIdx = reader.get<int8_t>(); // Mob animation frame index

		reader.skip<uint8_t>(); // Damage stats calculator index. Bit 8 == mob doomed
		
		reader.skip<point>(); // Mob pos
		reader.skip<point>(); // Damage pos
		if (!meso_explosion) {
			reader.skip<uint16_t>(); // Delay per hit
		}
		else {
			hits = reader.get<int8_t>(); // Hits for Meso Explosion
		}
		for (int8_t k = 0; k < hits; ++k) {
			game_damage damage = reader.get<game_damage>();
			attack.damages[map_mob_id].push_back(damage);
			attack.total_damage += damage;
		}
		if (skill_type != data::type::skill_type::summon) {
			reader.skip<game_checksum>();
		}
	}

	if (skill_type == data::type::skill_type::ranged) {
		attack.projectile_pos = reader.get<point>();
	}
	attack.player_pos = reader.get<point>();

	if (skill_id == constant::skill::night_walker::poison_bomb) {
		attack.projectile_pos = reader.get<point>();
	}
	return attack;
}

}
}