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

#include "common/types.hpp"
#include "common/util/id_looper.hpp"

namespace vana {
	namespace util {
		// Keep track of a pool of identifiers and ensure that identifiers aren't reused
		template <typename TIdentifier>
		class id_pool {
		public:
			explicit id_pool(TIdentifier minimum = 1, TIdentifier maximum = std::numeric_limits<TIdentifier>::max()) :
				m_identifiers{minimum, maximum}
			{
			}

			auto lease() -> TIdentifier {
				if (free() == 0) {
					throw std::range_error{"all identifiers are consumed"};
				}

				TIdentifier ret;
				do {
					ret = m_identifiers.next();
				} while (m_taken.find(ret) != std::end(m_taken));

				m_taken.insert(ret);

				return ret;
			}

			auto taken() const -> size_t {
				return m_taken.size();
			}

			auto free() const -> size_t {
				return m_identifiers.range() - m_taken.size();
			}

			auto release(TIdentifier value) -> void {
				auto iter = m_taken.find(value);
				if (iter != std::end(m_taken)) {
					m_taken.erase(iter);
				}
			}
		private:
			id_looper<TIdentifier> m_identifiers;
			hash_set<TIdentifier> m_taken;
		};
	}
}