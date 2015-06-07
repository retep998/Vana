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
-- Sparkling Crystal

dofile("scripts/utils/npcHelper.lua");

addText("You can use the " .. npcRef(1061010) .. " to go back to the real world. ");
addText("Are you sure you want to go back?");
answer = askYesNo();

if answer == answer_yes then
	destinationMap = nil;
	m = getMap();
	if m == 108010101 then destinationMap = 100000000;
	elseif m == 108010201 then destinationMap = 101000000;
	elseif m == 108010301 then destinationMap = 102000000;
	elseif m == 108010401 then destinationMap = 103000000;
	elseif m == 108010501 then destinationMap = 120000000;
	end

	setMap(destinationMap);
end