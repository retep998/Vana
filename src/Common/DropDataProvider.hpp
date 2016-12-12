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

#include "ItemDataObjects.hpp"
#include "Types.hpp"
#include <unordered_map>
#include <vector>

class DropDataProvider {
public:
	auto loadData() -> void;

	auto hasDrops(int32_t objectId) const -> bool;
	auto getDrops(int32_t objectId) const -> const vector_t<DropInfo> &;
	auto getGlobalDrops() const -> const vector_t<GlobalDrop> &;
private:
	auto loadDrops() -> void;
	auto loadGlobalDrops() -> void;

	hash_map_t<int32_t, vector_t<DropInfo>> m_dropInfo;
	vector_t<GlobalDrop> m_globalDrops;
};