/*
Copyright (C) 2008-2013 Vana Development Team

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

namespace MiscUtilities {
	// Sorta based on Boost's
	template<typename T>
	class optional {
	public:
		optional() : m_initialized(false) { }
		optional(const T &val) : m_initialized(true), m_val(val) { }
		bool is_initialized() const { return m_initialized; }
		const T & get() const { return m_val; }
		T & get() { return m_val; }
		void reset() { m_initialized = false; }
		optional<T> & operator =(T val) { m_initialized = true; m_val = val; return *this; }
		optional<T> & operator =(const optional<T> &r) { m_initialized = r.m_initialized; m_val = r.m_val; return *this; }
	private:
		bool m_initialized;
		T m_val;
	};
}