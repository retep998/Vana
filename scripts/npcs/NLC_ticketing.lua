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
-- Bell, warps to/from Kerning and NLC
-- Originally programmed by Biscuit

dofile("scripts/lua_functions/npcHelper.lua");

map = getMap();
cost = 5000;
item = nil;

function isBasicTicket()
	return getLevel() <= 10;
end

if isBasicTicket() then
	cost = 1000;
end

if map == 103000100 then
	addText("Hi, I can take you to New Leaf City (NLC) for only " .. blue(cost .. " mesos") .. ". ");
	addText("Would you like to go?");
	answer = askYesNo();

	if answer == answer_yes then
		if isBasicTicket() then item = 4031710;
		else item = 4031711;
		end
	else
		addText("Ok, come talk to me again when you want to go to NLC.");
		sendOk();
	end
elseif map == 600010001 then
	addText("Hi, I can take you back to Kerning City for only " .. blue(cost .. " mesos") .. ". ");
	addText("Would you like to go?");
	answer = askYesNo();

	if answer == answer_yes then
		if isBasicTicket() then item = 4031712;
		else item = 4031713;
		end
	else
		addText("Ok, come talk to me again when you want to go back to Kerning City.");
		sendOk();
	end
elseif map == 600010004 then
	addText("Do you want to go back to Kerning City subway station now?");
	answer = askYesNo();

	if answer == answer_no then
		addText("Okay, Please wait~!");
		sendNext();
	else
		setMap(103000100);
	end
elseif map == 600010002 then
	addText("Do you want to go back to New Leaf City subway station now?");
	answer = askYesNo();

	if answer == answer_no then
		addText("Okay, Please wait~!");
		sendNext();
	else
		setMap(600010001);
	end
end

if item then
	if hasOpenSlotsFor(item, 1) and giveMesos(-cost) then
		giveItem(item, 1);
	else
		addText("I'm sorry, but you don't have enough money. ");
		addText("It costs " .. blue(cost .. " Mesos") .. ". ");
		sendOk();
	end
end