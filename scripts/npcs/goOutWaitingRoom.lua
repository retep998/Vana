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
-- Various waiting room attendants

addText("Do you want to leave the waiting room? You can, but the ticket is NOT refundable. Are you sure you still want to leave this room?");
yes = askYesNo();

if yes == 1 then
	m = getMap();
	toMap = 0;

	if m == 101000301 then toMap = 101000300;
	elseif m == 200000112 then toMap = 200000100;
	elseif m == 200000122 then toMap = 200000100;
	elseif m == 200000132 then toMap = 200000100;
	elseif m == 200000152 then toMap = 200000100;
	elseif m == 220000110 then toMap = 220000100;
	elseif m == 240000111 then toMap = 240000100;
	elseif m == 260000110 then toMap = 260000100;
	else consoleOutput("Unsupported goOutWaitingRoom map: " .. m);
	end

	if toMap ~= 0 then
		setMap(toMap, "sp");
	end
else
	addText("You'll get to your destination in a few. Go ahead and talk to other people, and before you know it, you'll be there already.");
	sendNext();
end