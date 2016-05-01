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

#include "Common/BuffSourceType.hpp"
#include "Common/Types.hpp"

namespace Vana {
	class ItemDataProvider;
	class SkillDataProvider;
	struct ConsumeInfo;
	struct MobSkillLevelInfo;
	struct SkillLevelInfo;

	class BuffSource {
	public:
		BuffSource() = default;

		auto getType() const -> BuffSourceType;
		auto getItemId() const -> item_id_t;
		auto getSkillId() const -> skill_id_t;
		auto getSkillLevel() const -> skill_level_t;
		auto getMobSkillId() const -> mob_skill_id_t;
		auto getMobSkillLevel() const -> mob_skill_level_t;
		auto getId() const -> int32_t;
		auto getItemData(const ItemDataProvider &provider) const -> const ConsumeInfo * const;
		auto getSkillData(const SkillDataProvider &provider) const -> const SkillLevelInfo * const;
		auto getMobSkillData(const SkillDataProvider &provider) const -> const MobSkillLevelInfo * const;

		static auto fromItem(item_id_t itemId) -> BuffSource;
		static auto fromSkill(skill_id_t skillId, skill_level_t skillLevel) -> BuffSource;
		static auto fromMobSkill(mob_skill_id_t skillId, mob_skill_level_t skillLevel) -> BuffSource;
	private:
		BuffSourceType m_type = BuffSourceType::None;
		skill_id_t m_skillId = 0;
		skill_level_t m_skillLevel = 0;
		mob_skill_id_t m_mobSkillId = 0;
		mob_skill_level_t m_mobSkillLevel = 0;
		item_id_t m_itemId = 0;
	};
}