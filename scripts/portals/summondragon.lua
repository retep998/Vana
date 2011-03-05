--[[
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
--]]
-- Nine Spirit's Egg portal that summons the dragon

if getInstanceVariable("egg", true) ~= 1 and getItemAmount(4001094) > 0 then
	giveItem(4001094, -1);
	setReactorState(240040611, 2406000, 1);
	showMessage("The Egg of Nine Spirit, which was comfotably nested, has emitted a mysterious light and has returned to its nest.", m_red);
	setInstanceVariable("egg", "1");
end