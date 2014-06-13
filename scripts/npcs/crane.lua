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
-- Hak (Herb Town, Mu Lung, Orbis)

dofile("scripts/utils/npcHelper.lua");

map = getMap();
if map == 251000000 then
	addText("Hello there? ");
	addText("I'm the crane that flies from " .. blue("Orbis") .. " to " .. blue("Mu Lung") .. " and back. ");
	addText("I fly around all the time, so I figured, why not make some money by taking travelers like you along for a small fee? ");
	addText("It's good business for me. ");
	addText("Anyway, what do you think? ");
	addText("Do you want to fly to " .. blue("Mu Lung") .. " right now? ");
	addText("I only charge " .. blue("1500 mesos") .. ".");
	answer = askYesNo();

	if answer == answer_yes then
		if giveMesos(-1500) then
			setMap(250000100);
		else
			addText("Are you sure you have enough mesos?");
			sendNext();
		end
	else
		addText("OK. If you ever change your mind, please let me know.");
		sendNext();
	end
elseif map == 250000100 then
	choices = {
		makeChoiceHandler(" Orbis(6000 mesos)", function()
			if isInstance("hakToOrbis") then
				addText("Someone else is on the way to Orbis right now. ");
				addText("Talk to me in a little bit.");
				sendNext();
			else
				if giveMesos(-6000) then
					createInstance("hakToOrbis", 60, true);
					addInstancePlayer(getId());
					setMap(200090310);
				else
					addText("Are you sure you have enough mesos?");
					sendNext();
				end
			end
		end),
		makeChoiceHandler(" Herb Town(1500 mesos)", function()
			addText("Will you move to " .. blue(mapRef(251000000))  .. " now? ");
			addText("If you have " .. blue("1500mesos") .. ", I'll take you there right now.");
			answer = askYesNo();

			if answer == answer_yes then
				if giveMesos(-1500) then
					setMap(251000000);
				else
					addText("Are you sure you have enough mesos?");
					sendNext();
				end
			else
				addText("OK. If you ever change your mind, please let me know.");
				sendNext();
			end
		end),
	};

	addText("Hello there? ");
	addText("I'm the crane that flies from " .. blue("Mu Lung") .. " to " .. blue("Orbis") .. " and back. ");
	addText("I fly around all the time, so I figured, why not make some money by taking travelers like you along for a small fee? ");
	addText("It's good business for me. ");
	addText("Anyway, what do you think? \r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
elseif map == 200000141 then
	addText("Hello there? ");
	addText("I'm the crane that flies from " .. blue("Orbis") .. " to " .. blue("Mu Lung") .. " and back. ");
	addText("I fly around all the time, so I figured, why not make some money by taking travelers like you along for a small fee? ");
	addText("It's good business for me. ");
	addText("Anyway, what do you think? \r\n");
	addText(blue(choiceRef(" Mu Lung(6000 mesos)")) .. "\r\n");
	choice = askChoice();

	if isInstance("hakToMuLung") then
		addText("Someone else is on the way to Mu Lung right now. ");
		addText("Talk to me in a little bit.");
		sendNext();
	else
		if giveMesos(-6000) then
			createInstance("hakToMuLung", 60, true);
			addInstancePlayer(getId());
			setMap(200090300);
		else
			addText("Are you sure you have enough mesos?");
			sendNext();
		end
	end
end