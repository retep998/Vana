/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "IPacket.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Types.hpp"

namespace Vana {
	namespace MiscUtilities {
		// Sorta based on Boost's
		template <typename TElement>
		class optional {
		public:
			optional() = default;
			optional(const optional<TElement> &r) : m_initialized{r.m_initialized}, m_val{r.m_val} { }
			optional(optional<TElement> &&r) : m_initialized{r.m_initialized}, m_val{r.m_val} {
				r.m_initialized = false;
				r.m_val = {};
			}
			optional(const TElement &val) : m_initialized{true}, m_val{val} { }
			auto is_initialized() const -> bool { return m_initialized; }
			auto get() const -> const TElement & { return m_val; }
			auto get() -> TElement & { return m_val; }
			auto get(const TElement &defaultValue) const -> const TElement & { return m_initialized ? m_val : defaultValue; }
			auto get(const TElement &defaultValue) -> TElement { return m_initialized ? m_val : defaultValue; }
			auto reset() -> void { m_initialized = false; m_val = TElement{}; }
			auto operator =(TElement val) -> optional<TElement> & { m_initialized = true; m_val = val; return *this; }
			auto operator =(const optional<TElement> &r) -> optional<TElement> & { m_initialized = r.m_initialized; m_val = r.m_val; return *this; }
		private:
			bool m_initialized = false;
			TElement m_val = {};
		};

		template <typename TElement>
		auto operator ==(const optional<TElement> &lhs, const optional<TElement> &rhs) -> bool {
			if (lhs.is_initialized() != rhs.is_initialized()) {
				return false;
			}
			if (!lhs.is_initialized()) {
				return true;
			}
			return lhs.get() == rhs.get();
		}
		template <typename TElement>
		auto operator ==(const optional<TElement> &lhs, const TElement &rhs) -> bool {
			if (!lhs.is_initialized()) {
				return false;
			}
			return lhs.get() == rhs;
		}
		template <typename TElement>
		auto operator ==(const TElement &lhs, const optional<TElement> &rhs) -> bool {
			return rhs == lhs;
		}
		template <typename TElement>
		auto operator !=(const optional<TElement> &lhs, const optional<TElement> &rhs) -> bool {
			return !(lhs == rhs);
		}
		template <typename TElement>
		auto operator !=(const optional<TElement> &lhs, const TElement &rhs) -> bool {
			return !(lhs == rhs);
		}
	}

	template <typename TElement>
	struct PacketSerialize<MiscUtilities::optional<TElement>> {
		auto read(PacketReader &reader) -> MiscUtilities::optional<TElement> {
			MiscUtilities::optional<TElement> ret;
			if (reader.get<bool>()) {
				ret = reader.get<TElement>();
			}
			return ret;
		};
		auto write(PacketBuilder &builder, const MiscUtilities::optional<TElement> &obj) -> void {
			bool notNull = obj.is_initialized();
			builder.add<bool>(notNull);
			if (notNull) {
				builder.add<TElement>(obj.get());
			}
		};
	};

	// Useful DB-related aliases
	template <typename TElement>
	using optional_t = MiscUtilities::optional<TElement>;
	using opt_bool = optional_t<bool>;
	using opt_int8_t = optional_t<int8_t>;
	using opt_uint8_t = optional_t<uint8_t>;
	using opt_int16_t = optional_t<int16_t>;
	using opt_uint16_t = optional_t<uint16_t>;
	using opt_int32_t = optional_t<int32_t>;
	using opt_uint32_t = optional_t<uint32_t>;
	using opt_int64_t = optional_t<int64_t>;
	using opt_uint64_t = optional_t<uint64_t>;
	using opt_double = optional_t<double>;
	using opt_string_t = optional_t<string_t>;
}