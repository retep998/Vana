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

#include "common/make_unique.hpp"
#include "common/preprocessor.hpp"
#include "common/util/case_insensitive_equals.hpp"
#include "common/util/case_insensitive_hash.hpp"
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
		static auto get_instance() -> TypeName & { \
			static TypeName s_singleton; \
			return s_singleton; \
		} \
	NONCOPYABLE(TypeName); \
	private: \
		TypeName();

#define DEFAULT_EXCEPTION(TypeName, BaseType) \
	struct TypeName : public BaseType { \
		explicit TypeName(const std::string &message) : BaseType{message.c_str()} { } \
		explicit TypeName(const char *message = nullptr) : BaseType{message} { } \
	};

#define CODE_EXCEPTION(TypeName, BaseType) \
	struct TypeName : public BaseType { \
		int get_line() const { return m_line; } \
		const char *get_file() const { return m_file; } \
		TypeName(const char *file, int line, const std::string &message) : \
			BaseType{message.c_str()}, \
			m_file{file}, \
			m_line{line} \
		{ \
		} \
		TypeName(const char *file, int line, const char *message = nullptr) : \
			BaseType{message}, \
			m_file{file}, \
			m_line{line} \
		{ \
		} \
		private: \
			int m_line; \
			const char *m_file; \
	};

#define THROW_CODE_EXCEPTION_IMPL_2(TypeName, Message) \
	throw TypeName{__FILE__, __LINE__, Message};

#define THROW_CODE_EXCEPTION_IMPL_1(TypeName) \
	throw TypeName{__FILE__, __LINE__};

#define THROW_CODE_EXCEPTION(...) \
	DISPATCH(THROW_CODE_EXCEPTION_IMPL_, __VA_ARGS__)

namespace vana {
	enum class handle_result {
		handled,
		unhandled,
	};

	enum class search_result {
		found,
		not_found,
	};

	enum class multi_match_result {
		no_matches,
		one_match,
		multiple_matches,
	};

	enum class match_result {
		no_matches,
		any_matches,
	};

	enum class iteration_result {
		continue_iterating,
		stop_iterating,
	};

	enum class completion_result {
		complete,
		incomplete,
	};

	enum class result {
		success,
		failure,
	};

	enum class stack_result {
		full,
		partial,
		none,
	};

	enum class validity_result {
		valid,
		invalid,
	};

	enum class hacking_result {
		definitely_hacking,
		maybe_hacking,
		not_hacking,
	};

	enum class mystic_door_result {
		success,
		no_door_points,
		no_space,
		hacking,
	};

	enum class mob_elemental_attribute : std::int8_t {
		normal,
		immune,
		strong,
		weak,
	};

	enum class mob_control_status : std::int8_t {
		normal = 1,
		none = 5,
	};

	enum class mob_spawn_type {
		spawn,
		existing,
	};

	enum class job_type : std::int8_t {
		adventurer = 0,
		cygnus = 1,
		legend = 2,
		resistance = 3,
		nova = 6,
	};

	enum class stat_variance {
		none,
		normal,
		only_increase_with_great_chance,
		only_increase_with_amazing_chance,
		gachapon,
		chaos_normal,
		chaos_high,
	};

	struct packet_date {
		int16_t year;
		int8_t month;
		int8_t day;
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
	using effective_clock = std::chrono::system_clock;
	using duration = effective_clock::duration;
	using time_point = effective_clock::time_point;
	using nanoseconds = std::chrono::nanoseconds;
	using microseconds = std::chrono::microseconds;
	using milliseconds = std::chrono::milliseconds;
	using seconds = std::chrono::seconds;
	using minutes = std::chrono::minutes;
	using hours = std::chrono::hours;
	using string = std::string;
	using out_stream = std::ostringstream;
	using mutex = std::mutex;
	using recursive_mutex = std::recursive_mutex;
	using match = std::smatch;

