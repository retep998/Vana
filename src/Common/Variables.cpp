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
#include "Variables.h"

void Variables::deleteVariable(const string &name) {
	if (m_variables.find(name) != m_variables.end()) {
		m_variables.erase(name);
	}
}

void Variables::setVariable(const string &name, const string &val) {
	m_variables[name] = val;
}

string Variables::getVariable(const string &name) {
	return (m_variables.find(name) == m_variables.end()) ? "" : m_variables[name];
}