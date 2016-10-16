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
#include "common/types.hpp"
#include "common/util/nullable_mode.hpp"
#include <cstdlib>
#include <string>
#include <vector>

namespace vana {
	namespace util {
		namespace misc {
			inline
			auto get_connection_type(server_type type) -> connection_type {
				switch (type) {
					case server_type::login: return connection_type::login;
					case server_type::world: return connection_type::world;
					case server_type::channel: return connection_type::channel;
					case server_type::cash: return connection_type::cash;
					case server_type::mts: return connection_type::mts;
					default: THROW_CODE_EXCEPTION(not_implemented_exception, "server_type");
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
					default: THROW_CODE_EXCEPTION(not_implemented_exception, "connection_type");
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
			auto get_optional(const TElement &test_value, vana::util::nullable_mode mode, init_list<TElement> nullable_values) -> optional<TElement> {
				optional<TElement> ret;
				if (mode == vana::util::nullable_mode::null_if_found) {
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
				else if (mode == vana::util::nullable_mode::force_not_null) {
					ret = test_value;
				}
				return ret;
			}

			template <typename TInteger>
			inline
			auto integer_div_ceil(TInteger value, TInteger divisor) -> TInteger {
				if (value <= 0) throw std::invalid_argument{"Value must be 1 or greater"};
				if (divisor <= 0) throw std::invalid_argument{"Divisor must be 1 or greater"};
				return 1 + ((value - 1) / divisor);
			};

			template <typename TInteger>
			inline
			auto is_non_overflowing_addition(TInteger value, TInteger modification) -> bool {
				if (value == 0 || modification == 0) {
					// If value is 0, you could add min or add max, either would be fine
					// If modification is 0, it's an identity operation on value
					return true;
				}
				if (value < 0) {
					if (modification > 0) {
						// No addition of positives to a negative will ever overflow
						return true;
					}
					// Our value is negative and we're adding a negative
					// We resolve the remaining space based on the value
					// If we already have the minimum value, the modification can only be 0 successfully
					// If we have, say, -1, the remaining space is 2^(signed_bit_count) - 2
					// Our modification has to be greater than the remaining amount
					// This is due to the fact that we're resolving a negative
					// e.g.
					// "Maximum" based on bit space: -16
					// Value: -7
					// Remaining: -16 - -7 = -9
					// Modification: -8
					// Modification is greater than -9, successful
					// Modification: -10
					// Modification is less than -9, failed
					TInteger remaining = std::numeric_limits<TInteger>::min() - value;
					return modification >= remaining;
				}

				if (modification < 0) {
					// No addition of negatives to a positive will ever overflow
					return true;
				}

				TInteger remaining = std::numeric_limits<TInteger>::max() - value;
				return modification <= remaining;
			}

			template <typename TInteger>
			inline
			auto is_non_overflowing_addition_with_zero_min(TInteger value, TInteger modification) -> bool {
				if (value == 0 || modification == 0) {
					return true;
				}

				if (value < 0) {
					// Categorically reject this operation if the starting point is negative
					// Even though two negatives may make a positive
					return false;
				}
				if (modification == std::numeric_limits<TInteger>::min()) {
					// Signed integer negate is undefined on this value
					return false;
				}

				return modification < 0 ?
					// If we're removing, we make sure that there are more than are being removed
					value >= -modification :
					// If we're adding, we make sure that there are more available than are being added
					(std::numeric_limits<TInteger>::max() - value) >= modification;
			}
		}
	}
}