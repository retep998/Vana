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
#include "instances.hpp"
#include "common/util/string.hpp"
#include <algorithm>

namespace vana {
namespace channel_server {

auto instances::add_instance(instance *inst) -> void {
	m_instances[inst->get_name()] = inst;
}

auto instances::remove_instance(instance *inst) -> void {
	m_instances.erase(inst->get_name());
}

auto instances::get_instance(const string &name) -> instance * {
	return is_instance(name) ? m_instances[name] : nullptr;
}

auto instances::is_instance(const string &name) -> bool {
	auto kvp = m_instances.find(name);
	bool exists = kvp != std::end(m_instances);
	if (exists && kvp->second->get_marked_for_delete()) {
		exists = false;
		instance *inst = kvp->second;
		m_instances.erase(name);
		delete inst;
	}
	return exists;
}

}
}