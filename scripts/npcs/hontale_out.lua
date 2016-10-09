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
-- Crystal of Roots

dofile("scripts/utils/bossHelper.lua");
dofile("scripts/utils/npcHelper.lua");

mapId = getMap();

if mapId == 240050100 or mapId == 240050101 or mapId == 240050102 or mapId == 240050103 or mapId == 240050104 or mapId == 240050105 or mapId == 240050200 or mapId == 240050300 or mapId == 240050310 then
	choices = {
		makeChoiceHandler(" read the words carefully.", function()
			if mapId == 240050100 then
				addText("Only those who have the crystal key can enter the maze room.\r\n");
				addText("Only those left can open the door of maze room.\r\n");
				addText("can get what you want from glittering tree hole.\r\n");
				addText("The key made on the flame shines in the cave.");
			elseif mapId == 240050105 then
				addText("When you find the key made on the flame, you'll see the end of the maze.");
			elseif mapId == 240050200 then
				addText("Darkness is connected to darkness, and light to light.\r\n");
				addText("Your choice always makes results.");
			elseif mapId == 240050300 or mapId == 240050310 then
				addText("The key made in the ice shines the cave.");
			else
				addText("You have to discard to earn something.\r\n");
				addText("You can drop what you wish to drop in the glittering tree hole.");
			end
			sendNext();
		end),
		makeChoiceHandler(" Would you like to give up and get out?", function()
			setMap(240050500, "st00");
		end),
	};

	addText("Words are revealed on the surface of crystal tangled roots.\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
elseif mapId == 240050400 then
	addText("Do you want to go back to " .. mapRef(240050000));
	answer = askYesNo();

	if answer == answer_yes then
		setMap(240050000, "st00");
	else
		addText("Think again and talk to me.");
		sendNext();
	end
elseif mapId == 240050500 then
	addText("The entrance of the cave is reflected on the crystal. ");
	addText("It seems you can get there when touching it.\r\n");
	addText(blue(choiceRef(" touch the crystal")));
	choice = askChoice();

	giveItem(4001087, -getItemAmount(4001087));
	giveItem(4001088, -getItemAmount(4001088));
	giveItem(4001089, -getItemAmount(4001089));
	giveItem(4001090, -getItemAmount(4001090));
	giveItem(4001091, -getItemAmount(4001091));
	giveItem(4001092, -getItemAmount(4001092));
	giveItem(4001093, -getItemAmount(4001093));
	setMap(240050000, "st00");
elseif mapId == 240060000 or mapId == 240060100 or mapId == 240060200 then
	x = getMaxHorntailBattles();
	addText("Do you want to give up squad and quit? ");
	if x ~= -1 then
		addText("You can only enter " .. x .. " " .. timeString(x) .. " a day. ");
		y = getEntryCount("Horntail", x);
		if y < x then
			-- If you quit now, you can enter only once today.
			addText("If you quit now, you can enter only " .. x - y .. " more " .. timeString(x - y));
		else
			addText("When you quit now, you can't enter again");
		end
		addText(" today.");
	end

	answer = askYesNo();
	if answer == answer_yes then
		setMap(240050400, "st00");
	else
		addText("Think again and talk to me.");
		sendNext();
	end
end