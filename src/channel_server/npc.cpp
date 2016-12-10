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
#include "npc.hpp"
#include "common/data/provider/script.hpp"
#include "common/session.hpp"
#include "common/util/file.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/lua/lua_npc.hpp"
#include "channel_server/npc_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"
#include <string>

namespace vana {
namespace channel_server {

npc::npc(game_npc_id npc_id, ref_ptr<player> player, game_quest_id quest_id, bool is_start) :
	m_player{player},
	m_npc_id{npc_id}
{
	init_script(get_script(quest_id, is_start));
}

npc::npc(game_npc_id npc_id, ref_ptr<player> player, const point &pos, game_quest_id quest_id, bool is_start) :
	m_pos{pos},
	m_player{player},
	m_npc_id{npc_id}
{
	init_script(get_script(quest_id, is_start));
}

npc::npc(game_npc_id npc_id, ref_ptr<player> player, const string &script) :
	m_player{player},
	m_npc_id{npc_id}
{
	init_script(script);
}

auto npc::has_script(int32_t npc_id, game_quest_id quest_id, bool start) -> bool {
	string script = "";
	auto &channel = channel_server::get_instance();
	if (quest_id == 0) {
		script = channel.get_script_data_provider().get_script(&channel, npc_id, data::type::script_type::npc);
	}
	else {
		script = channel.get_script_data_provider().get_quest_script(&channel, quest_id, start ? 0 : 1);
	}
	return vana::util::file::exists(script);
}

auto npc::get_script(game_quest_id quest_id, bool start) -> string {
	auto &channel = channel_server::get_instance();
	if (quest_id == 0) {
		return channel.get_script_data_provider().get_script(&channel, m_npc_id, data::type::script_type::npc);
	}
	return channel.get_script_data_provider().get_quest_script(&channel, quest_id, start ? 0 : 1);
}

auto npc::init_script(const string &filename) -> void {
	if (vana::util::file::exists(filename)) {
		m_lua_npc = make_owned_ptr<lua::lua_npc>(filename, m_player->get_id());
		m_player->set_npc(this);
	}
	else {
		end();
	}
}

auto npc::set_end_script(game_npc_id npc_id, const string &fullscript) -> void {
	m_next_npc = npc_id;
	m_script = fullscript;
}

auto npc::check_end() -> bool {
	if (is_end()) {
		if (m_next_npc != 0) {
			npc *next_npc = new npc{m_next_npc, m_player, m_script};
			next_npc->run();
		}
		m_player->set_npc(nullptr);
		return true;
	}

	return false;
}

auto npc::run() -> void {
	if (check_end()) {
		return;
	}
	m_lua_npc->run();
	check_end();
}

auto npc::send_simple() -> void {
	m_sent_dialog = packets::npc::dialogs::simple;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text));
	m_text = "";
}

auto npc::send_yes_no() -> void {
	m_sent_dialog = packets::npc::dialogs::yes_no;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text));
	m_text = "";
}

auto npc::send_dialog(bool back, bool next, bool save) -> void {
	if (save) {
		// Store the current NPC state, for future "back" button use
		m_previous_states.push_back(make_ref_ptr<npc_chat_state>(m_text, back, next));
	}

	m_sent_dialog = packets::npc::dialogs::normal;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text)
		.add<bool>(back)
		.add<bool>(next));
	m_text = "";
}

auto npc::send_dialog(ref_ptr<npc_chat_state> npc_state) -> void {
	m_text = npc_state->text;
	send_dialog(npc_state->back, npc_state->next, false);
}

auto npc::send_accept_decline() -> void {
	m_sent_dialog = packets::npc::dialogs::accept_decline;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text));
	m_text = "";
}

auto npc::send_accept_decline_no_exit() -> void {
	m_sent_dialog = packets::npc::dialogs::accept_decline_no_exit;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text));
	m_text = "";
}

auto npc::send_quiz(int8_t type, int32_t object_id, int32_t correct, int32_t questions, int32_t time) -> void {
	m_sent_dialog = packets::npc::dialogs::quiz;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, "", false)
		.add<int8_t>(0)
		.add<int32_t>(type)
		.add<int32_t>(object_id)
		.add<int32_t>(correct)
		.add<int32_t>(questions)
		.add<int32_t>(time));
}

auto npc::send_question(const string &question, const string &clue, int32_t min_length, int32_t max_length, int32_t time) -> void {
	m_sent_dialog = packets::npc::dialogs::question;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, "", false)
		.add<int8_t>(0x00) // If it's 0x01, it does something else
		.add<string>(m_text)
		.add<string>(question) // Another question thing
		.add<string>(clue)
		.add<int32_t>(min_length)
		.add<int32_t>(max_length)
		.add<int32_t>(time));
}

auto npc::send_get_text(int16_t min, int16_t max, const string &def) -> void {
	m_sent_dialog = packets::npc::dialogs::get_text;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text)
		.add<string>(def)
		.add<int16_t>(min)
		.add<int16_t>(max));
	m_text = "";
}

auto npc::send_get_number(int32_t def, int32_t min, int32_t max) -> void {
	m_sent_dialog = packets::npc::dialogs::get_number;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text)
		.add<int32_t>(def)
		.add<int32_t>(min)
		.add<int32_t>(max));
	m_text = "";
}

auto npc::send_style(vector<int32_t> styles) -> void {
	m_sent_dialog = packets::npc::dialogs::style;
	m_player->send(packets::npc::npc_chat(m_sent_dialog, m_npc_id, m_text)
		.add<uint8_t>(static_cast<uint8_t>(styles.size()))
		.add<vector<int32_t>>(styles, styles.size()));
	m_text = "";
}

auto npc::proceed_back() -> void {
	if (m_state == 0) {
		// Hacking
		return;
	}

	m_state--;
	send_dialog(m_previous_states[m_state]);
}

auto npc::proceed_next() -> void {
	m_state++;
	if (m_state < m_previous_states.size()) {
		// Usage of "next" button after the "back" button
		send_dialog(m_previous_states[m_state]);
	}
	else {
		m_lua_npc->proceed_next();
	}
}

auto npc::proceed_selection(uint8_t selected) -> void {
	m_lua_npc->proceed_selection(selected);
}

auto npc::proceed_number(int32_t number) -> void {
	m_lua_npc->proceed_number(number);
}

auto npc::proceed_text(const string &text) -> void {
	m_lua_npc->proceed_text(text);
}

}
}