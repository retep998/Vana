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
#include "ConnectionListener.hpp"
#include "Common/CommonPacket.hpp"
#include "Common/ConnectionManager.hpp"
#include "Common/EncryptedPacketTransformer.hpp"
#include "Common/InterServerConfig.hpp"
#include "Common/Session.hpp"
#include "Common/ThreadPool.hpp"

namespace Vana {

ConnectionListener::ConnectionListener(
	const ConnectionListenerConfig &config,
	HandlerCreator handlerCreator,
	asio::io_service &ioService,
	asio::ip::tcp::endpoint endpoint,
	ConnectionManager &manager) :
	m_acceptor{ioService, endpoint},
	m_config{config},
	m_handlerCreator{handlerCreator},
	m_manager{manager}
{
}

void ConnectionListener::beginAccept() {
	auto handler = m_handlerCreator();
	auto newSession = make_ref_ptr<Session>(
		m_acceptor.get_io_service(),
		m_manager,
		handler);

	m_manager.start(newSession);
	m_acceptor.async_accept(newSession->getSocket(), [this, newSession](const asio::error_code &error) mutable {
		if (!error) {
			newSession->setType(m_config.type);

			iv_t recvIv = 0;
			iv_t sendIv = 0;
			if (!m_config.encrypt) {
				newSession->start(m_config.ping, make_ref_ptr<PacketTransformer>());
			}
			else {
				recvIv = Randomizer::rand<iv_t>();
				sendIv = Randomizer::rand<iv_t>();
				newSession->start(m_config.ping, make_ref_ptr<EncryptedPacketTransformer>(recvIv, sendIv));
			}

			newSession->send(
				Packets::connect(
					m_config.subversion,
					recvIv,
					sendIv),
				false);

			this->beginAccept();
		}
		else if (error.value() == asio::error::operation_aborted) {
			// Intentionally blank
		}
		else {
			std::cerr << "BEGINACCEPT ERROR: " << error.message() << std::endl;
			this->m_manager.stop(newSession);
		}
	});
}

void ConnectionListener::stop() {
	m_acceptor.close();
}

}