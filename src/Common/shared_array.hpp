/*
Copyright (C) 2008-2012 Vana Development Team

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

namespace std {
	// Sorta based on Boost's, not really
	template<typename T>
	class shared_array {
	private:
		template<typename U>
		struct array_deleter {
			void operator()(U *p) {
				delete[] p;
			}
		};

		std::shared_ptr<T> m_ptr;
	public:
		explicit shared_array(T *p = nullptr): m_ptr(p, array_deleter<T>()) { }
		shared_array(const shared_array &r): m_ptr(r.m_ptr) { }

		void reset(T *p = nullptr) { m_ptr.reset(p); }
		T & operator[] (std::ptrdiff_t i) const { return m_ptr.get()[i]; }
		T * get() const { return m_ptr.get(); }
		operator bool() const { return m_ptr.get() != nullptr; }
	};
}