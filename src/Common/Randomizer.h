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

#include "Types.h"
#include <algorithm>
#include <iterator>
#include <limits>
#include <random>
#include <type_traits>

class Randomizer {
public:
	template <typename TNumber>
	static auto rand() -> TNumber {
		return rand(std::numeric_limits<TNumber>::max(), std::numeric_limits<TNumber>::min());
	}

	template <typename TNumber>
	static auto rand(TNumber max, TNumber min = 0) -> TNumber {
		return s_rand.rand(max, min);
	}

	template <typename TContainer>
	static auto shuffle(TContainer &c) -> void {
		shuffle(std::begin(c), std::end(c));
	}

	template <typename TIterator>
	static auto shuffle(TIterator begin, TIterator end) -> void {
		std::shuffle(begin, end, s_rand.engine());
	}

	template <typename TContainer>
	static auto select(const TContainer &c) -> decltype(std::cbegin(c)) {
		return select(std::cbegin(c), std::cend(c));
	}

	template <typename TIterator>
	static auto select(TIterator begin, TIterator end) -> TIterator {
		auto distance = rand(std::distance(begin, end) - 1);
		TIterator element = begin;
		std::advance(element, distance);
		return element;
	}
private:
	class _impl {
	public:
		_impl() {
			std::random_device seedingEngine;
			m_engine.seed(seedingEngine());
		}

		template <typename TNumber>
		auto rand(TNumber max, TNumber min = 0) -> TNumber {
			static_assert(std::is_integral<TNumber>::value || std::is_floating_point<TNumber>::value, "TNumber must be integral or floating point");

			TNumber result;
			if (std::is_integral<TNumber>::value) {
				if (std::is_unsigned<TNumber>::value) {
					std::uniform_int_distribution<uint64_t> distribution(static_cast<uint64_t>(min), static_cast<uint64_t>(max));
					result = static_cast<TNumber>(distribution(m_engine));
				}
				else {
					std::uniform_int_distribution<int64_t> distribution(static_cast<int64_t>(min), static_cast<int64_t>(max));
					result = static_cast<TNumber>(distribution(m_engine));
				}
			}
			else {
				std::uniform_real_distribution<long double> distribution(static_cast<long double>(min), static_cast<long double>(max));
				return static_cast<TNumber>(distribution(m_engine));
			}
			return result;
		}

		auto engine() -> std::mt19937 & {
			return m_engine;
		}
	private:
		std::mt19937 m_engine;
	};

	static _impl s_rand;
};