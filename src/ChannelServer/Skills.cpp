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
#include "Skills.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/PacketReader.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/Randomizer.hpp"
#include "Common/SkillConstants.hpp"
#include "Common/SkillDataProvider.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/Timer.hpp"
#include "ChannelServer/Buffs.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/GmPacket.hpp"
#include "ChannelServer/Inventory.hpp"
#include "ChannelServer/MapPacket.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Mist.hpp"
#include "ChannelServer/MobHandler.hpp"
#include "ChannelServer/MysticDoor.hpp"
#include "ChannelServer/Party.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/SkillsPacket.hpp"
#include "ChannelServer/SummonHandler.hpp"
#include <functional>

namespace vana {
namespace channel_server {

auto skills::add_skill(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_skill_id skill_id = reader.get<game_skill_id>();
	if (!game_logic_utilities::is_beginner_skill(skill_id)) {
		if (player->get_stats()->get_sp() == 0) {
			// Hacking
			return;
		}
		if (!player->is_gm() && !game_logic_utilities::skill_matches_job(skill_id, player->get_stats()->get_job())) {
			// Hacking
			return;
		}
	}
	if (player->get_skills()->add_skill_level(skill_id, 1) && !game_logic_utilities::is_beginner_skill(skill_id)) {
		player->get_stats()->set_sp(player->get_stats()->get_sp() - 1);
	}
}

auto skills::cancel_skill(ref_ptr<player> player, packet_reader &reader) -> void {
	int32_t identifier = reader.get<int32_t>();

	// Both buffs and "standing" skills e.g. Hurricane go through this packet
	// Handle "standing" skills here, otherwise dispatch to buffs

	switch (identifier) {
		case vana::skills::bowmaster::hurricane:
		case vana::skills::wind_archer::hurricane:
		case vana::skills::marksman::piercing_arrow:
		case vana::skills::fp_arch_mage::big_bang:
		case vana::skills::il_arch_mage::big_bang:
		case vana::skills::bishop::big_bang:
		case vana::skills::corsair::rapid_fire:
			player->send_map(packets::skills::end_charge_or_stationary_skill(player->get_id(), player->get_charge_or_stationary_skill()));
			player->set_charge_or_stationary_skill(charge_or_stationary_skill_data{});
			return;
	}

	stop_skill(player, player->get_active_buffs()->translate_to_source(identifier));
}

auto skills::stop_skill(ref_ptr<player> player, const buff_source &source, bool from_timer) -> void {
	switch (source.get_type()) {
		case buff_source_type::item:
		case buff_source_type::skill:
			if (source.get_skill_level() == 0) {
				// Hacking
				return;
			}
			break;
		case buff_source_type::mob_skill:
			if (source.get_mob_skill_level() == 0) {
				// Hacking
				return;
			}
			break;
	}

	buffs::end_buff(player, source, from_timer);

	if (source.get_skill_id() == vana::skills::super_gm::hide) {
		player->send(packets::gm::end_hide());
		player->get_map()->gm_hide_change(player);
	}
}

auto skills::get_affected_party_members(party *party, int8_t affected, int8_t members) -> const vector<ref_ptr<player>> {
	vector<ref_ptr<player>> ret;
	if (affected & game_logic_utilities::get_party_member1(members)) {
		ret.push_back(party->get_member_by_index(1));
	}
	if (affected & game_logic_utilities::get_party_member2(members)) {
		ret.push_back(party->get_member_by_index(2));
	}
	if (affected & game_logic_utilities::get_party_member3(members)) {
		ret.push_back(party->get_member_by_index(3));
	}
	if (affected & game_logic_utilities::get_party_member4(members)) {
		ret.push_back(party->get_member_by_index(4));
	}
	if (affected & game_logic_utilities::get_party_member5(members)) {
		ret.push_back(party->get_member_by_index(5));
	}
	if (affected & game_logic_utilities::get_party_member6(members)) {
		ret.push_back(party->get_member_by_index(6));
	}
	return ret;
}

auto skills::use_skill(ref_ptr<player> player_value, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_skill_id skill_id = reader.get<game_skill_id>();
	int16_t added_info = 0;
	game_skill_level level = reader.get<game_skill_level>();
	uint8_t type = 0;
	uint8_t direction = 0;
	if (level == 0 || player_value->get_skills()->get_skill_level(skill_id) != level) {
		// Hacking
		return;
	}

	if (player_value->get_stats()->is_dead()) {
		// Possibly hacking, possibly lag
		return;
	}

	auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
	if (skill_id == vana::skills::priest::mystic_door) {
		point origin = reader.get<point>();
		mystic_door_result result = player_value->get_skills()->open_mystic_door(origin, skill->buff_time);
		if (result == mystic_door_result::hacking) {
			return;
		}
		if (result == mystic_door_result::no_space || result == mystic_door_result::no_door_points) {
			// TODO FIXME packet?
			// There's probably some packet to indicate failure
			return;
		}
	}

	if (apply_skill_costs(player_value, skill_id, level) == result::failure) {
		// Most likely hacking, could feasibly be lag
		return;
	}

	switch (skill_id) {
		case vana::skills::priest::mystic_door:
			// Prevent the default case from executing, there's no packet data left for it
			break;
		case vana::skills::brawler::mp_recovery: {
			game_health mod_hp = player_value->get_stats()->get_max_hp() * skill->x / 100;
			game_health heal_mp = mod_hp * skill->y / 100;
			player_value->get_stats()->modify_hp(-mod_hp);
			player_value->get_stats()->modify_mp(heal_mp);
			break;
		}
		case vana::skills::shadower::smokescreen: {
			point origin = reader.get<point>();
			mist *m = new mist{
				player_value->get_map_id(),
				player_value,
				skill->buff_time,
				skill->dimensions.move(player_value->get_pos()),
				skill_id,
				level};
			break;
		}
		case vana::skills::corsair::battleship:
			// TODO FIXME hacking? Remove?
			if (player_value->get_active_buffs()->get_battleship_hp() == 0) {
				player_value->get_active_buffs()->reset_battleship_hp();
			}
			break;
		case vana::skills::crusader::armor_crash:
		case vana::skills::white_knight::magic_crash:
		case vana::skills::dragon_knight::power_crash: {
			// Might be CRC
			reader.unk<uint32_t>();
			uint8_t mobs = reader.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				game_map_object map_mob_id = reader.get<game_map_object>();
				if (auto mob = player_value->get_map()->get_mob(map_mob_id)) {
					if (randomizer::percentage<uint16_t>() < skill->prop) {
						mob->do_crash_skill(skill_id);
					}
				}
			}
			break;
		}
		case vana::skills::hero::monster_magnet:
		case vana::skills::paladin::monster_magnet:
		case vana::skills::dark_knight::monster_magnet: {
			int32_t mobs = reader.get<int32_t>();
			for (int32_t k = 0; k < mobs; k++) {
				game_map_object map_mob_id = reader.get<game_map_object>();
				uint8_t success = reader.get<uint8_t>();
				player_value->send_map(packets::skills::show_magnet_success(map_mob_id, success));
			}
			direction = reader.get<uint8_t>();
			break;
		}
		case vana::skills::fp_wizard::slow:
		case vana::skills::il_wizard::slow:
		case vana::skills::blaze_wizard::slow:
		case vana::skills::page::threaten:
			// Might be CRC
			reader.unk<uint32_t>();
			// Intentional fallthrough
		case vana::skills::fp_mage::seal:
		case vana::skills::il_mage::seal:
		case vana::skills::blaze_wizard::seal:
		case vana::skills::priest::doom:
		case vana::skills::hermit::shadow_web:
		case vana::skills::night_walker::shadow_web:
		case vana::skills::shadower::ninja_ambush:
		case vana::skills::night_lord::ninja_ambush: {
			uint8_t mobs = reader.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				if (auto mob = player_value->get_map()->get_mob(reader.get<int32_t>())) {
					mob_handler::handle_mob_status(player_value->get_id(), mob, skill_id, level, 0, 0);
				}
			}
			break;
		}
		case vana::skills::bishop::heros_will:
		case vana::skills::il_arch_mage::heros_will:
		case vana::skills::fp_arch_mage::heros_will:
		case vana::skills::dark_knight::heros_will:
		case vana::skills::hero::heros_will:
		case vana::skills::paladin::heros_will:
		case vana::skills::night_lord::heros_will:
		case vana::skills::shadower::heros_will:
		case vana::skills::bowmaster::heros_will:
		case vana::skills::marksman::heros_will:
		case vana::skills::buccaneer::heros_will:
		case vana::skills::corsair::heros_will:
			player_value->get_active_buffs()->remove_debuff(mob_skills::seduce);
			break;
		case vana::skills::priest::dispel: {
			int8_t affected = reader.get<int8_t>();
			player_value->get_active_buffs()->use_player_dispel();
			if (party *party = player_value->get_party()) {
				const auto members = get_affected_party_members(party, affected, party->get_members_count());
				for (const auto &party_member : members) {
					if (party_member != nullptr && party_member != player_value && party_member->get_map() == player_value->get_map()) {
						if (randomizer::percentage<uint16_t>() < skill->prop) {
							party_member->send(packets::skills::show_skill(party_member->get_id(), skill_id, level, direction, true, true));
							party_member->send_map(packets::skills::show_skill(party_member->get_id(), skill_id, level, direction, true));
							party_member->get_active_buffs()->use_player_dispel();
						}
					}
				}
			}

			reader.unk<int16_t>();

			affected = reader.get<int8_t>();
			for (int8_t k = 0; k < affected; k++) {
				game_map_object map_mob_id = reader.get<game_map_object>();
				if (auto mob = player_value->get_map()->get_mob(map_mob_id)) {
					if (randomizer::percentage<uint16_t>() < skill->prop) {
						mob->dispel_buffs();
					}
				}
			}
			break;
		}
		case vana::skills::cleric::heal: {
			uint16_t heal_rate = skill->hp_prop;
			if (heal_rate > 100) {
				heal_rate = 100;
			}
			party *party = player_value->get_party();
			int8_t party_players = (party != nullptr ? party->get_members_count() : 1);
			game_health heal = (heal_rate * player_value->get_stats()->get_max_hp() / 100) / party_players;

			if (party != nullptr) {
				game_experience exp_increase = 0;
				const auto members = party->get_party_members(player_value->get_map_id());
				for (const auto &party_member : members) {
					game_health chp = party_member->get_stats()->get_hp();
					if (chp > 0 && chp < party_member->get_stats()->get_max_hp()) {
						party_member->get_stats()->modify_hp(heal);
						if (player_value != party_member) {
							exp_increase += 20 * (party_member->get_stats()->get_hp() - chp) / (8 * party_member->get_stats()->get_level() + 190);
						}
					}
				}
				if (exp_increase > 0) {
					player_value->get_stats()->give_exp(exp_increase);
				}
			}
			else {
				player_value->get_stats()->modify_hp(heal);
			}
			break;
		}
		case vana::skills::fighter::rage:
		case vana::skills::dawn_warrior::rage:
		case vana::skills::spearman::iron_will:
		case vana::skills::spearman::hyper_body:
		case vana::skills::fp_wizard::meditation:
		case vana::skills::il_wizard::meditation:
		case vana::skills::blaze_wizard::meditation:
		case vana::skills::cleric::bless:
		case vana::skills::priest::holy_symbol:
		case vana::skills::bishop::resurrection:
		case vana::skills::bishop::holy_shield:
		case vana::skills::bowmaster::sharp_eyes:
		case vana::skills::marksman::sharp_eyes:
		case vana::skills::assassin::haste:
		case vana::skills::night_walker::haste:
		case vana::skills::hermit::meso_up:
		case vana::skills::bandit::haste:
		case vana::skills::buccaneer::speed_infusion:
		case vana::skills::thunder_breaker::speed_infusion:
		case vana::skills::buccaneer::time_leap:
		case vana::skills::hero::maple_warrior:
		case vana::skills::paladin::maple_warrior:
		case vana::skills::dark_knight::maple_warrior:
		case vana::skills::fp_arch_mage::maple_warrior:
		case vana::skills::il_arch_mage::maple_warrior:
		case vana::skills::bishop::maple_warrior:
		case vana::skills::bowmaster::maple_warrior:
		case vana::skills::marksman::maple_warrior:
		case vana::skills::night_lord::maple_warrior:
		case vana::skills::shadower::maple_warrior:
		case vana::skills::buccaneer::maple_warrior:
		case vana::skills::corsair::maple_warrior: {
			if (skill_id == vana::skills::buccaneer::time_leap) {
				player_value->get_skills()->remove_all_cooldowns();
			}
			if (party *party = player_value->get_party()) {
				int8_t affected = reader.get<int8_t>();
				const auto members = get_affected_party_members(party, affected, party->get_members_count());
				for (const auto &party_member : members) {
					if (party_member != nullptr && party_member != player_value && party_member->get_map() == player_value->get_map()) {
						party_member->send(packets::skills::show_skill(party_member->get_id(), skill_id, level, direction, true, true));
						party_member->send_map(packets::skills::show_skill(party_member->get_id(), skill_id, level, direction, true));
						buffs::add_buff(party_member, skill_id, level, 0);
						if (skill_id == vana::skills::buccaneer::time_leap) {
							party_member->get_skills()->remove_all_cooldowns();
						}
					}
				}
			}
			break;
		}

