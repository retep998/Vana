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
#include "lua_npc.hpp"
#include "common/script_data_provider.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/npc.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/quests.hpp"
#include "channel_server/storage_packet.hpp"
#include <vector>

namespace vana {
namespace channel_server {

lua_npc::lua_npc(const string &filename, game_player_id player_id) :
	lua_scriptable{filename, player_id, true}
{
	set_npc_environment_variables();

	// Miscellaneous
	expose("getDistanceToPlayer", &lua_exports::get_distance_npc);
	expose("getNpcId", &lua_exports::get_npc_id);
	expose("runNpc", &lua_exports::run_npc_npc);
	expose("showStorage", &lua_exports::show_storage);

	// NPC interaction
	expose("addText", &lua_exports::add_text);
	expose("askAcceptDecline", &lua_exports::ask_accept_decline);
	expose("askAcceptDeclineNoExit", &lua_exports::ask_accept_decline_no_exit);
	expose("askChoice", &lua_exports::ask_choice);
	expose("askNumber", &lua_exports::ask_number);
	expose("askQuestion", &lua_exports::ask_question);
	expose("askQuiz", &lua_exports::ask_quiz);
	expose("askStyle", &lua_exports::ask_style);
	expose("askText", &lua_exports::ask_text);
	expose("askYesNo", &lua_exports::ask_yes_no);
	expose("sendBackNext", &lua_exports::send_back_next);
	expose("sendBackOk", &lua_exports::send_back_ok);
	expose("sendNext", &lua_exports::send_next);
	expose("sendOk", &lua_exports::send_ok);

	// Quest
	expose("addQuest", &lua_exports::add_quest);
	expose("endQuest", &lua_exports::end_quest);
}

auto lua_npc::set_npc_environment_variables() -> void {
	set<int32_t>("answer_yes", 1);
	set<int32_t>("answer_no", 0);

	set<int32_t>("answer_accept", 1);
	set<int32_t>("answer_decline", 0);

	set<int32_t>("quiz_npc", 0);
	set<int32_t>("quiz_mob", 1);
	set<int32_t>("quiz_item", 2);
}

auto lua_npc::handle_thread_completion() -> void {
	channel_server::get_instance().get_player_data_provider().get_player(m_player_id)->get_npc()->end();
}

auto lua_npc::proceed_next() -> result {
	return resume(0);
}

auto lua_npc::proceed_selection(uint8_t selected) -> result {
	push_thread<uint8_t>(selected);
	return resume(1);
}

auto lua_npc::proceed_number(int32_t number) -> result {
	push_thread<int32_t>(number);
	return resume(1);
}

auto lua_npc::proceed_text(const string &text) -> result {
	push_thread<string>(text);
	return resume(1);
}

auto lua_npc::handle_error(const string &filename, const string &error) -> void {
	print_error(error);
	channel_server::get_instance().get_player_data_provider().get_player(m_player_id)->get_npc()->end();
}

auto lua_exports::get_npc(lua_State *lua_vm, lua_environment &env) -> npc * {
	return get_player(lua_vm, env)->get_npc();
}

// Miscellaneous
auto lua_exports::get_distance_npc(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, get_player(lua_vm, env)->get_pos() - get_npc(lua_vm, env)->get_pos());
	return 1;
}

auto lua_exports::get_npc_id(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_npc_id>(lua_vm, get_npc(lua_vm, env)->get_npc_id());
	return 1;
}

auto lua_exports::run_npc_npc(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_npc_id npc_id = env.get<game_npc_id>(lua_vm, 1);
	string script;
	auto &channel = channel_server::get_instance();
	if (env.is(lua_vm, 2, lua::lua_type::string)) {
		// We already have our script name
		string specified = env.get<string>(lua_vm, 2);
		script = channel.get_script_data_provider().build_script_path(script_types::npc, specified);
	}
	else {
		script = channel.get_script_data_provider().get_script(&channel, npc_id, script_types::npc);
	}
	get_npc(lua_vm, env)->set_end_script(npc_id, script);
	return 0;
}

auto lua_exports::show_storage(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player(lua_vm, env);
	player->send(packets::storage::show_storage(player, get_npc(lua_vm, env)->get_npc_id()));
	return 0;
}

// NPC interaction
auto lua_exports::add_text(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->add_text(env.get<string>(lua_vm, 1));
	return 0;
}

auto lua_exports::ask_accept_decline(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_accept_decline();
	return env.yield(1);
}

auto lua_exports::ask_accept_decline_no_exit(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_accept_decline_no_exit();
	return env.yield(1);
}

auto lua_exports::ask_choice(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_simple();
	return env.yield(1);
}

auto lua_exports::ask_number(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	int32_t def = env.get<int32_t>(lua_vm, 1);
	int32_t min = env.get<int32_t>(lua_vm, 2);
	int32_t max = env.get<int32_t>(lua_vm, 3);
	get_npc(lua_vm, env)->send_get_number(def, min, max);
	return env.yield(1);
}

auto lua_exports::ask_question(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string question = env.get<string>(lua_vm, 1);
	string clue = env.get<string>(lua_vm, 2);
	int32_t min_length = env.get<int32_t>(lua_vm, 3);
	int32_t max_length = env.get<int32_t>(lua_vm, 4);
	int32_t time = env.get<int32_t>(lua_vm, 5);

	get_npc(lua_vm, env)->send_question(question, clue, min_length, max_length, time);
	return env.yield(1);
}

auto lua_exports::ask_quiz(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	int8_t type = env.get<int32_t>(lua_vm, 1);
	int32_t object_id = env.get<int32_t>(lua_vm, 2);
	int32_t correct = env.get<int32_t>(lua_vm, 3);
	int32_t questions = env.get<int32_t>(lua_vm, 4);
	int32_t time = env.get<int32_t>(lua_vm, 5);

	get_npc(lua_vm, env)->send_quiz(type, object_id, correct, questions, time);
	return env.yield(1);
}

auto lua_exports::ask_style(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	vector<int32_t> styles = env.get<vector<int32_t>>(lua_vm, 1);

	if (styles.size() > 0) {
		get_npc(lua_vm, env)->send_style(styles);
	}

	return env.yield(1);
}

auto lua_exports::ask_text(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	int16_t min = 0;
	int16_t max = 0;
	string def = "";
	if (env.is(lua_vm, 1, lua::lua_type::number) && env.is(lua_vm, 2, lua::lua_type::number)) {
		min = env.get<int32_t>(lua_vm, 1);
		max = env.get<int32_t>(lua_vm, 2);
		if (env.is(lua_vm, 3, lua::lua_type::string)) {
			def = env.get<string>(lua_vm, 3);
		}
	}
	else if (env.is(lua_vm, 1, lua::lua_type::string)) {
		def = env.get<string>(lua_vm, 1);
	}
	get_npc(lua_vm, env)->send_get_text(min, max, def);
	return env.yield(1);
}

auto lua_exports::ask_yes_no(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_yes_no();
	return env.yield(1);
}

auto lua_exports::send_back_next(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_dialog(true, true);
	return env.yield(0);
}

auto lua_exports::send_back_ok(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_dialog(true, false);
	return env.yield(0);
}

auto lua_exports::send_next(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_dialog(false, true);
	return env.yield(0);
}

auto lua_exports::send_ok(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_npc(lua_vm, env)->send_dialog(false, false);
	return env.yield(0);
}

// Quest
auto lua_exports::add_quest(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_quest_id quest_id = env.get<game_quest_id>(lua_vm, 1);
	get_player(lua_vm, env)->get_quests()->add_quest(quest_id, get_npc(lua_vm, env)->get_npc_id());
	return 0;
}

auto lua_exports::end_quest(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_quest_id quest_id = env.get<game_quest_id>(lua_vm, 1);
	get_player(lua_vm, env)->get_quests()->finish_quest(quest_id, get_npc(lua_vm, env)->get_npc_id());
	return 0;
}

}
}