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
#include "connection_manager.hpp"
#include "common/config/inter_server.hpp"
#include "common/connection_listener.hpp"
#include "common/connection_listener_config.hpp"
#include "common/encrypted_packet_transformer.hpp"
#include "common/exit_code.hpp"
#include "common/session.hpp"
#include "common/util/misc.hpp"
#include "common/util/thread_pool.hpp"

namespace vana {

connection_manager::connection_manager(abstract_server *server) :
	m_server{server}
{
	m_work = make_owned_ptr<asio::io_service::work>(m_io_service);
}

connection_manager::~connection_manager() {
	// m_work.reset() needs to be a pre-wait hook and in the destructor for the cases where the thread is never leased (e.g. DB unavailable)
	// Doing this a second time doesn't harm an already-reset m_work pointer, so we're in the clear
	m_work.reset();
	m_thread.reset();
}

auto connection_manager::listen(const connection_listener_config &config, handler_creator handler_creator) -> void {
	asio::ip::tcp::endpoint endpoint{
		config.ip_type == ip::type::ipv4 ?
			asio::ip::tcp::v4() :
			asio::ip::tcp::v6(),
		config.port
	};

	auto listener = make_ref_ptr<connection_listener>(config, handler_creator, m_io_service, endpoint, *this);
	m_servers.push_back(listener);
	listener->begin_accept();
}

auto connection_manager::connect(const ip &destination, connection_port port, const config::ping &ping, server_type source_type, handler_creator handler_creator) -> pair<result, ref_ptr<session>> {
	asio::ip::address end_address;
	if (destination.get_type() == ip::type::ipv4) {
		end_address = asio::ip::address_v4{destination.as_ipv4()};
	}
	else {
		THROW_CODE_EXCEPTION(not_implemented_exception, "IPv6 unsupported");
	}
	asio::ip::tcp::endpoint endpoint{end_address, port};
	asio::error_code error;

	auto handler = handler_creator();
	auto new_session = make_ref_ptr<session>(
		m_io_service,
		*this,
		handler);

	new_session->get_socket().connect(endpoint, error);

	if (!error) {
		// Now let's process the connect packet
		try {
			auto result = new_session->sync_read(10); // May require maintenance if the IV packet ever dips below 10 bytes
			if (result.first) {
				std::cerr << "SESSION SYNCREAD ERROR: " << result.first.message() << std::endl;
				exit(exit_code::server_connection_error);
				new_session->disconnect();
			}
			else {
				packet_reader &reader = result.second;

				packet_header header = reader.get<packet_header>(); // Gives us the packet length
				game_version version = reader.get<game_version>();
				string subversion = reader.get<string>();
				crypto_iv send_iv = reader.get<crypto_iv>();
				crypto_iv recv_iv = reader.get<crypto_iv>();
				game_locale locale = reader.get<game_locale>();

				if (version != maple_version::version || locale != maple_version::locale || subversion != maple_version::login_subversion) {
					std::cerr << "ERROR: The server you are connecting to lacks the same MapleStory version." << std::endl;
					std::cerr << "Expected locale/version (subversion): " << static_cast<int16_t>(locale) << "/" << version << " (" << subversion << ")" << std::endl;
					std::cerr << "Local locale/version (subversion): " << static_cast<int16_t>(maple_version::locale) << "/" << maple_version::version << " (" << maple_version::login_subversion << ")" << std::endl;
					new_session->disconnect();
					exit(exit_code::server_version_mismatch);
				}
				else {
					new_session->set_type(vana::util::misc::get_connection_type(source_type));
					new_session->start(ping, make_ref_ptr<encrypted_packet_transformer>(recv_iv, send_iv));

					m_sessions.insert(new_session);

					return std::make_pair(result::success, new_session);
				}
			}
		}
		catch (packet_content_exception) {
			std::cerr << "ERROR: Malformed IV packet" << std::endl;
			new_session->disconnect();
			exit(exit_code::server_malformed_iv_packet);
		}
	}
	else {
		std::cerr << "CONNECTMANAGER CONNECT ERROR: " << error.message() << std::endl;
		exit(exit_code::server_connection_error);
	}

	return std::make_pair(result::failure, ref_ptr<session>{nullptr});
}

auto connection_manager::stop() -> void {
	m_stopping = true;
	for (auto &server : m_servers) {
		server->stop();
	}
	m_servers.clear();

	for (auto &session : m_sessions) {
		session->disconnect();
	}
	m_sessions.clear();
}

auto connection_manager::stop(ref_ptr<session> session) -> void {
	if (m_stopping) return;
	m_sessions.erase(session);
}

auto connection_manager::start(ref_ptr<session> session) -> void {
	if (m_stopping) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	m_sessions.insert(session);
}

auto connection_manager::get_server() -> abstract_server * {
	return m_server;
}

auto connection_manager::run() -> void {
	m_thread = vana::util::thread_pool::lease(
		[this] { m_io_service.run(); },
		[this] { m_work.reset(); });
}

}