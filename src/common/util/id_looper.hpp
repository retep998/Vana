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

#include <limits>
#include <type_traits>

namespace vana {
	namespace util {
		// Keep track and returns a number between min and max, restarts when number reaches max
		template <typename TIdentifier>
		class id_looper {
		public:
			explicit id_looper(TIdentifier minimum = 1, TIdentifier maximum = std::numeric_limits<TIdentifier>::max()) :
				m_current{minimum},
				m_minimum{minimum},
				m_maximum{maximum}
			{
				if (maximum <= minimum) {
					throw std::domain_error{"maximum must be greater than minimum"};
				}
				if (minimum == type_min() && maximum == type_max()) {
					throw std::domain_error{"range is not representible in the unsigned version of the TIdentifier type"};
				}
			}

			auto next() -> TIdentifier {
				TIdentifier ret = m_current;
				if (m_current == m_maximum) {
					m_current = m_minimum;
				}
				else {
					m_current++;
				}
				return ret;
			}

			using unsigned_type = typename std::make_unsigned_t<TIdentifier>;
			auto range() const -> unsigned_type { return range_helper<TIdentifier>(); }
		private:
			static auto type_max() -> TIdentifier { return std::numeric_limits<TIdentifier>::max(); }
			static auto type_min() -> TIdentifier { return std::numeric_limits<TIdentifier>::min(); }
			static auto abs(TIdentifier val) -> typename unsigned_type {
				if (val < 0) return static_cast<unsigned_type>(-val);
				return static_cast<unsigned_type>(val);
			}
			template <typename THelper>
			auto range_helper() const -> typename std::enable_if_t<std::is_unsigned<THelper>::value, unsigned_type> {
				// Type is already unsigned, the only unrepresentible value is [0, maxInt]
				// Which is taken care of by the constructor
				return (m_maximum - m_minimum) + 1;
			}
			template <typename THelper>
			auto range_helper() const -> typename std::enable_if_t<std::is_signed<THelper>::value, unsigned_type> {
				if (((m_maximum <= 0) == (m_minimum < 0)) || m_maximum == type_max() && m_minimum == 0) {
					// In the case where both are <= 0 or both are > 0, this is simple logic because the range is fully representible as a positive signed type
					// The only values that are not representible at this point are [0, maxInt] and [minInt, 0]
					// Which are taken care of by the cast here
					return static_cast<unsigned_type>(m_maximum - m_minimum) + 1;
				}
				// We have mixed signedness which could mean up to double the type's max value in representability
				// For this reason, we use distance from 0 and cast it into the unsigned type to allow us to do the math
				// So if we have [-2147, 2147], we end up with 2147 + 2147 + 1 or 4295 possible values
				// If we have mixed signedness and max is > min (validated by constructor), only min can be negative and needs abs
				// We already validated whether max and min had the same sign, so that case doesn't apply here
				return static_cast<unsigned_type>(m_maximum) + abs(m_minimum) + 1;
			}

			TIdentifier m_current;
			TIdentifier m_minimum;
			TIdentifier m_maximum;
		};
	}
}