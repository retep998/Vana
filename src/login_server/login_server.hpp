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

#include "common/abstract_server.hpp"
#include "common/curse_data_provider.hpp"
#include "common/equip_data_provider.hpp"
#include "common/finalization_pool.hpp"
#include "common/salt_config.hpp"
#include "common/salt_size_config.hpp"
#include "common/types.hpp"
#include "common/valid_char_data_provider.hpp"
#include "login_server/login_server_accepted_session.hpp"
#include "login_server/worlds.hpp"

namespace vana {
	namespace login_server {
		class login_server final : public abstract_server {
			SINGLETON(login_server);
		public:
			auto get_pin_enabled() const -> bool;
			auto rehash_config() -> void;
			auto get_invalid_login_threshold() const -> int32_t;
			auto get_valid_char_data_provider() const -> const valid_char_data_provider &;
			auto get_equip_data_provider() const -> const equip_data_provider &;
			auto get_curse_data_provider() const -> const curse_data_provider &;
			auto get_worlds() -> worlds &;
			auto get_character_account_salt_size() const -> const salt_size_config &;
			auto get_character_account_salting_policy() const -> const salt_config &;
			auto finalize_user(ref_ptr<user> user_value) -> void;
			auto finalize_server_session(ref_ptr<login_server_accepted_session> session) -> void;
		protected:
			auto init_complete() -> void override;
			auto load_data() -> result override;
			auto load_config() -> result override;
			auto listen() -> void;
			auto load_worlds() -> void;
			auto make_log_identifier() const -> opt_string override;
			auto get_log_prefix() const -> string override;
		private:
			bool m_pin_enabled = false;
			connection_port m_port = 0;
			int32_t m_max_invalid_logins = 0;
			salt_size_config m_account_salt_size;
			salt_config m_account_salting_policy;
			valid_char_data_provider m_valid_char_data_provider;
			equip_data_provider m_equip_data_provider;
			curse_data_provider m_curse_data_provider;
			worlds m_worlds;
			finalization_pool<user> m_user_pool;
			finalization_pool<login_server_accepted_session> m_session_pool;
		};
	}
}