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
#include "CustomFunctions.hpp"
#include "channel_server/Player.hpp"

namespace vana {
namespace channel_server {

auto custom_functions::initialize(case_insensitive_hash_map<chat_command, game_chat> &command_list) -> void {
	// Define any custom functions you may have here, just like chat_handler_functions::initialize
	chat_command command;

}

}
}