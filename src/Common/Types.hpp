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

#include "CaseInsensitiveEquals.hpp"
#include "CaseInsensitiveHash.hpp"
#include "make_unique.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef WIN32
#define thread_local __thread
#endif

// Macros that expand common patterns to avoid inheriting classes and reduce boilerplate
#define NO_DEFAULT_CONSTRUCTOR(TypeName) \
	public: \
		TypeName() = delete;

#define NONCOPYABLE(TypeName) \
	public: \
		TypeName(const TypeName &) = delete; \
		auto operator=(const TypeName &) -> TypeName & = delete;

#define NONMOVABLE(TypeName) \
	public: \
		TypeName(TypeName &&) = delete; \
		auto operator=(TypeName &&) -> TypeName & = delete;

#define SINGLETON(TypeName) \
	public: \
		static auto getInstance() -> TypeName & { \
			static TypeName singleton; \
			return singleton; \
		} \
	NONCOPYABLE(TypeName); \
	private: \
		TypeName();

#define DEFAULT_EXCEPTION(TypeName, BaseType) \
	struct TypeName : public BaseType { \
		explicit TypeName(const std::string &message) : BaseType{message.c_str()} { } \
		explicit TypeName(const char *message = nullptr) : BaseType{message} { } \
	};

namespace Vana {
	enum class HandleResult {
		Handled,
		Unhandled,
	};

	enum class SearchResult {
		Found,
		NotFound,
	};

	enum class MultiMatchResult {
		NoMatches,
		OneMatch,
		MultipleMatches,
	};

	enum class MatchResult {
		NoMatches,
		AnyMatches,
	};

	enum class IterationResult {
		ContinueIterating,
		StopIterating,
	};

	enum class CompletionResult {
		Complete,
		Incomplete,
	};

	enum class Result {
		Successful,
		Failure,
	};

	enum class ValidityResult {
		Valid,
		Invalid,
	};

	enum class HackingResult {
		DefinitelyHacking,
		MaybeHacking,
		NotHacking,
	};

	enum class MysticDoorResult {
		Success,
		NoDoorPoints,
		NoSpace,
		Hacking,
	};

	using std::chrono::duration_cast;

	// Miscellaneous utility types
	using int8_t = std::int8_t;
	using int16_t = std::int16_t;
	using int32_t = std::int32_t;
	using int64_t = std::int64_t;
	using uint8_t = std::uint8_t;
	using uint16_t = std::uint16_t;
	using uint32_t = std::uint32_t;
	using uint64_t = std::uint64_t;
	using effective_clock_t = std::chrono::system_clock;
	using duration_t = effective_clock_t::duration;
	using time_point_t = effective_clock_t::time_point;
	using nanoseconds_t = std::chrono::nanoseconds;
	using microseconds_t = std::chrono::microseconds;
	using milliseconds_t = std::chrono::milliseconds;
	using seconds_t = std::chrono::seconds;
	using minutes_t = std::chrono::minutes;
	using hours_t = std::chrono::hours;
	using string_t = std::string;
	using out_stream_t = std::ostringstream;
	using thread_t = std::thread;
	using mutex_t = std::mutex;
	using recursive_mutex_t = std::recursive_mutex;
	using match_t = std::smatch;

