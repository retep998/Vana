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

addText("Hmm... ");
addText("The winds are favorable. ");
addText("Are you thinking of leaving " .. mapRef(130000000) .. " and going somewhere else? ");
addText("This ferry sails to Orbis on the Ossyria Continent. ");
sendNext();

if getLevel() < 30 then
	addText("Wait, why are you going to Ossyria whenyou're not even Lv. 30? ");
	addText("Do you know how tough the monsters in Orbis are?! ");
	addText("Going there at your level is very dangerous. ");
	addText("Do you still want to go?");
	sendBackNext();
end

addText("Have you taken care of everything you needed to in " .. mapRef(130000000) .. "? ");
addText("If you happen to be headed toward " .. blue(mapRef(200000000)) .. " I can take you there. ");
addText("What do you say? ");
addText("Are you going to go to " .. mapRef(200000000) .. "?\r\n\r\n");
addText("You'll have to pay a fee of " .. blue(price) .. " Mesos.");
answer = askYesNo();

if answer == answer_no then
	addText("If not, forget it.");
	sendNext();
else
	mapId = queryBoatMap("ereveToOrbisTrip");
	if mapId == nil then
		addText("There are no more boats available right now. ");
		addText("Try again later.");
		sendNext();
	elseif giveMesos(-price) then
		setMap(mapId);
	else
		addText("Hey you don't have enough Mesos with you... the ride will cost you " .. blue(price) .. " Mesos.");
		sendNext();
	end
end