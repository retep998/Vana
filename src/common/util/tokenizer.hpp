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

#include <string>
#include <vector>

namespace vana {
	namespace util {
		// Sorta based on Boost's, not really
		class tokenizer {
			using container = std::vector<std::string>;

		public:
			tokenizer(const std::string &content, const std::string &sep) {
				size_t start = 0;
				size_t end = 0;
				while (end != std::string::npos) {
					end = content.find(sep, start);
					m_tokens.push_back(content.substr(start, (end == std::string::npos) ? end : end - start));
					start = (end > (std::string::npos - sep.size()) ? std::string::npos : end + sep.size());
				}
			}
			using iterator = container::iterator;
			using const_iterator = container::const_iterator;
			using reverse_iterator = container::reverse_iterator;
			using const_reverse_iterator = container::const_reverse_iterator;
			auto begin() -> iterator { return std::begin(m_tokens); }
			auto end() -> iterator { return std::end(m_tokens); }
			auto cbegin() const -> const_iterator { return std::cbegin(m_tokens); }
			auto cend() const -> const_iterator { return std::cend(m_tokens); }
			auto rbegin() -> reverse_iterator { return std::rbegin(m_tokens); }
			auto rend() -> reverse_iterator { return std::rend(m_tokens); }
			auto crbegin() const -> const_reverse_iterator { return std::crbegin(m_tokens); }
			auto crend() const -> const_reverse_iterator { return std::crend(m_tokens); }
		private:
			container m_tokens;
		};
	}
}