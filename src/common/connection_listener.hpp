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

#include "common/connection_listener_config.hpp"
#include "common/session_temp.hpp"
#include "common/types_temp.hpp"
#include <asio.hpp>

namespace vana {
	class connection_manager;

	class connection_listener {
	public:
		connection_listener(
			const connection_listener_config &config,
			handler_creator handler_creator,
			asio::io_service &io_service,
			asio::ip::tcp::endpoint endpoint,
			connection_manager &manager);

		auto begin_accept() -> void;
		auto stop() -> void;
	private:
		connection_listener_config m_config;
		asio::ip::tcp::acceptor m_acceptor;
		connection_manager &m_manager;
		handler_creator m_handler_creator;
	};
}