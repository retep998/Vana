--[[
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
--]]
-- Second Horntail room portal in the middle

if not isInstance("horntail") then
	return;
end

rightHead = getInstanceVariable("right_head", type_bool);
if not rightHead then
	gm = isGm();
	gmInstance = getInstanceVariable("gm", type_bool);
	if gm == gmInstance then
		showMessage("The enormous creature is approaching from the deep cave.", msg_blue);
		setReactorState(getMap(), 2408003, 0);
		spawnMobPos(getMap(), 8810025, -350, 220);
		setInstanceVariable("right_head", true);
	end
end