/*
Copyright (C) 2008-2011 Vana Development Team

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
#include <boost/algorithm/string.hpp>

Instances * Instances::singleton = nullptr;

void Instances::addInstance(Instance *instance) {
	m_instances[boost::to_upper_copy(instance->getName())] = instance;
}

void Instances::removeInstance(Instance *instance) {
	m_instances.erase(boost::to_upper_copy(instance->getName()));
}

Instance * Instances::getInstance(const string &name) {
	string upCaseName = boost::to_upper_copy(name);
	return (m_instances.find(upCaseName) == m_instances.end()) ? nullptr : m_instances[upCaseName];
}

bool Instances::isInstance(const string &name) {
	string upCaseName = boost::to_upper_copy(name);
	bool is = m_instances.find(upCaseName) != m_instances.end();
	if (is && m_instances[upCaseName]->getMarkedForDelete()) {
		is = false;
		Instance *instance = m_instances[upCaseName];
		m_instances.erase(upCaseName);
		delete instance;
	}
	return is;
}