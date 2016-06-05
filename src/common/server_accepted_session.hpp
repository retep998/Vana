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

#include "common/connection_type.hpp"
#include "common/external_ip.hpp"
#include "common/external_ip_resolver.hpp"
#include "common/packet_handler.hpp"
#include "common/server_type.hpp"
#include "common/session.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	class abstract_server;
	class packet_reader;
	class server_accepted_session;

	class server_accepted_session : public packet_handler {
	public:
		server_accepted_session(abstract_server &server);

		auto get_type() const -> server_type;
		auto is_authenticated() const -> bool;
		auto get_external_ips() const -> const ip_matrix &;
		auto match_subnet(const ip &test) const -> ip;
		auto set_external_ip_information(const ip &default_ip, const ip_matrix &matrix) -> void;
	protected:
		virtual auto handle(packet_reader &reader) -> result override;
		virtual auto authenticated(server_type type) -> void;
		virtual auto on_disconnect() -> void override;
		virtual auto on_connect() -> void override;
	private:
		bool m_is_authenticated = false;
		server_type m_type = server_type::none;
		abstract_server &m_server;
		external_ip_resolver m_resolver;
	};
}