	template <typename TElement>
	using vector_t = std::vector<TElement>;
	template <typename TElement>
	using init_list_t = std::initializer_list<TElement>;
	template <typename TFirst, typename TSecond>
	using pair_t = std::pair<TFirst, TSecond>;
	template <typename ... TArgs>
	using tuple_t = std::tuple<TArgs...>;
	template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
	using hash_map_t = std::unordered_map<TKey, TElement, THash, TOperation>;
	template <typename TKey, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
	using hash_set_t = std::unordered_set<TKey, THash, TOperation>;
	template <typename TElement, typename TString = string_t>
	using case_insensitive_hash_map_t = std::unordered_map<TString, TElement, CaseInsensitiveHash, CaseInsensitiveEquals>;
	template <typename TString = string_t>
	using case_insensitive_hash_set_t = std::unordered_set<TString, CaseInsensitiveHash, CaseInsensitiveEquals>;
	template <typename TKey, typename TElement, typename TOrdering = std::less<TKey>>
	using ord_map_t = std::map<TKey, TElement, TOrdering>;
	template <typename TKey, typename TOrdering = std::less<TKey>>
	using ord_set_t = std::set<TKey, TOrdering>;
	template <typename TFunc>
	using function_t = std::function<TFunc>;
	template <typename TPtr>
	using owned_ptr_t = std::unique_ptr<TPtr>;
	template <typename TPtr>
	using ref_ptr_t = std::shared_ptr<TPtr>;
	template <typename TPtr>
	using view_ptr_t = std::weak_ptr<TPtr>;
	template <typename TMutex>
	using owned_lock_t = std::unique_lock<TMutex>;
	template <typename TElement, size_t Size>
	using array_t = std::array<TElement, Size>;
	template <typename TElement>
	using queue_t = std::deque<TElement>;

	template <typename TPtr>
	using enable_shared = std::enable_shared_from_this<TPtr>;

	template <typename TSrc, typename ...TArgs>
	inline
	auto make_ref_ptr(TArgs && ...args) -> ref_ptr_t<TSrc> {
		return std::make_shared<TSrc>(args...);
	}

	template <typename TSrc, typename ...TArgs>
	inline
	auto make_owned_ptr(TArgs && ...args) -> owned_ptr_t<TSrc> {
		return std::make_unique<TSrc>(args...);
	}

	// Game protocol/entity types
	using world_id_t = int8_t;
	using channel_id_t = int8_t;
	using item_id_t = int32_t;
	using map_id_t = int32_t;
	using mob_id_t = int32_t;
	using npc_id_t = int32_t;
	using shop_id_t = int32_t;
	using job_id_t = int16_t;
	using reactor_id_t = int32_t;
	using party_id_t = int32_t;
	using skill_id_t = int32_t;
	using summon_id_t = int32_t;
	using mist_id_t = int32_t;
	using mob_skill_id_t = uint8_t;
	using pet_id_t = int64_t;
	using player_id_t = int32_t;
	using foothold_id_t = int16_t;
	using seat_id_t = int16_t;
	using trade_id_t = int32_t;
	using trade_slot_t = uint8_t;
	using morph_id_t = int16_t;
	using quest_id_t = uint16_t;
	using portal_id_t = int8_t;
	using account_id_t = int32_t;
	using skin_id_t = int8_t;
	using hair_id_t = int32_t;
	using face_id_t = int32_t;
	using gender_id_t = int8_t;

	using header_t = uint16_t;
	using port_t = uint16_t;
	using version_t = uint16_t;
	using game_locale_t = uint8_t;
	using chat_t = string_t;
	using chat_stream_t = out_stream_t;
	using player_level_t = uint8_t;
	using skill_level_t = uint8_t;
	using fame_t = int16_t;
	using stat_t = int16_t;
	using health_t = int16_t;
	using health_ap_t = uint16_t;
	using mob_skill_level_t = uint8_t;
	using inventory_t = int8_t;
	using inventory_slot_t = int16_t;
	using inventory_slot_count_t = uint8_t;
	using slot_qty_t = int16_t;
	using storage_slot_t = uint8_t;
	using damage_t = int32_t;
	using mesos_t = int32_t;
	using experience_t = int32_t;
	using map_object_t = int32_t;
	using coord_t = int16_t;
	using tick_count_t = uint32_t;
	using charge_time_t = int32_t;
	using portal_count_t = uint8_t;
	using checksum_t = uint32_t;

	DEFAULT_EXCEPTION(NotImplementedException, std::exception);
	DEFAULT_EXCEPTION(InvalidOperationException, std::exception);
}