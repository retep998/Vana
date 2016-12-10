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
#include "server_accepted_session.hpp"
#include "common/abstract_server.hpp"
#include "common/authentication_packet.hpp"
#include "common/inter_header.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include <asio.hpp>
#include <iostream>

namespace vana {

server_accepted_session::server_accepted_session(abstract_server &server) :
	m_server{server}
{
}

auto server_accepted_session::handle(packet_reader &reader) -> result {
	if (reader.get<packet_header>() == IMSG_PASSWORD) {
		if (reader.get<string>() == m_server.get_inter_password()) {
			m_is_authenticated = true;

			set_external_ip_information(m_session->get_ip(), reader.get<vector<external_ip>>());

			server_type type = static_cast<server_type>(reader.get<server_type_underlying>());
			m_type = type;
			authenticated(type);
		}
		else {
			m_server.log(vana::log::type::server_auth_failure, [&](out_stream &log) {
				log << "IP: " << m_session->get_ip();
			});
			m_session->disconnect();
			return result::failure;
		}
	}
	else if (!m_is_authenticated) {
		m_session->disconnect();
		return result::failure;
	}
	reader.reset();
	return result::success;
}

auto server_accepted_session::authenticated(server_type type) -> void {
	// Intentionally blank
}

auto server_accepted_session::get_type() const -> server_type {
	return m_type;
}

auto server_accepted_session::is_authenticated() const -> bool {
	return m_is_authenticated;
}

auto server_accepted_session::get_external_ips() const -> const ip_matrix & {
	return m_resolver.get_external_ips();
}

auto server_accepted_session::match_subnet(const ip &test) const -> ip {
	return m_resolver.match_ip_to_subnet(test);
}

auto server_accepted_session::set_external_ip_information(const ip &default_ip, const ip_matrix &matrix) -> void {
	m_resolver.set_external_ip_information(default_ip, matrix);
}

auto server_accepted_session::on_connect() -> void {
	// Intentionally blank
}

auto server_accepted_session::on_disconnect() -> void {
	// Intentionally blank
}

}