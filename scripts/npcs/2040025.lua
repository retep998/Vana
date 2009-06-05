--[[
Copyright (C) 2008-2009 Vana Development Team

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
-- Second Eos Rock

if state == 0 then
	if getItemAmount(4001020) >= 1 then
		addText("You can use #b#t4001020##k to activate #b#p2040025##k. Which of these rocks would you like to teleport to?\r\n");
		addText("#b#L0# #p2040024#(100th floor)#l\r\n");
		addText("#b#L1# #p2040026#(41st floor)#l");
		sendSimple();
	else
		addText("There's a rock that will enable you to teleport to #b#p2040024# or #p2040026##k, but it cannot be activated without the scroll.");
		sendOK();
		endNPC();
	end
elseif state == 1 then
	answer = getSelected();
	if answer == 1 then
		addText("You can use #b#t4001020##k to activate #b#p2040025##k. Will you teleport to #b#p2040024##k at the 100st floor?");
	elseif answer == 2 then
		addText("You can use #b#t4001020##k to activate #b#p2040025##k. Will you teleport to #b#p2040026##k at the 41st floor?");
	end
	sendYesNo();
elseif state == 2 then
	answer2 = getSelected();
	if answer == 1 and answer2 == 1 then
		giveItem(4001020, -1);
		setMap(221024400, "go00");
	elseif answer == 2 and answer2 == 1 then
		giveItem(4001020, -1);
		setMap(221021700, "go00");
	end
	endNPC();
end