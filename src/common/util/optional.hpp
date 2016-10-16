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

#include "common/i_packet.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/types.hpp"

namespace vana {
	namespace util {
		// Sorta based on Boost's
		template <typename TElement>
		class optional {
		public:
			optional() = default;
			optional(const optional<TElement> &r) :
				m_initialized{r.m_initialized}
			{
				create(r.get());
			}
			optional(optional<TElement> &&r) :
				m_initialized{r.m_initialized}
			{
				if (r.is_initialized()) {
					create(r.get()); 
					r.destroy();
				}
			}
			optional(const TElement &val) :
				m_initialized{true}
			{
				create(val);
			}
			~optional() { destroy(); }
			auto is_initialized() const -> bool { return m_initialized; }
			auto get() const -> const TElement & { return get_casted_storage(); }
			auto get() -> TElement & { return get_casted_storage(); }
			auto get(const TElement &default_value) const -> const TElement & { return m_initialized ? get_casted_storage() : default_value; }
			auto get(const TElement &default_value) -> TElement { return m_initialized ? get_casted_storage() : default_value; }
			auto reset() -> void { destroy(); }
			auto operator =(TElement val) -> optional<TElement> & {
				destroy();
				m_initialized = true;
				create(val);
				return *this;
			}
			auto operator =(const optional<TElement> &r) -> optional<TElement> & {
				destroy();
				if (r.is_initialized()) {
					m_initialized = true;
					create(r.get());
				}
				return *this;
			}
		private:
			void destroy() {
				if (m_initialized) {
					get_casted_storage().~TElement();
					m_initialized = false;
				}
			}
			void create(const TElement &value) {
				new (get_storage()) TElement{value};
			}
			void * get_storage() { return m_data; }
			const void * get_storage() const { return m_data; }
			TElement & get_casted_storage() { return *static_cast<TElement *>(get_storage()); }
			const TElement & get_casted_storage() const { return *static_cast<const TElement *>(get_storage()); };

			bool m_initialized = false;
			char m_data[sizeof(TElement)];
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
		template <typename TElement>
		auto operator !=(const TElement &lhs, const optional<TElement> &rhs) -> bool {
			return !(rhs == lhs);
		}
	}

	template <typename TElement>
	struct packet_serialize<vana::util::optional<TElement>> {
		auto read(packet_reader &reader) -> vana::util::optional<TElement> {
			vana::util::optional<TElement> ret;
			if (reader.get<bool>()) {
				ret = reader.get<TElement>();
			}
			return ret;
		};
		auto write(packet_builder &builder, const vana::util::optional<TElement> &obj) -> void {
			bool not_null = obj.is_initialized();
			builder.add<bool>(not_null);
			if (not_null) {
				builder.add<TElement>(obj.get());
			}
		};
	};

	// Useful DB-related aliases
	template <typename TElement>
	using optional = vana::util::optional<TElement>;
	using opt_bool = optional<bool>;
	using opt_int8_t = optional<int8_t>;
	using opt_uint8_t = optional<uint8_t>;
	using opt_int16_t = optional<int16_t>;
	using opt_uint16_t = optional<uint16_t>;
	using opt_int32_t = optional<int32_t>;
	using opt_uint32_t = optional<uint32_t>;
	using opt_int64_t = optional<int64_t>;
	using opt_uint64_t = optional<uint64_t>;
	using opt_double = optional<double>;
	using opt_string = optional<string>;
}