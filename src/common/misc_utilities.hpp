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

#include "common/connection_type.hpp"
#include "common/server_type.hpp"
#include "common/types_temp.hpp"
#include <cstdlib>
#include <string>
#include <vector>

namespace vana {
	namespace utilities {
		namespace misc {
			enum class nullable_mode {
				null_if_found = 1,
				force_not_null = 2,
				force_null = 3
			};

			inline
			auto get_connection_type(server_type type) -> connection_type {
				switch (type) {
					case server_type::login: return connection_type::login;
					case server_type::world: return connection_type::world;
					case server_type::channel: return connection_type::channel;
					case server_type::cash: return connection_type::cash;
					case server_type::mts: return connection_type::mts;
					default: throw not_implemented_exception{"server_type"};
				}
			}

			inline
			auto get_server_type(connection_type type) -> server_type {
				switch (type) {
					case connection_type::login: return server_type::login;
					case connection_type::world: return server_type::world;
					case connection_type::channel: return server_type::channel;
					case connection_type::cash: return server_type::cash;
					case connection_type::mts: return server_type::mts;
					default: throw not_implemented_exception{"connection_type"};
				}
			}

			template <typename TElement>
			auto convert_linear_scale(
				const TElement &value,
				const TElement &value_domain_min,
				const TElement &value_domain_max,
				const TElement &new_domain_min,
				const TElement &new_domain_max) -> TElement
			{
				if (value_domain_min >= value_domain_max) throw std::invalid_argument{"Domain min must be below domain max"};
				if (new_domain_min >= new_domain_max) throw std::invalid_argument{"Domain min must be below domain max"};
				if (value < value_domain_min || value > value_domain_max) throw std::invalid_argument{"Value must be within domain"};
				double value_range = value_domain_max - value_domain_min;
				double new_range = new_domain_max - new_domain_min;
				return static_cast<TElement>(
					((static_cast<double>(value) - new_domain_min) / value_range) *
					new_range + new_domain_min);
			}

			template <typename TElement>
			auto get_optional(const TElement &test_value, nullable_mode mode, init_list<TElement> nullable_values) -> optional<TElement> {
				optional<TElement> ret;
				if (mode == nullable_mode::null_if_found) {
					bool found = false;
					for (const auto &null_value : nullable_values) {
						if (test_value == null_value) {
							found = true;
							break;
						}
					}
					if (!found) {
						ret = test_value;
					}
				}
				else if (mode == nullable_mode::force_not_null) {
					ret = test_value;
				}
				return ret;
			}
		}
	}
}