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

#include "common/types_temp.hpp"
#include <string>

namespace vana {
	class packet_builder;
	class packet_reader;

	namespace world_server {
		class world_server_accepted_session;
		class login_server_session;

		namespace sync_handler {
			// Dispatch
			auto handle(ref_ptr<world_server_accepted_session> session, packet_reader &reader) -> void;
			auto handle(ref_ptr<login_server_session> session, packet_reader &reader) -> void;
			// Config
			auto handle_config_sync(packet_reader &reader) -> void;
		}
	}
}