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
-- Kiru

dofile("scripts/utils/boatHelper.lua");
dofile("scripts/utils/npcHelper.lua");

price = nil;
if getLevel() < 30 then
	price = 100;
else
	price = 1000;
end

addText("This ship will head toward " .. blue(mapRef(130000000)) .. ", an island where you'll find crimson leaves soaking up the sun, the gentle breeze that glides past the stream, and the Empress of Maple, Cygnus. ");
addText("If you're interested in joining the Cygnus Knights, then you should definitely pay a visit there. ");
addText("Are you interested in visiting " .. mapRef(130000000) .. "?\r\n\r\n");
addText("The trip will cost you " .. blue(price) .. " Mesos.");
answer = askYesNo();

if answer == answer_no then
	addText("Not interested? ");
	addText("Oh well...");
	sendNext();
else
	mapId = queryBoatMap("orbisToEreveTrip");
	if mapId == nil then
		addText("There are no more boats available right now. ");
		addText("Try again later.");
		sendNext();
	elseif giveMesos(-price) then
		setMap(mapId);
	else
		addText("Hey, you don't have enough Mesos with you... the ride will cost you " .. blue(price) .. " Mesos.");
		sendNext();
	end
end