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

namespace vana {
	class packet_reader;

	namespace login_server {
		class user;

		namespace login {
			auto login_user(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto set_gender(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto handle_login(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto register_pin(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto login_back(ref_ptr<user> user_value) -> void;
			auto check_pin(ref_ptr<user> user_value, packet_reader &reader) -> void;
		}
	}
}