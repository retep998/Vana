/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ConnectionManager.hpp"
#include "Common/ConnectionListener.hpp"
#include "Common/ConnectionListenerConfig.hpp"
#include "Common/EncryptedPacketTransformer.hpp"
#include "Common/ExitCodes.hpp"
#include "Common/InterServerConfig.hpp"
#include "Common/MiscUtilities.hpp"
#include "Common/Session.hpp"
#include "Common/ThreadPool.hpp"

namespace Vana {

ConnectionManager::ConnectionManager(AbstractServer *server) :
	m_server{server}
{
	m_work = make_owned_ptr<asio::io_service::work>(m_ioService);
}

ConnectionManager::~ConnectionManager() {
	// m_work.reset() needs to be a pre-wait hook and in the destructor for the cases where the thread is never leased (e.g. DB unavailable)
	// Doing this a second time doesn't harm an already-reset m_work pointer, so we're in the clear
	m_work.reset();
	m_thread.reset();
}

auto ConnectionManager::listen(const ConnectionListenerConfig &config, HandlerCreator handlerCreator) -> void {
	asio::ip::tcp::endpoint endpoint{
		config.ipType == Ip::Type::Ipv4 ?
			asio::ip::tcp::v4() :
			asio::ip::tcp::v6(),
		config.port
	};

	auto listener = make_ref_ptr<ConnectionListener>(config, handlerCreator, m_ioService, endpoint, *this);
	m_servers.push_back(listener);
	listener->beginAccept();
}

auto ConnectionManager::connect(const Ip &destination, port_t port, const PingConfig &ping, ServerType sourceType, HandlerCreator handlerCreator) -> pair_t<Result, ref_ptr_t<Session>> {
	asio::ip::address endAddress;
	if (destination.getType() == Ip::Type::Ipv4) {
		endAddress = asio::ip::address_v4{destination.asIpv4()};
	}
	else {
		throw NotImplementedException{"IPv6 unsupported"};
	}
	asio::ip::tcp::endpoint endpoint{endAddress, port};
	asio::error_code error;

	auto handler = handlerCreator();
	auto newSession = make_ref_ptr<Session>(
		m_ioService,
		*this,
		handler);

	newSession->getSocket().connect(endpoint, error);

	if (!error) {
		// Now let's process the connect packet
		try {
			auto result = newSession->syncRead(10); // May require maintenance if the IV packet ever dips below 10 bytes
			if (result.first) {
				std::cerr << "SESSION SYNCREAD ERROR: " << result.first.message() << std::endl;
				ExitCodes::exit(ExitCodes::ServerConnectionError);
				newSession->disconnect();
			}
			else {
				PacketReader &reader = result.second;

				header_t header = reader.get<header_t>(); // Gives us the packet length
				version_t version = reader.get<version_t>();
				string_t subversion = reader.get<string_t>();
				iv_t sendIv = reader.get<iv_t>();
				iv_t recvIv = reader.get<iv_t>();
				game_locale_t locale = reader.get<game_locale_t>();

				if (version != MapleVersion::Version || locale != MapleVersion::Locale || subversion != MapleVersion::LoginSubversion) {
					std::cerr << "ERROR: The server you are connecting to lacks the same MapleStory version." << std::endl;
					std::cerr << "Expected locale/version (subversion): " << static_cast<int16_t>(locale) << "/" << version << " (" << subversion << ")" << std::endl;
					std::cerr << "Local locale/version (subversion): " << static_cast<int16_t>(MapleVersion::Locale) << "/" << MapleVersion::Version << " (" << MapleVersion::LoginSubversion << ")" << std::endl;
					newSession->disconnect();
					ExitCodes::exit(ExitCodes::ServerVersionMismatch);
				}
				else {
					newSession->setType(MiscUtilities::getConnectionType(sourceType));
					newSession->start(ping, make_ref_ptr<EncryptedPacketTransformer>(recvIv, sendIv));

					m_sessions.insert(newSession);

					return std::make_pair(Result::Successful, newSession);
				}
			}
		}
		catch (PacketContentException) {
			std::cerr << "ERROR: Malformed IV packet" << std::endl;
			newSession->disconnect();
			ExitCodes::exit(ExitCodes::ServerMalformedIvPacket);
		}
	}
	else {
		std::cerr << "CONNECTMANAGER CONNECT ERROR: " << error.message() << std::endl;
		ExitCodes::exit(ExitCodes::ServerConnectionError);
	}

	return std::make_pair(Result::Failure, ref_ptr_t<Session>{nullptr});
}

auto ConnectionManager::stop() -> void {
	for (auto &server : m_servers) {
		server->stop();
	}
	m_servers.clear();

	auto sessions = m_sessions;
	for (auto &session : sessions) {
		session->disconnect();
	}
}

auto ConnectionManager::stop(ref_ptr_t<Session> session) -> void {
	m_sessions.erase(session);
}

auto ConnectionManager::start(ref_ptr_t<Session> session) -> void {
	m_sessions.insert(session);
}

auto ConnectionManager::getServer() -> AbstractServer * {
	return m_server;
}

auto ConnectionManager::run() -> void {
	m_thread = ThreadPool::lease(
		[this] { m_ioService.run(); },
		[this] { m_work.reset(); });
}

}