/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Instances.h"
#include "StringUtilities.h"
#include <algorithm>

Instances * Instances::singleton = nullptr;

void Instances::addInstance(Instance *instance) {
	string upper = StringUtilities::toUpper(instance->getName());
	m_instances[upper] = instance;
}

void Instances::removeInstance(Instance *instance) {
	string upper = StringUtilities::toUpper(instance->getName());
	m_instances.erase(upper);
}

Instance * Instances::getInstance(const string &name) {
	string upper = StringUtilities::toUpper(name);
	auto kvp = m_instances.find(upper);
	return kvp != m_instances.end() ? kvp->second : nullptr;
}

bool Instances::isInstance(const string &name) {
	string upper = StringUtilities::toUpper(name);
	auto kvp = m_instances.find(upper);
	bool exists = kvp != m_instances.end();
	if (exists && kvp->second->getMarkedForDelete()) {
		exists = false;
		Instance *instance = kvp->second;
		m_instances.erase(upper);
		delete instance;
	}
	return exists;
}