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

#include "common/ExternalIp.hpp"
#include "common/ExternalIpResolver.hpp"
#include "common/Ip.hpp"
#include "common/Types.hpp"

namespace vana {
	namespace login_server {
		class channel : public external_ip_resolver {
			NONCOPYABLE(channel);
		public:
			channel() = default;
			auto set_port(connection_port port) -> void { m_port = port; }
			auto set_population(int32_t population) -> void { m_population = population; }
			auto get_port() const -> connection_port { return m_port; }
			auto get_population() const -> int32_t { return m_population; }
		private:
			connection_port m_port = 0;
			int32_t m_population = 0;
		};
	}
}