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
#include "Instances.hpp"
#include "Common/StringUtilities.hpp"
#include <algorithm>

namespace Vana {
namespace ChannelServer {

auto Instances::addInstance(Instance *instance) -> void {
	m_instances[instance->getName()] = instance;
}

auto Instances::removeInstance(Instance *instance) -> void {
	m_instances.erase(instance->getName());
}

auto Instances::getInstance(const string_t &name) -> Instance * {
	return isInstance(name) ? m_instances[name] : nullptr;
}

auto Instances::isInstance(const string_t &name) -> bool {
	auto kvp = m_instances.find(name);
	bool exists = kvp != std::end(m_instances);
	if (exists && kvp->second->getMarkedForDelete()) {
		exists = false;
		Instance *instance = kvp->second;
		m_instances.erase(name);
		delete instance;
	}
	return exists;
}

}
}