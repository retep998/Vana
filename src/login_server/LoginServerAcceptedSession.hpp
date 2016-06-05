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

#include "common/ServerAcceptedSession.hpp"
#include "common/Types.hpp"

namespace vana {
	class packet_reader;

	namespace login_server {
		class login_server_accepted_session final : public server_accepted_session, public enable_shared<login_server_accepted_session> {
			NONCOPYABLE(login_server_accepted_session);
		public:
			login_server_accepted_session(abstract_server &server);

			auto set_world_id(game_world_id id) -> void;
			auto get_world_id() const -> optional<game_world_id>;
		protected:
			auto handle(packet_reader &reader) -> result override;
			auto authenticated(server_type type) -> void override;
			auto on_disconnect() -> void override;
		private:
			optional<game_world_id> m_world_id;
		};
	}
}