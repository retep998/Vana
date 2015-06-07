--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- Shawn, Excavation Site

dofile("scripts/utils/npcHelper.lua");

iterationBreak = 1;

choices = {
	makeChoiceHandler(" What's Sharenian?", function()
		addText("Sharenian was a literate civilization from the past that had control over every area of the Victoria Island. ");
		addText("The Temple of Golem, the Shrine in the deep part of the Dungeon, and other old architectural constructions where no one knows who built it are indeed made during the Sharenian times.");
		sendNext();

		addText("The last king of Sharenian was a gentleman named Sharen III, and apparently he was a very wise and compassionate king. ");
		addText("But one day, the whole kingdom collapsed, and there was no explanation made for it.");
		sendBackNext();
	end),
	makeChoiceHandler(" " .. itemRef(4001024) .. "? What's that?", function()
		addText(itemRef(4001024) .. " is a legendary jewel that brings eternal youth to the one that possesses it. ");
		addText("Ironically, it seems like everyone that had " .. itemRef(4001024) .. " ended up downtrodden, which should explain the downfall of Sharenian.");
		sendNext();
	end),
	makeChoiceHandler(" Guild Quest?", function()
		addText("I've sent groups of the explorers to Sharenian before, but none of them ever came back, which prompted us to start the Guild Quest. ");
		addText("We've been waiting for guilds that are strong enough to take on tough challenges, guilds like yours.");
		sendNext();

		addText("The ultimate goal of this Guild Quest is to explore Sharenian and find " .. itemRef(4001024) .. ". ");
		addText("This is not a task where power solves everything. ");
		addText("Teamwork is more important here.");
		sendBackNext();
	end),
	makeChoiceHandler(" No, I'm fine now.", function()
		addText("Really? ");
		addText("If you have anything else to ask, please feel free to talk to me.");
		sendOk();

		return iterationBreak;
	end),
};

while true do
	if extra == nil then
		extra = 1;
		addText("We, the Union of Guilds, have been trying to decipher 'Emerald Tablet,' a treasured old relic, for a long time. ");
		addText("As a result, we have found out that Sharenian, the mysterious country from the past, lay asleep here. ");
		addText("We also found out that clues of " .. itemRef(4001024) .. ", a legendary, mythical jewelry, may be here at the remains of Sharenian. ");
		addText("This is why the Union of Guilds have opened Guild Quest to ultimately find " .. itemRef(4001024) .. ".\r\n");
	else
		addText("Do you have any other questions?\r\n");
	end

	addText(blue(choiceRef(choices)));
	choice = askChoice();

	if selectChoice(choices, choice) == iterationBreak then
		break;
	end
end