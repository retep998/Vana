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

#include "Rect.hpp"
#include "Types.hpp"
#include <unordered_map>
#include <vector>

struct ReactorStateInfo {
	int8_t nextState = 0;
	int16_t type = 0;
	slot_qty_t itemQuantity = 0;
	item_id_t itemId = 0;
	int32_t timeout = 0;
	Rect dimensions;
	vector_t<skill_id_t> triggerSkills;
};

struct ReactorData {
	bool removeInFieldSet = false;
	bool activateByTouch = false;
	int8_t maxStates = 0;
	reactor_id_t link = 0;
	hash_map_t<int8_t, vector_t<ReactorStateInfo>> states;
};

class ReactorDataProvider {
public:
	auto loadData() -> void;

	auto getReactorData(reactor_id_t reactorId, bool respectLink = false) const -> const ReactorData &;
private:
	auto loadReactors() -> void;
	auto loadStates() -> void;
	auto loadTriggerSkills() -> void;

	hash_map_t<reactor_id_t, ReactorData> m_reactorInfo;
};