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

#include "optional.hpp"
#include "soci.h"
#include "Types.h"
#include <string>

namespace soci {
	template <>
	struct type_conversion<unix_time_t> {
		typedef std::tm base_type;
		typedef unix_time_t target_type;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = time(nullptr);
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
	struct type_conversion<std::string> {
		typedef std::string base_type;
		typedef std::string target_type;

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
		typedef int base_type;
		typedef bool target_type;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			out = (ind == i_ok && in == 1);
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = (in ? 1 : 0);
			ind = i_ok;
		}
	};
	/*
	template <>
	struct type_conversion<uint8_t> {
		typedef int base_type;
		typedef uint8_t target_type;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				const base_type max = (std::numeric_limits<target_type>::max)();
				const base_type min = (std::numeric_limits<target_type>::min)();
				if (in < min || in >  max) {
					throw soci_error("Value outside of allowed range");
				}

				out = static_cast<target_type>(in);
			}
		}

		static void to_base(const target_type &in, base_type &out, indicator &ind) {
			out = in;
			ind = i_ok;
		}
	};
	*/
	template <>
	struct type_conversion<int8_t> {
		typedef int base_type;
		typedef int8_t target_type;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				const base_type max = (std::numeric_limits<target_type>::max)();
				const base_type min = (std::numeric_limits<target_type>::min)();
				if (in < min || in >  max) {
					throw soci_error("Value outside of allowed range");
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
	struct type_conversion<int16_t> {
		typedef int base_type;
		typedef int16_t target_type;

		static void from_base(const base_type &in, indicator &ind, target_type &out) {
			if (ind == i_null) {
				out = 0;
			}
			else {
				const base_type max = (std::numeric_limits<target_type>::max)();
				const base_type min = (std::numeric_limits<target_type>::min)();
				if (in < min || in >  max) {
					throw soci_error("Value outside of allowed range");
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
		typedef int base_type;
		typedef int32_t target_type;

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
		typedef long long base_type;
		typedef int64_t target_type;

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
		typedef long long base_type;
		typedef uint64_t target_type;

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
		typedef double base_type;
		typedef double target_type;

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
	struct type_conversion<std::optional<T>> {
		typedef typename type_conversion<T>::base_type base_type;

		static void from_base(const base_type &in, indicator ind, std::optional<T> &out) {
			if (ind == i_null) {
				out.reset();
			}
			else {
				T tmp;
				type_conversion<T>::from_base(in, ind, tmp);
				out = tmp;
			}
		}

		static void to_base(const std::optional<T> &in, base_type &out, indicator &ind) {
			if (in.is_initialized()) {
				type_conversion<T>::to_base(in.get(), out, ind);
			}
			else {
				ind = i_null;
			}
		}
	};
}