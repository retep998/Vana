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
#include "PacketHandler.hpp"

namespace Vana {

auto PacketHandler::getIp() const -> optional_t<Ip> {
	if (m_disconnected) {
		return {};
	}
	return m_session->getIp();
}

auto PacketHandler::disconnect() -> void {
	if (m_disconnected) {
		return;
	}
	m_session->disconnect();
}

auto PacketHandler::send(const PacketBuilder &builder) -> void {
	if (m_disconnected) {
		return;
	}
	m_session->send(builder);
}

auto PacketHandler::getLatency() const -> milliseconds_t {
	if (m_disconnected) {
		return milliseconds_t{0};
	}
	return m_session->getLatency();
}

auto PacketHandler::handle(PacketReader &reader) -> Result {
	return Result::Successful;
}

auto PacketHandler::onConnectBase(ref_ptr_t<Session> session) -> void {
	m_session = session;
	onConnect();
}

auto PacketHandler::onDisconnectBase() -> void {
	m_session.reset();
	m_disconnected = true;
	onDisconnect();
}

auto PacketHandler::onConnect() -> void {
	// Intentionally blank
}

auto PacketHandler::onDisconnect() -> void {
	// Intentionally blank
}

}