		case vana::skills::beginner::echo_of_hero:
		case vana::skills::noblesse::echo_of_hero:
		case vana::skills::super_gm::haste:
		case vana::skills::super_gm::holy_symbol:
		case vana::skills::super_gm::bless:
		case vana::skills::super_gm::hyper_body:
		case vana::skills::super_gm::heal_plus_dispel:
		case vana::skills::super_gm::resurrection: {
			uint8_t players = reader.get<uint8_t>();
			function<bool(ref_ptr<player>)> do_action;
			function<void(ref_ptr<player>)> action;
			switch (skill_id) {
				case vana::skills::super_gm::heal_plus_dispel:
					do_action = [](ref_ptr<player> target) { return !target->get_stats()->is_dead(); };
					action = [](ref_ptr<player> target) {
						target->get_stats()->set_hp(target->get_stats()->get_max_hp());
						target->get_stats()->set_mp(target->get_stats()->get_max_mp());
						target->get_active_buffs()->use_player_dispel();
					};
					break;
				case vana::skills::super_gm::resurrection:
					do_action = [](ref_ptr<player> target) { return target->get_stats()->is_dead(); };
					action = [](ref_ptr<player> target) {
						target->get_stats()->set_hp(target->get_stats()->get_max_hp());
					};
					break;
				default:
					do_action = [](ref_ptr<player> target) { return true; };
					action = [skill_id, level](ref_ptr<player> target) {
						buffs::add_buff(target, skill_id, level, 0);
					};
			}
			for (uint8_t i = 0; i < players; i++) {
				game_player_id player_id = reader.get<game_player_id>();
				auto target = channel_server::get_instance().get_player_data_provider().get_player(player_id);
				if (target != nullptr && target != player_value && do_action(target)) {
					player_value->send(packets::skills::show_skill(player_value->get_id(), skill_id, level, direction, true, true));
					player_value->send_map(packets::skills::show_skill(player_value->get_id(), skill_id, level, direction, true));

					action(target);
				}
			}
			break;
		}
		default:
			type = reader.get<int8_t>();
			switch (type) {
				case 0x80:
					added_info = reader.get<int16_t>();
					break;
			}
			break;
	}

