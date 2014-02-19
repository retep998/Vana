--[[
Copyright (C) 2008-2014 Vana Development Team

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
-- Various waiting room attendants

dofile("scripts/lua_functions/npcHelper.lua");

addText("Do you want to leave the waiting room? ");
addText("You can, but the ticket is NOT refundable. ");
addText("Are you sure you still want to leave this room?");
answer = askYesNo();

if answer == answer_yes then
	m = getMap();
	destinationMap = nil;
	if m == 101000301 then destinationMap = 101000300;
	elseif m == 200000112 then destinationMap = 200000100;
	elseif m == 200000122 then destinationMap = 200000100;
	elseif m == 200000132 then destinationMap = 200000100;
	elseif m == 200000152 then destinationMap = 200000100;
	elseif m == 220000110 then destinationMap = 220000100;
	elseif m == 240000111 then destinationMap = 240000100;
	elseif m == 260000110 then destinationMap = 260000100;
	else
		consoleOutput("Unsupported goOutWaitingRoom map: " .. m);
		return;
	end

	setMap(destinationMap);
else
	addText("You'll get to your destination in a few. ");
	addText("Go ahead and talk to other people, and before you know it, you'll be there already.");
	sendNext();
end