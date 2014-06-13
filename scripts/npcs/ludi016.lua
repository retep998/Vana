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
-- Third Eos Rock

dofile("scripts/utils/npcHelper.lua");

if isGm() or getItemAmount(4001020) > 0 then
	function makeChoice(npcId, mapId, floorName, floorConfirmName)
		if floorConfirmName == nil then
			floorConfirmName = floorName;
		end
		return makeChoiceData(" " .. npcRef(npcId) .. "(" .. floorName .. " floor)", {mapId, npcId, floorConfirmName});
	end

	choices = {
		makeChoice(2040025, 221022900, "71st"),
		makeChoice(2040027, 221020000, "1st"),
	};

	addText("You can use " .. blue(itemRef(4001020)) .. " to activate " .. blue(npcRef(2040026)) .. ". ");
	addText("Which of these rocks would you like to teleport to?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	data = selectChoice(choices, choice);
	map, npcId, floorName = data[1], data[2], data[3];

	addText("You can use " .. blue(itemRef(4001020)) .. " to activate " .. blue(npcRef(2040026)) .. ". ");
	addText("Will you teleport to " .. blue(npcRef(npcId)) .. " at the " .. floorName .. " floor?");
	answer = askYesNo();

	if answer == answer_yes then
		giveItem(4001020, -1);
		setMap(map, "go00");
	end
else
	addText("There's a rock that will enable you to teleport to " .. blue(npcRef(2040025) .. " or " .. npcRef(2040027)) .. ", but it cannot be activated without the scroll.");
	sendOk();
end