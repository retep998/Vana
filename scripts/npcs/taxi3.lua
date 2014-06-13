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
-- Regular Cab (Kerning cab)

dofile("scripts/utils/npcHelper.lua");

function generateChoice(mapId, price)
	return makeChoiceData(mapRef(mapId) .. "(" .. price .. " mesos) ", {mapId, price});
end

function getPrice(basePrice)
	if getJob() == 0 then
		return basePrice / 10;
	end
	return basePrice;
end

choices = {
	generateChoice(104000000, getPrice(1000)),
	generateChoice(102000000, getPrice(800)),
	generateChoice(101000000, getPrice(1200)),
	generateChoice(100000000, getPrice(1000)),
};

addText("Hi! I drive the " .. npcRef(1052016) .. ". ");
addText("If you want to go from town to town safely and fast, then ride our cab. ");
addText("We'll gladly take you to your destination with an affordable price.");
sendNext();

if getJob() == 0 then
	addText("We have a special 90% discount for beginners. ");
end
addText("Choose your destination, for fees will change from place to place.\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

data = selectChoice(choices, choice);
mapId, price = data[1], data[2];

addText("You don't have anything else to do here, huh? ");
addText("Do you really want to go to " .. blue(mapRef(mapId)) .. "? ");
addText("It'll cost you " .. blue(price .. " mesos") .. ".");
answer = askYesNo();

if answer == answer_yes then
	if giveMesos(-price) then
		setMap(mapId);
	else
		addText("You don't have enough mesos. ");
		addText("Sorry to say this, but without them, you won't be able to ride this cab.");
		sendOk();
	end
else
	addText("There's a lot to see in this town, too. ");
	addText("Come back and find me when you need to go to a different town.");
	sendOk();
end