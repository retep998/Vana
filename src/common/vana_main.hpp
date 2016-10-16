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

#include "common/abstract_server.hpp"
#include "common/exit_code.hpp"
#include "common/lua/config_file.hpp"
#include <asio.hpp>
#include <botan/botan.h>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>

namespace vana {
	extern exit_code g_exit_code;

	template <typename TAbstractServer>
	auto main() -> exit_code_underlying {
		Botan::LibraryInitializer init{"thread_safe=true"};
		asio::io_service s;
		asio::signal_set signals{s, SIGINT};

		try {
			abstract_server &server = TAbstractServer::get_instance();
			signals.async_wait([&server](const asio::error_code &ec, int handler_id) {
				server.shutdown();
			});

			if (server.initialize() == result::success) {
				s.run();
			}
			else {
				server.shutdown();
			}
		}
#if DEBUG
		catch (not_implemented_exception &e) {
			std::cerr << "PROGRAM HIT UNIMPLEMENTED FEATURE AT " << e.get_file() << " L" << e.get_line() << ": " << e.what() << std::endl;
			exit(exit_code::program_exception);
		}
		catch (invalid_operation_exception &e) {
			std::cerr << "PROGRAM HIT INVALID OPERATION AT " << e.get_file() << " L" << e.get_line() << ": " << e.what() << std::endl;
			exit(exit_code::program_exception);
		}
		catch (codepath_invalid_exception &e) {
			std::cerr << "PROGRAM HIT INVALID CODE PATH AT " << e.get_file() << " L" << e.get_line() << ": " << e.what() << std::endl;
			exit(exit_code::program_exception);
		}
#endif
		catch (lua::config_exception &) {
			// Code path intentionally blank
			// Each vanilla config_exception has an associated message at the throw site
		}
		catch (std::exception &e) {
			std::cerr << "PROGRAM ERROR: " << e.what() << std::endl;
			exit(exit_code::program_exception);
		}

		return static_cast<exit_code_underlying>(g_exit_code);
	}
}