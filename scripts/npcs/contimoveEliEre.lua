--[[
Copyright (C) 2008-2016 Vana Development Team

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
-- Kiriru

dofile("scripts/utils/mapManagerHelper.lua");
dofile("scripts/utils/npcHelper.lua");

price = nil;
if getLevel() < 30 then
	price = 100;
else
	price = 1000;
end

addText("Oh, and... so... this ship will take you to " .. blue("Ereve") .. ", the place where you'll find crimson leaves soaking up the sun, the gentle breeze that glides past the stream, and the Empress of Maple, Cygnus. ");
addText("Would you like to head over to Ereve?\r\n\r\n");
addText("The trip costs " .. blue(price) .. " Mesos.");
answer = askYesNo();

if answer == answer_no then
	addText("If you're not interested, then oh well...");
	sendNext();
else
	mapId = queryManagedMap("elliniaToEreveTrip");
	if mapId == nil then
		addText("There are no more boats available right now. ");
		addText("Try again later.");
		sendNext();
	elseif giveMesos(-price) then
		setMap(mapId);
	else
		addText("I don't think you have enough mesos, double check your inventory.");
		sendNext();
	end
end