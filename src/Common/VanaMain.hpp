/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "ConnectionManager.hpp"
#include "ExitCodes.hpp"
#include <botan/botan.h>
#include <exception>
#include <functional>
#include <iostream>

namespace Vana {
#ifdef WIN32
	function_t<void()> consoleControlFunction;

	auto WINAPI consoleControlHandler(DWORD ctrl_type) -> BOOL {
		switch (ctrl_type) {
			case CTRL_C_EVENT:
			case CTRL_BREAK_EVENT:
			case CTRL_CLOSE_EVENT:
			case CTRL_SHUTDOWN_EVENT:
				consoleControlFunction();
				return TRUE;
		}
		return FALSE;
	}
#endif

	template <typename TAbstractServer>
	auto main() -> int {
		Botan::LibraryInitializer init("thread_safe=true");
		try {
			TAbstractServer &server = TAbstractServer::getInstance();
			ConnectionManager &connMan = ConnectionManager::getInstance();

			server.initialize();
#ifdef WIN32
			// Allow the server to stop on windows console events
			consoleControlFunction = [&server]() { server.shutdown(); };
			SetConsoleCtrlHandler(consoleControlHandler, TRUE);
			connMan.run();
			connMan.join();
#endif
		}
		catch (std::exception &e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
			ExitCodes::exit(ExitCodes::ProgramException);
		}
		return ExitCodes::Ok;
	}
}