	template <typename TElement>
	using vector = std::vector<TElement>;
	template <typename TElement>
	using init_list = std::initializer_list<TElement>;
	template <typename TFirst, typename TSecond>
	using pair = std::pair<TFirst, TSecond>;
	template <typename ... TArgs>
	using tuple = std::tuple<TArgs...>;
	template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
	using hash_map = std::unordered_map<TKey, TElement, THash, TOperation>;
	template <typename TKey, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
	using hash_set = std::unordered_set<TKey, THash, TOperation>;
	template <typename TElement, typename TString = string>
	using case_insensitive_hash_map = std::unordered_map<TString, TElement, vana::util::case_insensitive_hash, vana::util::case_insensitive_equals>;
	template <typename TString = string>
	using case_insensitive_hash_set = std::unordered_set<TString, vana::util::case_insensitive_hash, vana::util::case_insensitive_equals>;
	template <typename TKey, typename TElement, typename TOrdering = std::less<TKey>>
	using ord_map = std::map<TKey, TElement, TOrdering>;
	template <typename TKey, typename TOrdering = std::less<TKey>>
	using ord_set = std::set<TKey, TOrdering>;
	template <typename TFunc>
	using function = std::function<TFunc>;
	template <typename TPtr>
	using owned_ptr = std::unique_ptr<TPtr>;
	template <typename TPtr>
	using ref_ptr = std::shared_ptr<TPtr>;
	template <typename TPtr>
	using view_ptr = std::weak_ptr<TPtr>;
	template <typename TMutex>
	using owned_lock = std::unique_lock<TMutex>;
	template <typename TElement, size_t Size>
	using array = std::array<TElement, Size>;
	template <typename TElement>
	using queue = std::deque<TElement>;

	template <typename TPtr>
	using enable_shared = std::enable_shared_from_this<TPtr>;

	template <typename TSrc, typename ...TArgs>
	inline
	auto make_ref_ptr(TArgs && ...args) -> ref_ptr<TSrc> {
		return std::make_shared<TSrc>(args...);
	}

	template <typename TSrc, typename ...TArgs>
	inline
	auto make_owned_ptr(TArgs && ...args) -> owned_ptr<TSrc> {
		return std::make_unique<TSrc>(args...);
	}

	// Game protocol/entity types
	using packet_header = uint16_t;
	using connection_port = uint16_t;
	using crypto_iv = uint32_t;

	using game_world_id = int8_t;
	using game_channel_id = int8_t;
	using game_item_id = int32_t;
	using game_map_id = int32_t;
	using game_mob_id = int32_t;
	using game_npc_id = int32_t;
	using game_shop_id = int32_t;
	using game_job_id = int16_t;
	using game_reactor_id = int32_t;
	using game_party_id = int32_t;
	using game_skill_id = int32_t;
	using game_summon_id = int32_t;
	using game_mist_id = int32_t;
	using game_mob_skill_id = uint8_t;
	using game_pet_id = int64_t;
	using game_player_id = int32_t;
	using game_foothold_id = int16_t;
	using game_seat_id = int16_t;
	using game_trade_id = int32_t;
	using game_trade_slot = uint8_t;
	using game_morph_id = int16_t;
	using game_quest_id = uint16_t;
	using game_portal_id = int8_t;
	using game_account_id = int32_t;
	using game_skin_id = int8_t;
	using game_hair_id = int32_t;
	using game_face_id = int32_t;
	using game_gender_id = int8_t;

	using game_version = uint16_t;
	using game_locale = uint8_t;
	using game_chat = string;
	using game_chat_stream = out_stream;
	using game_player_level = uint8_t;
	using game_skill_level = uint8_t;
	using game_fame = int16_t;
	using game_stat = int16_t;
	using game_health = int16_t;
	using game_health_ap = uint16_t;
	using game_mob_skill_level = uint8_t;
	using game_inventory = int8_t;
	using game_inventory_slot = int16_t;
	using game_inventory_slot_count = uint8_t;
	using game_slot_qty = int16_t;
	using game_storage_slot = uint8_t;
	using game_damage = int32_t;
	using game_mesos = int32_t;
	using game_experience = int32_t;
	using game_map_object = int32_t;
	using game_coord = int16_t;
	using game_tick_count = uint32_t;
	using game_charge_time = int32_t;
	using game_portal_count = uint8_t;
	using game_checksum = uint32_t;

	// Indicates that a code path is not implemented (intentionally) and there is a programming error afoot.
	// Most frequently used with switch-cases on enumerations.
	CODE_EXCEPTION(not_implemented_exception, std::exception);

	// Indicates that generally, some operation has caused an invalid code path to manifest.
	// It may be the result of invalid input or improper usage and the programmer should review to see which.
	CODE_EXCEPTION(invalid_operation_exception, std::exception);

	// Indicates that specifically, this code path was never expected to be hit and there is a problem the programmer must address.
	// Typically this will be high-level invariants that can't be expressed at an API level such as "a user connection will always have a world ID if they make it to character creation."
	CODE_EXCEPTION(codepath_invalid_exception, std::exception);
}