	player_value->send_map(packets::skills::show_skill(player_value->get_id(), skill_id, level, direction));

	if (buffs::add_buff(player_value, skill_id, level, added_info) == result::successful) {
		if (skill_id == vana::skills::super_gm::hide) {
			player_value->send(packets::gm::begin_hide());
			player_value->get_map()->gm_hide_change(player_value);
		}

		return;
	}

	if (game_logic_utilities::is_summon(skill_id)) {
		point pos = reader.get<point>(); // Useful?
		summon_handler::use_summon(player_value, skill_id, level);
	}
}

auto skills::apply_skill_costs(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level, bool elemental_amp) -> result {
	if (player->has_gm_benefits()) {
		// Ensure we don't lock, but don't actually use anything
		player->get_stats()->set_hp(player->get_stats()->get_hp(), true);
		player->get_stats()->set_mp(player->get_stats()->get_mp(), true);
		return result::successful;
	}

	auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
	seconds cool_time = skill->cool_time;
	game_health mp_use = skill->mp;
	game_health hp_use = skill->hp;
	int16_t money_consume = skill->money_consume;
	game_item_id item = skill->item;
	if (mp_use > 0) {
		auto concentrate = player->get_active_buffs()->get_concentrate_source();
		if (concentrate.is_initialized()) {
			auto skill = player->get_active_buffs()->get_buff_skill_info(concentrate.get());
			mp_use = (mp_use * skill->x) / 100;
		}
		else if (elemental_amp && player->get_skills()->has_elemental_amp()) {
			mp_use = (mp_use * player->get_skills()->get_skill_info(player->get_skills()->get_elemental_amp())->x) / 100;
		}

		if (player->get_stats()->get_mp() < mp_use) {
			return result::failure;
		}
		player->get_stats()->modify_mp(-mp_use, true);
	}
	else {
		player->get_stats()->set_mp(player->get_stats()->get_mp(), true);
	}
	if (hp_use > 0) {
		if (player->get_stats()->get_hp() < hp_use) {
			return result::failure;
		}
		player->get_stats()->modify_hp(-hp_use);
	}
	if (item > 0) {
		if (player->get_inventory()->get_item_amount(item) < skill->item_count) {
			return result::failure;
		}
		inventory::take_item(player, item, skill->item_count);
	}
	if (cool_time.count() > 0 && skill_id != vana::skills::corsair::battleship) {
		if (is_cooling(player, skill_id)) {
			return result::failure;
		}
		start_cooldown(player, skill_id, cool_time);
	}

	if (money_consume > 0) {
		int16_t min_mesos = money_consume - (80 + level * 5);
		int16_t max_mesos = money_consume + (80 + level * 5);
		int16_t amount = randomizer::rand<int16_t>(max_mesos, min_mesos);
		game_mesos mesos = player->get_inventory()->get_mesos();
		if (mesos - amount < 0) {
			// Hacking
			return result::failure;
		}
		player->get_inventory()->modify_mesos(-amount);
	}

	return result::successful;
}

