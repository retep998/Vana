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
#pragma once

#include "TimerContainerHolder.hpp"
#include "Types.hpp"
#include "Variables.hpp"
#include <memory>
#include <string>

class EventDataProvider : public TimerContainerHolder {
public:
	EventDataProvider();
	auto loadData() -> void;
	auto clearInstances() -> void;
	auto getVariables() const -> Variables * { return m_variables.get(); }
private:
	auto loadEvents() -> void;
	auto loadInstances() -> void;
	auto startInstance(const string_t &name, const duration_t &time, const duration_t &repeat = seconds_t{0}) -> void;

	owned_ptr_t<Variables> m_variables;
};