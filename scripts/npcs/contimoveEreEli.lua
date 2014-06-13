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
-- Kiriru

dofile("scripts/utils/boatHelper.lua");
dofile("scripts/utils/jobHelper.lua");
dofile("scripts/utils/npcHelper.lua");

price = nil;
if getLevel() < 30 then
	if getLevel() < 13 and isCygnus() then
		addText("Hmm. ");
		addText("You haven't reached Lv. 13 yet... ");
		addText("Why don't you train a bit more? ");
		addText("Victoria Island can be a rough place. ");
		addText("I think you should get to Lv. 13 and get assigned to Henesys before you head out. ");
		addText("Of course I can't stop you if you insist on going now... ");
		sendNext();
	end

	price = 100;
else
	price = 1000;
end

addText("Ummm, are you trying to leave Ereve again? ");
addText("I can take you to " .. blue("Ellinia") .. " if you want...\r\n\r\n");
addText("You'll have to pay a fee of " .. blue(price) .. " Mesos.");
answer = askYesNo();

if answer == answer_no then
	addText("If not, forget it.");
	sendNext();
else
	mapId = queryBoatMap("ereveToElliniaTrip");
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