auto skills::use_attack_skill(ref_ptr<player> player, game_skill_id skill_id) -> result {
	if (skill_id != vana::skills::all::regular_attack) {
		game_skill_level level = player->get_skills()->get_skill_level(skill_id);
		if (!channel_server::get_instance().get_skill_data_provider().is_valid_skill(skill_id) || level == 0) {
			return result::failure;
		}
		return apply_skill_costs(player, skill_id, level, true);
	}
	return result::successful;
}

auto skills::use_attack_skill_ranged(ref_ptr<player> player, game_skill_id skill_id, game_inventory_slot projectile_pos, game_inventory_slot cash_projectile_pos, game_item_id projectile_id) -> result {
	game_skill_level level = 0;
	if (skill_id != vana::skills::all::regular_attack) {
		level = player->get_skills()->get_skill_level(skill_id);
		if (!channel_server::get_instance().get_skill_data_provider().is_valid_skill(skill_id) || level == 0) {
			return result::failure;
		}
		if (apply_skill_costs(player, skill_id, level) == result::failure) {
			return result::failure;
		}
	}

	if (player->has_gm_benefits()) {
		return result::successful;
	}

	switch (game_logic_utilities::get_job_track(player->get_stats()->get_job())) {
		case jobs::job_tracks::bowman:
		case jobs::job_tracks::wind_archer:
			if (player->get_active_buffs()->has_soul_arrow()) {
				return result::successful;
			}
			if (!game_logic_utilities::is_arrow(projectile_id)) {
				return result::failure;
			}
			break;
		case jobs::job_tracks::thief:
		case jobs::job_tracks::night_walker:
			if (player->get_active_buffs()->has_shadow_stars()) {
				return result::successful;
			}
			if (cash_projectile_pos > 0) {
				item *cash_item = player->get_inventory()->get_item(inventories::cash, cash_projectile_pos);
				if (cash_item == nullptr || cash_item->get_id() != projectile_id) {
					return result::failure;
				}

				item *projectile = player->get_inventory()->get_item(inventories::use, projectile_pos);
				if (projectile == nullptr) {
					return result::failure;
				}

				projectile_id = projectile->get_id();
			}
			if (!game_logic_utilities::is_star(projectile_id)) {
				return result::failure;
			}
			break;
		case jobs::job_tracks::pirate:
			if (!game_logic_utilities::is_bullet(projectile_id)) {
				return result::failure;
			}
			break;
	}

	if (projectile_pos <= 0) {
		return result::failure;
	}

	item *projectile = player->get_inventory()->get_item(inventories::use, projectile_pos);
	if (projectile == nullptr || projectile->get_id() != projectile_id) {
		return result::failure;
	}

	game_slot_qty hits = 1;
	if (skill_id != vana::skills::all::regular_attack) {
		auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
		game_item_id optional_item = skill->optional_item;

		if (optional_item != 0 && optional_item == projectile_id) {
			if (projectile->get_amount() < skill->item_count) {
				return result::failure;
			}
			inventory::take_item_slot(player, inventories::use, projectile_pos, skill->item_count);
			return result::successful;
		}

		game_slot_qty bullets = skill->bullet_consume;
		if (bullets > 0) {
			hits = bullets;
		}
	}

	if (player->get_active_buffs()->has_shadow_partner()) {
		hits *= 2;
	}

	if (projectile->get_amount() < hits) {
		return result::failure;
	}

	inventory::take_item_slot(player, inventories::use, projectile_pos, hits);
	return result::successful;
}

