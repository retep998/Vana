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

#include "Rect.h"
#include "Types.h"
#include <unordered_map>
#include <vector>

struct ReactorStateInfo {
	int8_t nextState = 0;
	int16_t type = 0;
	int16_t itemQuantity = 0;
	int32_t itemId = 0;
	int32_t timeout = 0;
	Rect dimensions;
	vector_t<int32_t> triggerSkills;
};

struct ReactorData {
	bool removeInFieldSet = false;
	bool activateByTouch = false;
	int8_t maxStates = 0;
	int32_t link = 0;
	hash_map_t<int8_t, vector_t<ReactorStateInfo>> states;
};

class ReactorDataProvider {
	SINGLETON(ReactorDataProvider);
public:
	auto loadData() -> void;

	auto getReactorData(int32_t reactorId, bool respectLink = false) -> ReactorData *;
private:
	auto loadReactors() -> void;
	auto loadStates() -> void;
	auto loadTriggerSkills() -> void;

	hash_map_t<int32_t, ReactorData> m_reactorInfo;
};