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
-- Third Eos Rock

if getItemAmount(4001020) >= 1 then
	addText("You can use #b#t4001020##k to activate #b#p2040026##k. Which of these rocks would you like to teleport to?\r\n");
	addText("#b#L0# #p2040025#(71st floor)#l\r\n");
	addText("#b#L1# #p2040027#(1st floor)#l");
	what = askChoice();
	
	if what == 0 then
		map = 221022900;
		addText("You can use #b#t4001020##k to activate #b#p2040026##k. Will you teleport to #b#p2040025##k at the 71st floor?");
	elseif what == 1 then
		map = 221020000;
		addText("You can use #b#t4001020##k to activate #b#p2040026##k. Will you teleport to #b#p2040027##k at the 1st floor?");
	end
	yes = askYesNo();
	
	if yes == 1 then
		giveItem(4001020, -1);
		setMap(map, "go00");
	end
else
	addText("There's a rock that will enable you to teleport to #b#p2040025# or #p2040027##k, but it cannot be activated without the scroll.");
	sendOK();
end