auto skills::heal(ref_ptr<player> player, int64_t value, const buff_source &source) -> void {
	if (player->get_stats()->get_hp() < player->get_stats()->get_max_hp() && player->get_stats()->get_hp() > 0) {
		game_health val = static_cast<game_health>(value);
		player->get_stats()->modify_hp(val);
		player->send(packets::skills::heal_hp(val));
	}
}

auto skills::hurt(ref_ptr<player> player, int64_t value, const buff_source &source) -> void {
	game_health val = static_cast<game_health>(value);
	if (source.get_type() != buff_source_type::skill) throw not_implemented_exception{"hurt buff_source_type"};
	if (player->get_stats()->get_hp() - val > 1) {
		player->get_stats()->modify_hp(-val);
		player->send_map(packets::skills::show_skill_effect(player->get_id(), source.get_skill_id()));
	}
	else {
		buffs::end_buff(player, source);
	}
}

auto skills::start_cooldown(ref_ptr<player> player, game_skill_id skill_id, seconds cool_time, bool initial_load) -> void {
	if (is_cooling(player, skill_id)) {
		// Hacking
		return;
	}
	if (!initial_load) {
		player->send(packets::skills::send_cooldown(skill_id, cool_time));
		player->get_skills()->add_cooldown(skill_id, cool_time);
	}
	vana::timer::timer::create(
		[player, skill_id](const time_point &now) {
			skills::stop_cooldown(player, skill_id);
		},
		vana::timer::id{timer_type::cool_timer, skill_id},
		player->get_timer_container(),
		seconds{cool_time});
}

auto skills::stop_cooldown(ref_ptr<player> player, game_skill_id skill_id) -> void {
	player->get_skills()->remove_cooldown(skill_id);
	player->send(packets::skills::send_cooldown(skill_id, seconds{0}));
	if (skill_id == vana::skills::corsair::battleship) {
		player->get_active_buffs()->reset_battleship_hp();
	}

	vana::timer::id id{timer_type::cool_timer, skill_id};
	auto container = player->get_timer_container();
	if (container->is_timer_running(id)) {
		container->remove_timer(id);
	}
}

auto skills::is_cooling(ref_ptr<player> player, game_skill_id skill_id) -> bool {
	vana::timer::id id{timer_type::cool_timer, skill_id};
	return player->get_timer_container()->is_timer_running(id);
}

auto skills::get_cooldown_time_left(ref_ptr<player> player, game_skill_id skill_id) -> int16_t {
	int16_t cool_time = 0;
	if (is_cooling(player, skill_id)) {
		vana::timer::id id{timer_type::cool_timer, skill_id};
		cool_time = static_cast<int16_t>(player->get_timer_container()->get_remaining_time<seconds>(id).count());
	}
	return cool_time;
}

}
}