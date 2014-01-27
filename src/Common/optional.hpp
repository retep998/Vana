/*
Copyright (C) 2008-2014 Vana Development Team

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
	template <typename TElement>
	class optional {
	public:
		optional() = default;
		optional(const TElement &val) : m_initialized(true), m_val(val) {}
		auto is_initialized() const -> bool { return m_initialized; }
		auto get() const -> const TElement & { return m_val; }
		auto get() -> TElement & { return m_val; }
		auto getOrDefault(const TElement &def) -> TElement { return m_initialized ? m_val : def; }
		auto reset() -> void { m_initialized = false; }
		auto operator =(TElement val) -> optional<TElement> & { m_initialized = true; m_val = val; return *this; }
		auto operator =(const optional<TElement> &r) -> optional<TElement> & { m_initialized = r.m_initialized; m_val = r.m_val; return *this; }
	private:
		bool m_initialized = false;
		TElement m_val;
	};
}