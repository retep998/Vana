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
-- Fourth Eos Rock

if getItemAmount(4001020) >= 1 then
	addText("You can use #b#t4001020##k to activate #b#p2040027##k. Will you head over to #b#p2040026##k at the 41st floor??");
	yes = askYesNo();
	
	if yes == 1 then
		giveItem(4001020, -1);
		setMap(221021700, "go00");
	end
elseif getItemAmount(4001020) == 0 then
	addText("There's a rock that will enable you to teleport to #b#p2040026##k, but it cannot be activated without a scroll.");
	sendOK();
end
