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
		// Keep track of a pool of objects and ensure that identifiers aren't reused
		template <typename TIdentifier, typename TObject>
		class object_pool {
			using container = typename hash_map<TIdentifier, TObject>;

		public:
			explicit object_pool(TIdentifier minimum = 1, TIdentifier maximum = std::numeric_limits<TIdentifier>::max()) :
				m_identifiers{minimum, maximum}
			{
			}

			auto store(TObject obj) -> TIdentifier {
				if (free() == 0) {
					throw std::range_error{"all identifiers are consumed"};
				}

				TIdentifier ret;
				do {
					ret = m_identifiers.next();
				} while (find(ret) != end());

				m_taken.emplace(ret, obj);

				return ret;
			}

			using iterator = typename container::iterator;
			using const_iterator = typename container::const_iterator;

			auto begin() -> iterator { return std::begin(m_taken); }
			auto end() -> iterator { return std::end(m_taken); }
			auto cbegin() const -> const_iterator { return std::cbegin(m_taken); }
			auto cend() const -> const_iterator { return std::cend(m_taken); }

			auto taken() const -> size_t {
				return m_taken.size();
			}

			auto free() const -> size_t {
				return m_identifiers.range() - m_taken.size();
			}

			auto find(TIdentifier id) -> iterator {
				return m_taken.find(id);
			}

			auto release(TIdentifier value) -> void {
				auto iter = find(value);
				if (iter != end()) {
					m_taken.erase(iter);
				}
			}
		private:
			id_looper<TIdentifier> m_identifiers;
			container m_taken;
		};
	}
}