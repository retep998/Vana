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
#include "status_info.hpp"
#include "common/util/randomizer.hpp"

namespace vana {
namespace channel_server {

status_info::status_info(int32_t status, int32_t val, int32_t skill_id, seconds time) :
	status{status},
	val{val},
	skill_id{skill_id},
	time{time},
	reflection{0}
{
	switch (val) {
		case constant::status_effect::mob::freeze:
			if (skill_id == constant::skill::fp_arch_mage::paralyze) {
				break;
			}
		case constant::status_effect::mob::stun:
			time = seconds{
				time.count() +
				(skill_id == constant::skill::il_arch_mage::blizzard ? 3 : 1) +
				vana::util::randomizer::rand<int32_t>(static_cast<int32_t>(time.count()) * 2)
			};
			// The 1 accounts for the skill cast time
			// Ideally we'd like to remove both these additions with MCDB support for cast times
			break;
	}
}

status_info::status_info(int32_t status, int32_t val, int16_t mob_skill, int16_t level, seconds time) :
	status{status},
	val{val},
	skill_id{-1},
	mob_skill{mob_skill},
	level{level},
	time{time}
{
}

status_info::status_info(int32_t status, int32_t val, int16_t mob_skill, int16_t level, int32_t reflect, seconds time) :
	status{status},
	val{val},
	skill_id{-1},
	mob_skill{mob_skill},
	level{level},
	time{time},
	reflection{reflect}
{
}

}
}