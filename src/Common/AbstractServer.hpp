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

#include "Common/ConnectionManager.hpp"
#include "Common/ExternalIp.hpp"
#include "Common/InterServerConfig.hpp"
#include "Common/Ip.hpp"
#include "Common/LogConfig.hpp"
#include "Common/Logger.hpp"
#include "Common/SaltConfig.hpp"
#include "Common/Types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace vana {
	class session;
	struct log_config;

	class abstract_server {
	public:
		virtual ~abstract_server() = default;

		auto initialize() -> result;
		virtual auto shutdown() -> void;

		auto log(log_type type, const string &message) -> void;
		auto log(log_type type, function<void(out_stream &)> produce_message) -> void;
		auto log(log_type type, const char *message) -> void;
		auto get_server_type() const -> server_type;
		auto get_inter_password() const -> string;
		auto get_interserver_salting_policy() const -> const salt_config &;
	protected:
		abstract_server(server_type type);
		virtual auto load_config() -> result;
		virtual auto init_complete() -> void;
		virtual auto load_data() -> result = 0;
		virtual auto make_log_identifier() const -> opt_string = 0;
		virtual auto get_log_prefix() const -> string = 0;

		auto get_inter_server_config() const -> const inter_server_config &;
		auto send_auth(ref_ptr<session> session) const -> void;
		auto display_launch_time() const -> void;
		auto build_log_identifier(function<void(out_stream &)> produce_id) const -> opt_string;
		auto get_connection_manager() -> connection_manager & { return m_connection_manager; }
	private:
		auto load_log_config() -> void;
		auto create_logger(const log_config &conf) -> void;

		server_type m_server_type = server_type::none;
		time_point m_start_time;
		string m_inter_password;
		string m_salt;
		owned_ptr<base_logger> m_logger;
		inter_server_config m_inter_server_config;
		salt_config m_salting_policy;
		ip_matrix m_external_ips;
		connection_manager m_connection_manager;
	};
}