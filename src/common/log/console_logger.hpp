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

#include "common/log/base_logger.hpp"
#include <string>

namespace vana {
	namespace log {
		// Default logger/console
		class console_logger : public base_logger {
		public:
			console_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size = 10);

			auto log(vana::log::type type, const opt_string &identifier, const string &message) -> void override;
		};
	}
}