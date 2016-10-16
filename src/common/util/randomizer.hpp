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
#include <algorithm>
#include <iterator>
#include <limits>
#include <random>
#include <type_traits>

namespace vana {
	namespace util {
		class randomizer {
		public:
			template <typename TNumber>
			static auto rand() -> TNumber {
				return rand(std::numeric_limits<TNumber>::max(), std::numeric_limits<TNumber>::min());
			}

			template <>
			static auto rand<bool>() -> bool {
				return (rand<uint8_t>() & 1) == 1;
			}

			template <typename TDistribution>
			static auto rand(TDistribution &dist) -> typename TDistribution::result_type {
				return dist(s_rand.engine());
			}

			template <typename TNumber>
			static auto range(TNumber base, TNumber modifier) -> std::enable_if_t<std::is_integral<TNumber>::value, TNumber> {
				TNumber min = base - (modifier / 2);
				TNumber max = base + (modifier / 2);
				return s_rand.rand(max, min);
			}

			template <typename TNumber>
			static auto range(TNumber base, TNumber modifier) -> std::enable_if_t<std::is_floating_point<TNumber>::value, TNumber> {
				TNumber min = base - (modifier / 2);
				TNumber max = base + (modifier / 2);
				return s_rand.rand(max, min);
			}

			static auto percentage() -> int32_t {
				return percentage<int32_t>();
			}

			template <typename TNumber>
			static auto percentage() -> std::enable_if_t<std::is_integral<TNumber>::value, TNumber> {
				return s_rand.rand(99, 0);
			}

			template <typename TNumber>
			static auto rand(TNumber max, TNumber min = 0) -> std::enable_if_t<std::is_integral<TNumber>::value, TNumber> {
				return s_rand.rand(max, min);
			}

			template <typename TNumber>
			static auto rand(TNumber max, TNumber min = 0) -> std::enable_if_t<std::is_floating_point<TNumber>::value, TNumber> {
				return s_rand.rand(max, min);
			}

			template <typename TNumber>
			static auto twofold(TNumber min) -> std::enable_if_t<std::is_integral<TNumber>::value, TNumber> {
				return min + s_rand.rand(min, 0);
			}

			template <typename TNumber>
			static auto twofold(TNumber min) -> std::enable_if_t<std::is_floating_point<TNumber>::value, TNumber> {
				return min + s_rand.rand(min, 0);
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

			template <typename TContainer>
			static auto select(const TContainer *c) -> decltype(select(*c)) {
				return select(*c);
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
					std::random_device seeding_engine;
					m_engine.seed(seeding_engine());
				}

				template <typename TNumber>
				auto rand(TNumber max, TNumber min) -> std::enable_if_t<std::is_integral<TNumber>::value, TNumber> {
					std::uniform_int_distribution<TNumber> distribution{min, max};
					return distribution(m_engine);
				}

				// TODO FIXME c++
				// If C++ ever gets an actual small integer type, dump these specializations
				template <>
				auto rand<int8_t>(int8_t max, int8_t min) -> int8_t {
					return static_cast<int8_t>(rand<int16_t>(max, min));
				}

				template <>
				auto rand<uint8_t>(uint8_t max, uint8_t min) -> uint8_t {
					return static_cast<uint8_t>(rand<uint16_t>(max, min));
				}

				template <typename TNumber>
				auto rand(TNumber max, TNumber min) -> std::enable_if_t<std::is_floating_point<TNumber>::value, TNumber> {
					std::uniform_real_distribution<TNumber> distribution{min, max};
					return distribution(m_engine);
				}

				auto engine() -> std::mt19937 & {
					return m_engine;
				}
			private:
				std::mt19937 m_engine;
			};

			static _impl s_rand;
		};
	}
}