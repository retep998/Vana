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
#include "BuffSource.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/SkillDataProvider.hpp"

namespace Vana {

auto BuffSource::fromItem(item_id_t itemId) -> BuffSource {
	BuffSource ret;
	ret.m_type = BuffSourceType::Item;
	ret.m_itemId = itemId;
	ret.m_skillLevel = 1;
	return ret;
}

auto BuffSource::fromSkill(skill_id_t skillId, skill_level_t skillLevel) -> BuffSource {
	BuffSource ret;
	ret.m_type = BuffSourceType::Skill;
	ret.m_skillId = skillId;
	ret.m_skillLevel = skillLevel;
	return ret;
}

auto BuffSource::fromMobSkill(mob_skill_id_t skillId, mob_skill_level_t skillLevel) -> BuffSource {
	BuffSource ret;
	ret.m_type = BuffSourceType::MobSkill;
	ret.m_mobSkillId = skillId;
	ret.m_mobSkillLevel = skillLevel;
	return ret;
}

auto BuffSource::getType() const -> BuffSourceType {
	return m_type;
}

auto BuffSource::getItemId() const -> item_id_t {
	return m_itemId;
}

auto BuffSource::getSkillId() const -> skill_id_t {
	return m_skillId;
}

auto BuffSource::getSkillLevel() const -> skill_level_t {
	return m_skillLevel;
}

auto BuffSource::getMobSkillId() const -> mob_skill_id_t {
	return m_mobSkillId;
}

auto BuffSource::getMobSkillLevel() const -> mob_skill_level_t {
	return m_mobSkillLevel;
}

auto BuffSource::getId() const -> int32_t {
	switch (m_type) {
		case BuffSourceType::Item: return m_itemId;
		case BuffSourceType::Skill: return m_skillId;
		case BuffSourceType::MobSkill: return m_mobSkillId;
	}
	throw NotImplementedException{"BuffSourceType"};
}

auto BuffSource::getItemData(const ItemDataProvider &provider) const -> const ConsumeInfo * const {
	return provider.getConsumeInfo(m_itemId);
}

auto BuffSource::getSkillData(const SkillDataProvider &provider) const -> const SkillLevelInfo * const {
	return provider.getSkill(m_skillId, m_skillLevel);
}

auto BuffSource::getMobSkillData(const SkillDataProvider &provider) const -> const MobSkillLevelInfo * const {
	return provider.getMobSkill(m_mobSkillId, m_mobSkillLevel);
}

}