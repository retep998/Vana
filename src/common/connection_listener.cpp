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
#include "connection_listener.hpp"
#include "common/common_packet.hpp"
#include "common/config/inter_server.hpp"
#include "common/connection_manager.hpp"
#include "common/encrypted_packet_transformer.hpp"
#include "common/session.hpp"
#include "common/util/thread_pool.hpp"

namespace vana {

connection_listener::connection_listener(
	const connection_listener_config &config,
	handler_creator handler_creator,
	asio::io_service &io_service,
	asio::ip::tcp::endpoint endpoint,
	connection_manager &manager) :
	m_acceptor{io_service, endpoint},
	m_config{config},
	m_handler_creator{handler_creator},
	m_manager{manager}
{
}

void connection_listener::begin_accept() {
	auto handler = m_handler_creator();
	auto new_session = make_ref_ptr<session>(
		m_acceptor.get_io_service(),
		m_manager,
		handler);

	m_manager.start(new_session);
	m_acceptor.async_accept(new_session->get_socket(), [this, new_session](const asio::error_code &error) mutable {
		if (!error) {
			new_session->set_type(m_config.type);

			crypto_iv recv_iv = 0;
			crypto_iv send_iv = 0;
			if (!m_config.encrypt) {
				new_session->start(m_config.ping, make_ref_ptr<packet_transformer>());
			}
			else {
				recv_iv = vana::util::randomizer::rand<crypto_iv>();
				send_iv = vana::util::randomizer::rand<crypto_iv>();
				new_session->start(m_config.ping, make_ref_ptr<encrypted_packet_transformer>(recv_iv, send_iv));
			}

			new_session->send(
				packets::connect(
					m_config.subversion,
					recv_iv,
					send_iv),
				false);

			this->begin_accept();
		}
		else if (error.value() == asio::error::operation_aborted) {
			// Intentionally blank
		}
		else {
			std::cerr << "BEGINACCEPT ERROR: " << error.message() << std::endl;
			this->m_manager.stop(new_session);
		}
	});
}

void connection_listener::stop() {
	m_acceptor.close();
}

}