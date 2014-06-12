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

#include "CaseInsensitiveEquals.hpp"
#include "CaseInsensitiveHash.hpp"
#include "make_unique.hpp"
#include "optional.hpp"
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
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Thin wrapper around time_t so we could write a SOCI extension for it
// time_t on its own is defined as __int64_t on MSVC which was conflicting with the SOCI extension for int64_t
class unix_time_t {
public:
	unix_time_t() { m_time = time(nullptr); }
	unix_time_t(time_t t) { m_time = t; }
	unix_time_t(const unix_time_t &t) { m_time = t.m_time; }
	operator time_t() const { return m_time; }
	auto operator =(const time_t &t) -> unix_time_t & { m_time = t; return *this; }
	auto operator =(const unix_time_t &right) -> unix_time_t & { m_time = right.m_time; return *this; }
	auto operator +=(const time_t &t) -> unix_time_t & { m_time += t; return *this; }
	auto operator +=(const unix_time_t &right) -> unix_time_t & { m_time += right.m_time; return *this; }
	auto operator -=(const time_t &t) -> unix_time_t & { m_time -= t; return *this; }
	auto operator -=(const unix_time_t &right) -> unix_time_t & { m_time -= right.m_time; return *this; }
	auto operator +(const time_t &t) const -> unix_time_t { return unix_time_t(m_time + t); }
	auto operator +(const unix_time_t &right) const -> unix_time_t { return unix_time_t(m_time + right.m_time); }
	auto operator -(const time_t &t) const -> unix_time_t { return unix_time_t(m_time - t); }
	auto operator -(const unix_time_t &right) const -> unix_time_t { return unix_time_t(m_time - right.m_time); }
private:
	time_t m_time;
};

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
	StopIterating
};

enum class CompletionResult {
	Complete,
	Incomplete,
};

enum class Result {
	Successful,
	Failure
};

enum class HackingResult {
	DefinitelyHacking,
	MaybeHacking,
	NotHacking,
};

using std::chrono::duration_cast;

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
using locale_t = uint8_t;
using player_level_t = uint8_t;
using skill_level_t = uint8_t;
using fame_t = int16_t;
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

// Miscellaneous utility types
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
template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
using hash_map_t = std::unordered_map<TKey, TElement, THash, TOperation>;
template <typename TKey, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
using hash_set_t = std::unordered_set<TKey, THash, TOperation>;
template <typename TElement>
using case_insensitive_hash_map_t = std::unordered_map<string_t, TElement, CaseInsensitiveHash, CaseInsensitiveEquals>;
using case_insensitive_hash_set_t = std::unordered_set<string_t, CaseInsensitiveHash, CaseInsensitiveEquals>;
template <typename TKey, typename TElement, typename TOrdering = std::less<TKey>>
using ord_map_t = std::map<TKey, TElement, TOrdering>;
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
using opt_unix_time_t = optional_t<unix_time_t>;
using opt_string_t = optional_t<string_t>;

// Remove when VS2012 supports thread_local
#ifdef WIN32
#define thread_local __declspec(thread)
#else
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
		explicit TypeName(const std::string &message) : BaseType(message) { } \
		explicit TypeName(const char *message) : BaseType(message) { } \
	};