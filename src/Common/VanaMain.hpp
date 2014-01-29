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
	function_t<void()> consoleHandler;

	auto WINAPI consoleControlHandler(DWORD ctrlType) -> BOOL {
		switch (ctrlType) {
			case CTRL_C_EVENT:
			case CTRL_BREAK_EVENT:
			case CTRL_CLOSE_EVENT:
			case CTRL_SHUTDOWN_EVENT:
				consoleHandler();
				return TRUE;
		}
		return FALSE;
	}

	auto hookWin32Console(AbstractServer &server) -> void {
		ConnectionManager &manager = ConnectionManager::getInstance();
		consoleHandler = [&server] { server.shutdown(); };
		SetConsoleCtrlHandler(consoleControlHandler, TRUE);
		manager.run();
		manager.join();
	}
#endif

	template <typename TAbstractServer>
	auto main() -> int {
		Botan::LibraryInitializer init("thread_safe=true");
		try {
			AbstractServer &server = TAbstractServer::getInstance().initialize();

#ifdef WIN32
			hookWin32Console(server);
#endif
		}
		catch (std::exception &e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
			ExitCodes::exit(ExitCodes::ProgramException);
		}
		return ExitCodes::Ok;
	}
}