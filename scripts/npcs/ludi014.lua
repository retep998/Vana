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
-- First Eos Rock

if getItemAmount(4001020) >= 1 then
	addText("You can use #b#t4001020##k to activate #b#p2040024##k. Will you teleport to #b#p2040025##k at the 71st floor?");
	yes = askYesNo();
    
	if yes == 1 then
		giveItem(4001020, -1);
		setMap(221022900, "go00");
	end
elseif getItemAmount(4001020) == 0 then
	addText("There's a rock that will enable you to teleport to #b#p2040025##k, but it cannot be activated without the scroll.");
	sendOK();
end
