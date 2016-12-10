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
#include "meso_inventory.hpp"
#include "common/algorithm.hpp"
#include "common/constant/inventory.hpp"
#include "common/data/provider/item.hpp"
#include "common/util/misc.hpp"

namespace vana {
namespace util {

meso_inventory::meso_inventory(game_mesos mesos) :
	m_mesos{mesos}
{
	if (mesos < 0) {
		throw std::invalid_argument{"Must be 0 or greater"};
	}
}

auto meso_inventory::get_mesos() const -> game_mesos {
	return m_mesos;
}

auto meso_inventory::can_add_mesos(game_mesos mesos) const -> stack_result {
	if (mesos < 0) throw std::invalid_argument{"Must not be negative"};
	return can_modify_mesos(mesos);
}

auto meso_inventory::can_take_mesos(game_mesos mesos) const -> stack_result {
	if (mesos < 0) throw std::invalid_argument{"Must not be negative"};
	return can_modify_mesos(-mesos);
}

auto meso_inventory::can_modify_mesos(game_mesos mesos) const -> stack_result {
	if ((m_mesos == 0 && mesos < 0) || (m_mesos == std::numeric_limits<game_mesos>::max() && mesos > 0)) {
		return stack_result::none;
	}
	if (vana::util::misc::is_non_overflowing_addition_with_zero_min(m_mesos, mesos)) {
		return stack_result::full;
	}
	return stack_result::partial;
}

auto meso_inventory::can_accept(const meso_inventory &other) const -> stack_result {
	return can_modify_mesos(other.get_mesos());
}

auto meso_inventory::has_any() const -> bool {
	return m_mesos > 0;
}

auto meso_inventory::set_mesos(game_mesos new_mesos) -> result {
	if (new_mesos < 0) {
		return result::failure;
	}

	m_mesos = new_mesos;
	return result::success;
}

auto meso_inventory::add_mesos(game_mesos mesos, bool allow_partial) -> meso_modify_result {
	return modify_mesos_internal(can_add_mesos(mesos), mesos, allow_partial);
}

auto meso_inventory::take_mesos(game_mesos mesos, bool allow_partial) -> meso_modify_result {
	return modify_mesos_internal(can_take_mesos(mesos), mesos, allow_partial);
}

auto meso_inventory::modify_mesos(game_mesos mesos, bool allow_partial) -> meso_modify_result {
	return modify_mesos_internal(can_modify_mesos(mesos), mesos, allow_partial);
}

auto meso_inventory::modify_mesos_internal(stack_result query, game_mesos mesos, bool allow_partial) -> meso_modify_result {
	if (query == stack_result::none || (!allow_partial && query == stack_result::partial)) {
		return meso_modify_result{stack_result::none, m_mesos, 0};
	}

	game_mesos amount = mesos;
	if (query == stack_result::partial) {
		if (amount > 0) {
			amount -= (std::numeric_limits<game_mesos>::max() - m_mesos);
		}
		else {
			amount = -m_mesos;
		}
		m_mesos += amount;
	}
	else {
		m_mesos += amount;
	}
	return meso_modify_result{query, m_mesos, amount};
}

auto meso_inventory::accept(meso_inventory &other, bool allow_partial) -> meso_modify_result {
	game_mesos amount = other.get_mesos();
	auto query = modify_mesos(amount, allow_partial);
	if (query.get_result() == stack_result::none) {
		return query;
	}
	if (query.get_result() == stack_result::full) {
		other.set_mesos(0);
		return query;
	}
	other.set_mesos(amount - query.get_applied_amount());
	return query;
}

}
}