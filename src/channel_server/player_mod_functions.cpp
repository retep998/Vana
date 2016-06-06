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
#include "player_mod_functions.hpp"
#include "common/skill_constants.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/player_data_provider.hpp"

namespace vana {
namespace channel_server {

auto player_mod_functions::disconnect(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->disconnect();
	return chat_result::handled_display;
}

auto player_mod_functions::save(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->save_all();
	chat_handler_functions::show_info(player, "Your progress has been saved");
	return chat_result::handled_display;
}

auto player_mod_functions::mod_mesos(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_inventory()->set_mesos(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::heal(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->get_active_buffs()->use_player_dispel();
	player->get_active_buffs()->remove_debuff(mob_skills::seduce);
	player->get_active_buffs()->remove_debuff(mob_skills::crazy_skull);
	player->get_stats()->set_hp(player->get_stats()->get_max_hp());
	player->get_stats()->set_mp(player->get_stats()->get_max_mp());
	return chat_result::handled_display;
}

auto player_mod_functions::mod_str(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_str(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::mod_dex(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_dex(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::mod_int(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_int(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::mod_luk(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_luk(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::max_stats(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->get_stats()->set_fame(stats::max_fame);
	player->get_stats()->set_max_hp(stats::max_max_hp);
	player->get_stats()->set_max_mp(stats::max_max_mp);
	auto max = std::numeric_limits<game_stat>::max();
	player->get_stats()->set_str(max);
	player->get_stats()->set_dex(max);
	player->get_stats()->set_int(max);
	player->get_stats()->set_luk(max);
	return chat_result::handled_display;
}

auto player_mod_functions::hp(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		game_health amount = atoi(args.c_str());
		player->get_stats()->set_max_hp(amount);
		if (player->get_stats()->get_hp() > amount) {
			player->get_stats()->set_hp(player->get_stats()->get_max_hp());
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::mp(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		game_health amount = atoi(args.c_str());
		player->get_stats()->set_max_mp(amount);
		if (player->get_stats()->get_mp() > amount) {
			player->get_stats()->set_mp(player->get_stats()->get_max_mp());
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::sp(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_sp(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::ap(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_ap(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::fame(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_fame(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::level(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->get_stats()->set_level(atoi(args.c_str()));
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::job(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		game_job_id job = chat_handler_functions::get_job(args);
		if (job >= 0) {
			player->get_stats()->set_job(job);
		}
		else {
			chat_handler_functions::show_error(player, "Invalid job: " + args);
		}
	}
	else {
		chat_handler_functions::show_info(player, [&](out_stream &message) {
			message << "Current job: " << player->get_stats()->get_job();
		});
	}
	return chat_result::handled_display;
}

auto player_mod_functions::add_sp(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+) ?(-{0,1}\d+)?)", matches) == match_result::any_matches) {
		string raw_skill = matches[1];
		game_skill_id skill_id = atoi(raw_skill.c_str());
		if (channel_server::get_instance().get_skill_data_provider().is_valid_skill(skill_id)) {
			// Don't allow skills that do not exist to be added
			string count_string = matches[2];
			game_skill_level count = count_string.empty() ? 1 : atoi(count_string.c_str());

			player->get_skills()->add_skill_level(skill_id, count);
		}
		else {
			chat_handler_functions::show_error(player, "Invalid skill: " + raw_skill);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto player_mod_functions::max_sp(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+) ?(-{0,1}\d+)?)", matches) == match_result::any_matches) {
		string raw_skill = matches[1];
		game_skill_id skill_id = atoi(raw_skill.c_str());
		if (channel_server::get_instance().get_skill_data_provider().is_valid_skill(skill_id)) {
			// Don't allow skills that do not exist to be added
			string max = matches[2];
			game_skill_level max_level = max.empty() ? 1 : atoi(max.c_str());

			player->get_skills()->set_max_skill_level(skill_id, max_level);
		}
		else {
			chat_handler_functions::show_error(player, "Invalid skill: " + raw_skill);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

}
}