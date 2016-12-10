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
#include "buffs.hpp"
#include "common/algorithm.hpp"
#include "common/data/provider/buff.hpp"
#include "common/data/provider/skill.hpp"
#include "common/util/game_logic/player_skill.hpp"
#include "channel_server/buffs_packet.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/map.hpp"
#include "channel_server/player.hpp"
#include "channel_server/summon_handler.hpp"

namespace vana {
namespace channel_server {

auto buffs::add_buff(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level, int16_t added_info, game_map_object map_mob_id) -> result {
	auto source = data::type::buff_source::from_skill(skill_id, level);
	auto &buff_provider = channel_server::get_instance().get_buff_data_provider();
	if (!buff_provider.is_buff(source)) {
		return result::failure;
	}

	auto skill = source.get_skill_data(channel_server::get_instance().get_skill_data_provider());
	seconds time = skill->buff_time;
	switch (skill_id) {
		case constant::skill::dragon_knight::dragon_roar:
			time = seconds{skill->y};
			break;
		case constant::skill::super_gm::hide:
			time = constant::buff::max_buff_time;
			break;
	}

	auto buffs = preprocess_buff(player, source, time);
	if (!buffs.any_buffs()) return result::failure;

	auto &basics = buff_provider.get_buffs_by_effect();
	for (const auto &info : buffs.get_buff_info()) {
		if (info == basics.homing_beacon) {
			if (map_mob_id == player->get_active_buffs()->get_homing_beacon_mob()) return result::failure;
			player->get_active_buffs()->reset_homing_beacon_mob(map_mob_id);
			break;
		}
	}

	return player->get_active_buffs()->add_buff(source, buffs, time);
}

auto buffs::add_buff(ref_ptr<player> player, game_item_id item_id, const seconds &time) -> result {
	auto source = data::type::buff_source::from_item(item_id);
	if (!channel_server::get_instance().get_buff_data_provider().is_buff(source)) {
		return result::failure;
	}

	auto buffs = preprocess_buff(player, source, time);
	if (!buffs.any_buffs()) return result::failure;

	return player->get_active_buffs()->add_buff(source, buffs, time);
}

auto buffs::add_buff(ref_ptr<player> player, game_mob_skill_id skill_id, game_mob_skill_level level, milliseconds delay) -> result {
	auto source = data::type::buff_source::from_mob_skill(skill_id, level);
	if (!channel_server::get_instance().get_buff_data_provider().is_debuff(source)) {
		return result::failure;
	}

	if (player->get_stats()->is_dead() || player->get_active_buffs()->has_holy_shield() || player->has_gm_benefits()) {
		return result::failure;
	}

	auto skill = source.get_mob_skill_data(channel_server::get_instance().get_skill_data_provider());
	seconds time{skill->time};

	auto buffs = preprocess_buff(player, source, time);
	if (!buffs.any_buffs()) return result::failure;

	return player->get_active_buffs()->add_buff(source, buffs.with_delay(delay), time);
}

auto buffs::end_buff(ref_ptr<player> player, const data::type::buff_source &source, bool from_timer) -> void {
	player->get_active_buffs()->remove_buff(source, preprocess_buff(player, source, seconds{0}), from_timer);
}

auto buffs::buff_may_apply(ref_ptr<player> player, const data::type::buff_source &source, const seconds &time, const data::type::buff_info &buff) -> bool {
	switch (source.get_type()) {
		case data::type::buff_source_type::skill: {
			game_skill_id skill_id = source.get_skill_id();
			game_skill_level skill_level = source.get_skill_level();
			if (vana::util::game_logic::player_skill::is_dark_sight(skill_id)) {
				return
					buff.get_value() != data::type::buff_skill_value::speed ||
					skill_level != channel_server::get_instance().get_skill_data_provider().get_max_level(skill_id);
			}
			return true;
		}
		case data::type::buff_source_type::mob_skill: {
			game_mob_skill_id skill_id = source.get_mob_skill_id();
			return player->get_active_buffs()->get_buff_source(buff).is_initialized() ?
				false :
				true;
		}
		case data::type::buff_source_type::item: {
			game_item_id item_id = source.get_item_id();
			return true;
		}
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
}

auto buffs::preprocess_buff(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level, const seconds &time) -> data::type::buff {
	auto source = data::type::buff_source::from_skill(skill_id, level);
	return preprocess_buff(player, source, time);
}

auto buffs::preprocess_buff(ref_ptr<player> player, game_item_id item_id, const seconds &time) -> data::type::buff {
	auto source = data::type::buff_source::from_item(item_id);
	return preprocess_buff(player, source, time);
}

auto buffs::preprocess_buff(ref_ptr<player> player, game_mob_skill_id skill_id, game_mob_skill_level level, const seconds &time) -> data::type::buff {
	auto source = data::type::buff_source::from_mob_skill(skill_id, level);
	return preprocess_buff(player, source, time);
}

auto buffs::preprocess_buff(ref_ptr<player> player, const data::type::buff_source &source, const seconds &time) -> data::type::buff {
	auto &buff_provider = channel_server::get_instance().get_buff_data_provider();

	if (source.get_type() == data::type::buff_source_type::item) {
		game_item_id item_id = source.get_item_id();
		switch (item_id) {
			case constant::item::beholder_hex_wdef:
			case constant::item::beholder_hex_mdef:
			case constant::item::beholder_hex_acc:
			case constant::item::beholder_hex_avo:
			case constant::item::beholder_hex_watk: {
				data::type::buff_info data = summon_handler::make_buff(player, item_id);
				data::type::buff parsed{data};
				return preprocess_buff(player, source, time, parsed);
			}
		}
	}
	auto &skills_info = buff_provider.get_info(source);
	return preprocess_buff(player, source, time, skills_info);
}

auto buffs::preprocess_buff(ref_ptr<player> player, const data::type::buff_source &source, const seconds &time, const data::type::buff &buff_value) -> data::type::buff {
	vector<data::type::buff_info> applicable_buffs;
	vector<data::type::buff_info> existing_buffs;
	if (buff_value.is_selection_buff()) {
		int16_t chance = vana::util::randomizer::percentage<int16_t>();
		for (const auto &buff_info : buff_value.get_buff_info()) {
			if (chance < buff_info.get_chance()) {
				existing_buffs.push_back(buff_info);
				break;
			}
			else chance -= buff_info.get_chance();
		}
	}
	else {
		existing_buffs = buff_value.get_buff_info();
	}

	for (auto &info : existing_buffs) {
		if (!buff_may_apply(player, source, time, info)) continue;
		applicable_buffs.push_back(info);
	}

	return buff_value.with_buffs(applicable_buffs);
}

auto buffs::preprocess_buff(const data::type::buff &buff_value, const vector<uint8_t> &bit_positions_to_include) -> data::type::buff {
	vector<data::type::buff_info> applicable;
	for (const auto &info : buff_value.get_buff_info()) {
		bool found = false;
		for (const auto &bit_position : bit_positions_to_include) {
			if (bit_position == info) {
				found = true;
				break;
			}
		}
		if (found) {
			applicable.push_back(info);
		}
	}
	return buff_value.with_buffs(applicable);
}

auto buffs::convert_to_packet_types(const data::type::buff &buff) -> buff_packet_values {
	buff_packet_values ret;
	for (const auto &buff_info : buff.get_buff_info()) {
		ret.player.types[buff_info.get_buff_byte()] |= buff_info.get_buff_type();
		if (buff_info.is_movement_affecting()) ret.player.any_movement_buffs = true;
		if (buff_info.has_map_info()) {
			if (!ret.map.is_initialized()) {
				ret.map = buff_packet_structure{};
			}
			ret.map.get().types[buff_info.get_buff_byte()] |= buff_info.get_buff_type();
		}
	}
	return ret;
}

auto buffs::convert_to_packet(ref_ptr<player> player, const data::type::buff_source &source, const seconds &time, const data::type::buff &buff) -> buff_packet_values {
	buff_packet_values ret;
	bool any_map = false;
	for (const auto &buff_info : buff.get_buff_info()) {
		ret.player.types[buff_info.get_buff_byte()] |= buff_info.get_buff_type();
		if (buff_info.is_movement_affecting()) ret.player.any_movement_buffs = true;
		if (buff_info.has_map_info()) {
			auto map_value = buff_info.get_map_info();
			if (!any_map) {
				ret.map = buff_packet_structure{};
				any_map = true;
			}

			auto &map = ret.map.get();
			map.types[buff_info.get_buff_byte()] |= buff_info.get_buff_type();
			map.values.push_back(get_value(player, source, time, buff_info.get_bit_position(), map_value));
		}

		ret.player.values.push_back(get_value(player, source, time, buff_info));
	}
	ret.delay = buff.get_delay();
	return ret;
}

auto buffs::get_value(ref_ptr<player> player, const data::type::buff_source &source, const seconds &time, const data::type::buff_info &buff) -> buff_packet_value {
	if (buff.get_value() == data::type::buff_skill_value::predefined) {
		return buff_packet_value::from_value(2, buff.get_predefined_value());
	}
	return get_value(player, source, time, buff.get_bit_position(), buff.get_value(), 2);
}

auto buffs::get_value(ref_ptr<player> player, const data::type::buff_source &source, const seconds &time, uint8_t bit_position, const data::type::buff_map_info &buff) -> buff_packet_value {
	if (buff.get_value() == data::type::buff_skill_value::predefined) {
		return buff_packet_value::from_value(buff.get_size(), buff.get_predefined_value());
	}
	return get_value(player, source, time, bit_position, buff.get_value(), buff.get_size());
}

auto buffs::get_value(ref_ptr<player> player, const data::type::buff_source &source, const seconds &time, uint8_t bit_position, data::type::buff_skill_value value, uint8_t buff_value_size) -> buff_packet_value {
	switch (source.get_type()) {
		case data::type::buff_source_type::item: {
			THROW_CODE_EXCEPTION(not_implemented_exception, "item data::type::buff_skill_value type");
		}
		case data::type::buff_source_type::mob_skill: {
			game_mob_skill_id skill_id = source.get_mob_skill_id();
			game_mob_skill_level skill_level = source.get_mob_skill_level();
			auto skill = source.get_mob_skill_data(channel_server::get_instance().get_skill_data_provider());
			switch (value) {
				case data::type::buff_skill_value::x: return buff_packet_value::from_value(buff_value_size, skill->x);
				case data::type::buff_skill_value::y: return buff_packet_value::from_value(buff_value_size, skill->y);
				case data::type::buff_skill_value::prop: return buff_packet_value::from_value(buff_value_size, skill->prop);
				case data::type::buff_skill_value::level: return buff_packet_value::from_value(buff_value_size, skill_level);
				case data::type::buff_skill_value::skill_id: return buff_packet_value::from_value(buff_value_size, skill_id);
				case data::type::buff_skill_value::bitpacked_skill_and_level_32: return buff_packet_value::from_value(buff_value_size, (static_cast<int32_t>(skill_level) << 16) | skill_id);
				case data::type::buff_skill_value::special_processing: {
					switch (skill_id) {
						// This unusual skill has two "values"
						case constant::mob_skill::poison: {
							packet_builder builder;
							builder.add<int16_t>(static_cast<int16_t>(skill->x));
							builder.add<int16_t>(skill_id);
							builder.add<int16_t>(skill_level);
							return buff_packet_value::from_packet(builder);
						}
					}
					THROW_CODE_EXCEPTION(not_implemented_exception, "special_processing mob skill");
				}
			}
			THROW_CODE_EXCEPTION(not_implemented_exception, "mob data::type::buff_skill_value type");
		}
		case data::type::buff_source_type::skill: {
			game_skill_id skill_id = source.get_skill_id();
			game_skill_level skill_level = source.get_skill_level();
			auto skill = source.get_skill_data(channel_server::get_instance().get_skill_data_provider());

			switch (value) {
				case data::type::buff_skill_value::x: return buff_packet_value::from_value(buff_value_size, skill->x);
				case data::type::buff_skill_value::y: return buff_packet_value::from_value(buff_value_size, skill->y);
				case data::type::buff_skill_value::speed: return buff_packet_value::from_value(buff_value_size, skill->speed);
				case data::type::buff_skill_value::jump: return buff_packet_value::from_value(buff_value_size, skill->jump);
				case data::type::buff_skill_value::watk: return buff_packet_value::from_value(buff_value_size, skill->w_atk);
				case data::type::buff_skill_value::wdef: return buff_packet_value::from_value(buff_value_size, skill->w_def);
				case data::type::buff_skill_value::matk: return buff_packet_value::from_value(buff_value_size, skill->m_atk);
				case data::type::buff_skill_value::mdef: return buff_packet_value::from_value(buff_value_size, skill->m_def);
				case data::type::buff_skill_value::accuracy: return buff_packet_value::from_value(buff_value_size, skill->acc);
				case data::type::buff_skill_value::avoid: return buff_packet_value::from_value(buff_value_size, skill->avo);
				case data::type::buff_skill_value::prop: return buff_packet_value::from_value(buff_value_size, skill->prop);
				case data::type::buff_skill_value::morph: return buff_packet_value::from_value(buff_value_size, skill->morph);
				case data::type::buff_skill_value::level: return buff_packet_value::from_value(buff_value_size, skill_level);
				case data::type::buff_skill_value::skill_id: return buff_packet_value::from_value(buff_value_size, skill_id);
				case data::type::buff_skill_value::mob_count: return buff_packet_value::from_value(buff_value_size, skill->mob_count);
				case data::type::buff_skill_value::range: return buff_packet_value::from_value(buff_value_size, skill->range);
				case data::type::buff_skill_value::damage: return buff_packet_value::from_value(buff_value_size, skill->damage);
				case data::type::buff_skill_value::bitpacked_xy_16: return buff_packet_value::from_value(buff_value_size, (skill->x << 8) | skill->y);

				case data::type::buff_skill_value::special_processing:
					switch (skill_id) {
						case constant::skill::crusader::combo_attack:
						case constant::skill::dawn_warrior::combo_attack:
							return buff_packet_value::from_value(
								buff_value_size,
								player->get_active_buffs()->get_combo() + 1
							);
						case constant::skill::night_lord::shadow_stars:
							return buff_packet_value::from_value(
								buff_value_size,
								(player->get_inventory()->do_shadow_stars() % 10000) + 1
							);
						case constant::skill::super_gm::hide:
							// TODO FIXME BUFFS
							return buff_packet_value::from_value(buff_value_size, 1);
					}

					THROW_CODE_EXCEPTION(not_implemented_exception, "special_processing skill");

				case data::type::buff_skill_value::special_packet: {
					auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();

					packet_builder builder;
					if (bit_position == basics.energy_charge) {
						builder.add<int32_t>(player->get_active_buffs()->get_energy_charge_level());
					}
					else if (bit_position == basics.dash_speed) {
						builder.add<int32_t>(skill->x);
					}
					else if (bit_position == basics.dash_jump) {
						builder.add<int32_t>(skill->y);
					}
					else if (bit_position == basics.mount) {
						builder.add<game_item_id>(player->get_active_buffs()->get_mount_item_id());
					}
					else if (bit_position == basics.speed_infusion) {
						builder.add<int32_t>(skill->x);
					}
					else if (bit_position == basics.homing_beacon) {
						builder.unk<int32_t>(1);
					}
					else THROW_CODE_EXCEPTION(not_implemented_exception, "special_packet skill");

					builder.add<int32_t>(
						bit_position == basics.energy_charge ?
							0 :
							skill_id);
					builder.unk<int32_t>();
					builder.unk<int8_t>();

					if (bit_position == basics.energy_charge) {
						builder.add<int16_t>(static_cast<int16_t>(time.count()));
					}
					else if (bit_position == basics.dash_speed) {
						builder.add<int16_t>(static_cast<int16_t>(time.count()));
					}
					else if (bit_position == basics.dash_jump) {
						builder.add<int16_t>(static_cast<int16_t>(time.count()));
					}
					else if (bit_position == basics.mount) {
						// Intentionally blank
					}
					else if (bit_position == basics.speed_infusion) {
						builder.unk<int32_t>();
						builder.unk<int8_t>();
						builder.add<int16_t>(static_cast<int16_t>(time.count()));
					}
					else if (bit_position == basics.homing_beacon) {
						builder.add<game_map_object>(player->get_active_buffs()->get_homing_beacon_mob());
					}

					return buff_packet_value::from_special_packet(builder);
				}

				case data::type::buff_skill_value::gender_specific_morph:
					return buff_packet_value::from_value(
						buff_value_size,
						skill->morph +
						(player->get_gender() == constant::gender::male ? 0 : 100)
					);
			}

			THROW_CODE_EXCEPTION(not_implemented_exception, "skill data::type::buff_skill_value type");
		}
	}

	THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
}

}
}