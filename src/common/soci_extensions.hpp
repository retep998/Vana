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
#include "common/unix_time.hpp"
#include "common/util/optional.hpp"
#include <soci.h>
#include <string>

namespace soci {
	using unix_time = vana::unix_time;
	using string = vana::string;
	using int8_t = vana::int8_t;
	using int16_t = vana::int16_t;
	using int32_t = vana::int32_t;
	using int64_t = vana::int64_t;
	using uint8_t = vana::uint8_t;
	using uint16_t = vana::uint16_t;
	using uint32_t = vana::uint32_t;
	using uint64_t = vana::uint64_t;
	template <typename TElement>
	using optional = vana::optional<TElement>;

	template <>
	struct type_conversion<unix_time> {
		using base_type = std::tm;
		using target_type = unix_time;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = unix_time{0};
			}
			else {
				out = mktime(&const_cast<std::tm &>(in));
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			time_t conv = in;
			out = *localtime(&conv);
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<string> {
		using base_type = string;
		using target_type = string;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = "";
			}
			else {
				out = in;
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<bool> {
		using base_type = int;
		using target_type = bool;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			out = (ind == i_ok && in == 1);
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = (in ? 1 : 0);
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<int8_t> {
		using base_type = int;
		using target_type = int8_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				const base_type max = (std::numeric_limits<target_type>::max)();
				const base_type min = (std::numeric_limits<target_type>::min)();
				if (in < min || in >  max) {
					throw soci_error{"Value outside of allowed range"};
				}

				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

#ifdef WIN32
	// These conflict with the default SOCI lib, which we took care of for Windows
	// The defaults were also fixed in the SOCI code repo, but it's unclear when there will be a real release
	// Ergo, Linux may not run at all, but at least it compiles..?

	template <>
	struct type_conversion<uint8_t> {
		using base_type = int;
		using target_type = uint8_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				const base_type max = (std::numeric_limits<target_type>::max)();
				const base_type min = (std::numeric_limits<target_type>::min)();
				if (in < min || in >  max) {
					throw soci_error{"Value outside of allowed range"};
				}

				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<uint16_t> {
		using base_type = int;
		using target_type = uint16_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				const base_type max = (std::numeric_limits<target_type>::max)();
				const base_type min = (std::numeric_limits<target_type>::min)();
				if (in < min || in >  max) {
					throw soci_error{"Value outside of allowed range"};
				}

				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<uint32_t> {
		using base_type = int;
		using target_type = uint32_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};
#endif

	template <>
	struct type_conversion<int16_t> {
		using base_type = int;
		using target_type = int16_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				const base_type max = (std::numeric_limits<target_type>::max)();
				const base_type min = (std::numeric_limits<target_type>::min)();
				if (in < min || in >  max) {
					throw soci_error{"Value outside of allowed range"};
				}

				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<int32_t> {
		using base_type = int;
		using target_type = int32_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<int64_t> {
		using base_type = long long;
		using target_type = int64_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<uint64_t> {
		using base_type = long long;
		using target_type = uint64_t;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <>
	struct type_conversion<double> {
		using base_type = double;
		using target_type = double;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0.0;
			}
			else {
				out = in;
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};

	template <typename T>
	struct type_conversion<optional<T>> {
		using base_type = typename type_conversion<T>::base_type;

		static void from_base(const base_type &in, indicator ind, optional<T> &out) {
			if (ind == i_null) {
				out.reset();
			}
			else {
				T tmp;
				type_conversion<T>::from_base(in, ind, tmp);
				out = tmp;
			}
		}

		static void to_base(const optional<T> &in, base_type &out, indicator &ind) {
			if (in.is_initialized()) {
				type_conversion<T>::to_base(in.get(), out, ind);
			}
			else {
				ind = i_null;
			}
		}
	};
}