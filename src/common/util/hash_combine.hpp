// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  Based on Peter Dimov's proposal
//  http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
//  issue 6.18.
#pragma once

#include <functional>

namespace vana {
	namespace util {
		template <typename TValue>
		inline
		size_t hash_combine(size_t seed, const TValue &value) {
			std::hash<TValue> hasher;
			return seed ^ (hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
		}

		inline
		size_t hash_combinator() {
			return 0;
		}

		template <typename THead, typename ... TTail>
		inline
		size_t hash_combinator(const THead &value, const TTail & ... rest) {
			return hash_combine(hash_combinator(rest...), value);
		}
	}
}