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
-- Moose

dofile("scripts/lua_functions/npcHelper.lua");

mapId = getMap();
item = 1092041;

if mapId == 924000000 then
	addText("I have to let you know one thing before sending you to the training field. ");
	addText("You have to hold " .. blue(itemRef(item)) .. " that I gave you in shield training field. ");
	addText("Otherwise, you're dead.");
	sendNext();

	addText("Don't forget " .. red("to hold shield") .. " before you get there!\r\n");
	addText(blue(choiceList({
		" I want to get " .. itemRef(item) .. ".",
		" Let me go in to " .. mapRef(924000001) .. ".",
		" Let me out.",
	})));
	choice = askChoice();

	if choice == 0 then
		if getItemAmount(item) > 0 then
			addText("You already have " .. blue(itemRef(item)) .. ". ");
			addText("No need more.");
			sendNext();			
		else
			if destroyEquippedItem(item) then
				showMessage("Shield for learning skill was removed.", msg_red);
			end

			if hasOpenSlotsFor(item, result) then
				giveItem(item, 1);
				addText("I gave you " .. itemRef(item) .. ". ");
				addText("Check inventory. ");
				addText("You have to be equipped with it!");
				sendNext();
			else
				addText("I couldn't give you " .. blue(itemRef(item)) .. " as there's no blank in Equipment box. ");
				addText("Make a blank and try again.");
				sendNext();
			end
		end
	elseif choice == 1 then
		if isInstance("guardian") then
			addText("Other characters are on request. ");
			addText("You can't enter.");
			sendNext();
		else
			createInstance("guardian", 20 * 60, true);
			setMap(924000001);
		end
	elseif choice == 2 then
		setMap(924000002);
	end
elseif mapId == 924000001 then
	addText("Do you want to get out of here?");
	answer = askYesNo();
	if answer == answer_yes then
		setMap(924000002);
	end
elseif mapId == 924000002 then
	addText("I'll let you out. ");
	addText("You have to give " .. itemRef(item) .. " back.");
	sendNext();

	if destroyEquippedItem(item) then
		showMessage("Shield for learning skill was removed.", msg_red);
	end

	setMap(240010400, "st00");
end