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

#include "BuffInfo.hpp"
#include "BuffSkillValue.hpp"
#include "Types.hpp"

namespace Vana {
	class Buff {
	public:
		Buff();
		Buff(const init_list_t<BuffInfo> &buffInfo);
		Buff(const vector_t<BuffInfo> &buffInfo);

		auto getDelay() const -> milliseconds_t;
		auto getBuffInfo() const -> const vector_t<BuffInfo> &;
		auto isSelectionBuff() const -> bool;
		auto anyBuffs() const -> bool;
		auto anyActs() const -> bool;
		auto withBuffs(const vector_t<BuffInfo> &buffInfo) const -> Buff;
		auto withDelay(milliseconds_t value) const -> Buff;
	private:
		auto parseBuffsForData() -> void;
		auto sort() -> void;

		bool m_isSelectionBuff = false;
		bool m_anyActs = false;
		milliseconds_t m_delay = milliseconds_t{0};
		vector_t<BuffInfo> m_buffInfo;
	};
}