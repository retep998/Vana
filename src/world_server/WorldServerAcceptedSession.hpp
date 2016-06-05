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

#include "common_temp/ServerAcceptedSession.hpp"
#include "common_temp/Types.hpp"

namespace vana {
	class packet_reader;

	namespace world_server {
		class world_server_accepted_session final : public server_accepted_session, public enable_shared<world_server_accepted_session> {
			NONCOPYABLE(world_server_accepted_session);
		public:
			world_server_accepted_session(abstract_server &server);

			auto get_channel() const -> game_channel_id;
		protected:
			auto handle(packet_reader &reader) -> result override;
			auto authenticated(server_type type) -> void override;
			auto on_disconnect() -> void override;
		private:
			game_channel_id m_channel = 0;
		};
	}
}