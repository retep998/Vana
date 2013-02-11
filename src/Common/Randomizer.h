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

#include "Types.h"
#include <limits>
#include <random>

using std::mt19937;
using std::uniform_int_distribution;

class Randomizer {
public:
	template <typename TInteger>
	static TInteger rand() {
		return rand(std::numeric_limits<TInteger>::max(), std::numeric_limits<TInteger>::min());
	}

	template <typename TInteger>
	static TInteger rand(TInteger max, TInteger min = 0) {
		return s_rand.rand<TInteger>(max, min);
	}

private:
	class _impl {
	public:
		_impl() {
			m_engine.seed(std::rand());
			m_distribution = uniform_int_distribution<uint64_t>(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
		}

		template <typename TInteger>
		TInteger rand(TInteger max, TInteger min = 0) {
			TInteger diff = (max - min) + 1;
			uint64_t result = m_distribution(m_engine);
			if (diff != 0) result = (result % diff) + min;
			return static_cast<TInteger>(result);
		}
	private:
		mt19937 m_engine;
		uniform_int_distribution<uint64_t> m_distribution;
	};

	static _impl s_rand;
};