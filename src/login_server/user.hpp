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
#pragma once

#include "common/file_time.hpp"
#include "common/packet_handler.hpp"
#include "common/types.hpp"
#include "login_server/player_status.hpp"

namespace vana {
	class packet_reader;

	namespace login_server {
		class user final : public packet_handler, public enable_shared<user> {
			NONCOPYABLE(user);
		public:
			user() = default;

			auto set_gender(game_gender_id gender) -> void { m_gender = gender; }
			auto set_world_id(game_world_id world_id) -> void { m_world_id = world_id; }
			auto set_admin(bool value) -> void { m_admin = value; }
			auto set_channel(game_channel_id chan_id) -> void {	m_channel = chan_id; }
			auto set_account_id(game_account_id id) -> void { m_account_id = id; }
			auto set_status(player_status::player_status status) -> void { m_status = status; }
			auto set_pin(int32_t pin) -> void { m_pin = pin; }
			auto set_char_delete_password(opt_int32_t char_delete_password) -> void { m_char_delete_password = char_delete_password; }
			auto set_quiet_ban_reason(int8_t reason) -> void { m_quiet_ban_reason = reason; }
			auto set_quiet_ban_time(file_time ban_time) -> void { m_quiet_ban_time = ban_time; }
			auto set_creation_time(file_time creation_time) -> void { m_user_creation = creation_time; }
			auto set_gm_level(int32_t gm_level) -> void { m_gm_level = gm_level; }

			auto get_gender() const -> optional<game_gender_id> { return m_gender; }
			auto get_world_id() const -> optional<game_world_id> { return m_world_id; }
			auto is_admin() const -> bool { return m_admin; }
			auto get_channel() const -> game_channel_id { return m_channel; }
			auto get_account_id() const -> game_account_id { return m_account_id; }
			auto get_gm_level() const -> int32_t { return m_gm_level; }
			auto get_status() const -> player_status::player_status { return m_status; }
			auto get_pin() const -> opt_int32_t { return m_pin; }
			auto get_char_delete_password() const -> opt_int32_t { return m_char_delete_password; }
			auto get_quiet_ban_reason() const -> int8_t { return m_quiet_ban_reason; }
			auto get_quiet_ban_time() const -> file_time { return m_quiet_ban_time; }
			auto get_creation_time() const -> file_time { return m_user_creation; }

			auto add_invalid_login() -> int32_t { return ++m_invalid_logins; }
			auto set_online(bool online) -> void;
		protected:
			auto handle(packet_reader &reader) -> result override;
			auto on_disconnect() -> void override;
		private:
			bool m_admin = false;
			bool m_checked_pin = false;
			int8_t m_quiet_ban_reason = 0;
			game_channel_id m_channel = 0;
			game_account_id m_account_id = 0;
			int32_t m_invalid_logins = 0;
			int32_t m_gm_level = 0;
			optional<game_gender_id> m_gender;
			optional<game_world_id> m_world_id;
			opt_int32_t m_pin;
			opt_int32_t m_char_delete_password;
			file_time m_quiet_ban_time = 0;
			file_time m_user_creation = 0;
			player_status::player_status m_status = player_status::not_logged_in;
		};
	}
}