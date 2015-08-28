/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Logger.hpp"
#include "Types.hpp"
#include <memory>
#include <string>

namespace Vana {
	template <typename TLogger1, typename TLogger2>
	class DuoLogger : public Logger {
	public:
		DuoLogger(const string_t &filename, const string_t &format, const string_t &timeFormat, ServerType serverType, size_t bufferSize = 10) {
			m_logger1 = make_owned_ptr<TLogger1>(filename, format, timeFormat, serverType, bufferSize);
			m_logger2 = make_owned_ptr<TLogger2>(filename, format, timeFormat, serverType, bufferSize);
		}

		auto log(LogType type, const opt_string_t &identifier, const string_t &message) -> void override {
			getLogger1()->log(type, identifier, message);
			getLogger2()->log(type, identifier, message);
		}
	private:
		auto getLogger1() const -> TLogger1 * { return m_logger1.get(); }
		auto getLogger2() const -> TLogger2 * { return m_logger2.get(); }
		owned_ptr_t<TLogger1> m_logger1;
		owned_ptr_t<TLogger2> m_logger2;
	};

	template <typename TLogger1, typename TLogger2, typename TLogger3>
	class TriLogger : public Logger {
	public:
		TriLogger(const string_t &filename, const string_t &format, const string_t &timeFormat, ServerType serverType, size_t bufferSize = 10) {
			m_logger1 = make_owned_ptr<TLogger1>(filename, format, timeFormat, serverType, bufferSize);
			m_logger2 = make_owned_ptr<TLogger2>(filename, format, timeFormat, serverType, bufferSize);
			m_logger3 = make_owned_ptr<TLogger3>(filename, format, timeFormat, serverType, bufferSize);
		}

		auto log(LogType type, const opt_string_t &identifier, const string_t &message) -> void override {
			getLogger1()->log(type, identifier, message);
			getLogger2()->log(type, identifier, message);
			getLogger3()->log(type, identifier, message);
		}
	private:
		auto getLogger1() const -> TLogger1 * { return m_logger1.get(); }
		auto getLogger2() const -> TLogger2 * { return m_logger2.get(); }
		auto getLogger3() const -> TLogger3 * { return m_logger3.get(); }
		owned_ptr_t<TLogger1> m_logger1;
		owned_ptr_t<TLogger2> m_logger2;
		owned_ptr_t<TLogger3> m_logger3;
	};
}