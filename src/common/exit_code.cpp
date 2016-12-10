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

#include "common/exit_code.hpp"
#include "common/vana_main.hpp"
#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

namespace vana {

auto exit(exit_code code) -> void {
#ifndef DAEMON
	std::cout << "Please press enter to quit..." << std::endl;
	std::cin.get();
#endif
	vana::g_exit_code = code;
	raise(SIGINT);
}

}