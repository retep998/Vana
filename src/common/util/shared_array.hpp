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

#include <memory>

namespace vana {
	namespace util {
		// Sorta based on Boost's, not really
		template <typename TElement>
		class shared_array {
		private:
			template <typename TPointer>
			struct array_deleter {
				auto operator()(TPointer *p) -> void {
					delete[] p;
				}
			};

			std::shared_ptr<TElement> m_ptr;
		public:
			explicit shared_array(TElement *p = nullptr): m_ptr{p, array_deleter<TElement>{}} { }
			shared_array(const shared_array &r): m_ptr{r.m_ptr} { }

			auto reset(TElement *p = nullptr) -> void { m_ptr.reset(p); }
			auto operator[](std::ptrdiff_t i) const -> TElement & { return m_ptr.get()[i]; }
			auto get() const -> TElement * { return m_ptr.get(); }
			operator bool() const { return m_ptr.get() != nullptr; }
		};
	}
}