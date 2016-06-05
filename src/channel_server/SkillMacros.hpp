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

#include "common/Types.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vana {
	namespace channel_server {
		class skill_macros {
		public:
			struct skill_macro;

			auto add(uint8_t pos, skill_macro *macro) -> void;
			auto get_skill_macro(int8_t pos) -> skill_macro *;
			auto get_max() -> int8_t;

			auto load(game_player_id char_id) -> void;
			auto save(game_player_id char_id) -> void;
		private:
			int8_t m_max_point = -1;
			hash_map<int8_t, ref_ptr<skill_macro>> m_skill_macros;
		};

		struct skill_macros::skill_macro {
			NONCOPYABLE(skill_macro);
			NO_DEFAULT_CONSTRUCTOR(skill_macro);
		public:
			skill_macro(const string &name, bool shout, game_skill_id skill1, game_skill_id skill2, game_skill_id skill3);

			bool shout = false;
			game_skill_id skill1 = 0;
			game_skill_id skill2 = 0;
			game_skill_id skill3 = 0;
			string name;
		};

		inline
		auto skill_macros::add(uint8_t pos, skill_macro *macro) -> void {
			m_skill_macros[pos].reset(macro);
			if (m_max_point < pos) {
				m_max_point = pos;
			}
		}

		inline
		skill_macros::skill_macro * skill_macros::get_skill_macro(int8_t pos) {
			if (m_skill_macros.find(pos) != std::end(m_skill_macros)) {
				return m_skill_macros[pos].get();
			}
			return nullptr;
		}

		inline
		auto skill_macros::get_max() -> int8_t {
			return m_max_point;
		}

		inline
		skill_macros::skill_macro::skill_macro(const string &name, bool shout, game_skill_id skill1, game_skill_id skill2, game_skill_id skill3) :
			name{name},
			shout{shout},
			skill1{skill1},
			skill2{skill2},
			skill3{skill3}
		{
		}
	}
}