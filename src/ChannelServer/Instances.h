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
#pragma once

#include "Instance.h"
#include "Types.h"
#include <string>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::string;
using std::tr1::unordered_map;

class Instances : boost::noncopyable {
public:
	static Instances * InstancePtr() { // The irony, it burns!
		if (singleton == nullptr)
			singleton = new Instances;
		return singleton;
	}

	void addInstance(Instance *instance);
	void removeInstance(Instance *instance);
	Instance * getInstance(const string &name);
	bool isInstance(const string &name);
private:
	Instances() {};
	static Instances *singleton;

	unordered_map<string, Instance *> m_instances; // Index of instances by name
};
