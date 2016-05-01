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

#include "Common/Types.hpp"
#include <string>

namespace Vana {
	class PacketBuilder;
	class PacketReader;

	namespace WorldServer {
		class WorldServerAcceptedSession;
		class LoginServerSession;

		namespace SyncHandler {
			// Dispatch
			auto handle(ref_ptr_t<WorldServerAcceptedSession> session, PacketReader &reader) -> void;
			auto handle(ref_ptr_t<LoginServerSession> session, PacketReader &reader) -> void;
			// Config
			auto handleConfigSync(PacketReader &reader) -> void;
		}
	}
}