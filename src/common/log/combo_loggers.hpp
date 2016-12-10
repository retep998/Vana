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
#include "common/types.hpp"
#include <memory>
#include <string>

namespace vana {
	namespace log {
		template <typename TLogger1, typename TLogger2>
		class dual_logger : public base_logger {
		public:
			dual_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size = 10) {
				m_logger1 = make_owned_ptr<TLogger1>(filename, format, time_format, type, buffer_size);
				m_logger2 = make_owned_ptr<TLogger2>(filename, format, time_format, type, buffer_size);
			}

			auto log(vana::log::type type, const opt_string &identifier, const string &message) -> void override {
				get_logger1()->log(type, identifier, message);
				get_logger2()->log(type, identifier, message);
			}
		private:
			auto get_logger1() const -> TLogger1 * { return m_logger1.get(); }
			auto get_logger2() const -> TLogger2 * { return m_logger2.get(); }
			owned_ptr<TLogger1> m_logger1;
			owned_ptr<TLogger2> m_logger2;
		};

		template <typename TLogger1, typename TLogger2, typename TLogger3>
		class tri_logger : public base_logger {
		public:
			tri_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size = 10) {
				m_logger1 = make_owned_ptr<TLogger1>(filename, format, time_format, type, buffer_size);
				m_logger2 = make_owned_ptr<TLogger2>(filename, format, time_format, type, buffer_size);
				m_logger3 = make_owned_ptr<TLogger3>(filename, format, time_format, type, buffer_size);
			}

			auto log(vana::log::type type, const opt_string &identifier, const string &message) -> void override {
				get_logger1()->log(type, identifier, message);
				get_logger2()->log(type, identifier, message);
				get_logger3()->log(type, identifier, message);
			}
		private:
			auto get_logger1() const -> TLogger1 * { return m_logger1.get(); }
			auto get_logger2() const -> TLogger2 * { return m_logger2.get(); }
			auto get_logger3() const -> TLogger3 * { return m_logger3.get(); }
			owned_ptr<TLogger1> m_logger1;
			owned_ptr<TLogger2> m_logger2;
			owned_ptr<TLogger3> m_logger3;
		};
	}
}