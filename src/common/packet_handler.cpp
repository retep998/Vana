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
#include "packet_handler.hpp"

namespace vana {

auto packet_handler::get_ip() const -> optional<ip> {
	if (m_disconnected) {
		return {};
	}
	return m_session->get_ip();
}

auto packet_handler::disconnect() -> void {
	if (m_disconnected) {
		return;
	}
	m_session->disconnect();
}

auto packet_handler::send(const packet_builder &builder) -> void {
	if (m_disconnected) {
		return;
	}
	m_session->send(builder);
}

auto packet_handler::get_latency() const -> milliseconds {
	if (m_disconnected) {
		return milliseconds{0};
	}
	return m_session->get_latency();
}

auto packet_handler::handle(packet_reader &reader) -> result {
	return result::success;
}

auto packet_handler::on_connect_base(ref_ptr<session> session) -> void {
	m_session = session;
	on_connect();
}

auto packet_handler::on_disconnect_base() -> void {
	m_session.reset();
	m_disconnected = true;
	on_disconnect();
}

auto packet_handler::on_connect() -> void {
	// Intentionally blank
}

auto packet_handler::on_disconnect() -> void {
	// Intentionally blank
}

}