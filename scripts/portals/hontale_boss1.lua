--[[
Copyright (C) 2008-2012 Vana Development Team

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
-- First Horntail room portal in the middle

leftHead = not getInstanceVariable("lefthead", true);

if not leftHead then
	gm = isGm();
	gmInstance = getInstanceVariable("gm", true);
	if (gm and gmInstance) or (not gm and not gmInstance) then
		showMessage("The enormous creature is approaching from the deep cave.", env_blueMessage);
		setReactorState(getMap(), 2408003, 0);
		spawnMobPos(8810024, 880, 220);
		setInstanceVariable("lefthead", "1");
	end
end