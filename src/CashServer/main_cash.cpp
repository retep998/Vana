/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "CashServer.h"
#include "ConnectionManager.h"
#ifdef _WIN32
#include "ExceptionReport.h"
#endif
#include <exception>
#include <functional>
#include <iostream>

#ifdef _WIN32

std::tr1::function<void()> console_ctrl_function;

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type) {
	switch (ctrl_type) {
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			console_ctrl_function();
			return TRUE;
		default:
			return FALSE;
	}
}

#endif

int main() {
	try {
#ifdef _WIN32
		CExceptionReport exr = CExceptionReport();
#endif
		CashServer *server = CashServer::Instance();
		ConnectionManager *connMan = ConnectionManager::Instance();

		server->initialize();

#ifdef _WIN32
		// Allow the server to stop on windows console events
		console_ctrl_function = std::tr1::bind(&ConnectionManager::stop, connMan);
		SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
#endif

		connMan->run();
		connMan->join();
	}
	catch (std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
	}
	return 0;
}
