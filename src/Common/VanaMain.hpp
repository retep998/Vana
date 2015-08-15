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
#pragma once

#include "AbstractServer.hpp"
#include "ConfigFile.hpp"
#include "ExitCodes.hpp"
#include "ThreadPool.hpp"
#include <asio.hpp>
#include <botan/botan.h>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>

namespace Vana {
	extern exit_code_t exitCode;

	template <typename TAbstractServer>
	auto main() -> exit_code_t {
		Botan::LibraryInitializer init{"thread_safe=true"};
		asio::io_service s;
		asio::signal_set signals{s, SIGINT};

		try {
			AbstractServer &server = TAbstractServer::getInstance();

			signals.async_wait([&server](const asio::error_code &ec, int handlerId) {
				server.shutdown();
			});

			if (server.initialize() == Result::Successful) {
				s.run();
			}
			else {
				server.shutdown();
			}
		}
		catch (ConfigException &) { }
		catch (std::exception &e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
			ExitCodes::exit(ExitCodes::ProgramException);
		}

		return exitCode;
	}
}