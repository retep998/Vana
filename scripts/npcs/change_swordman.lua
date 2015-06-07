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
-- Warrior Job Instructor (Outside)

dofile("scripts/utils/npcHelper.lua");

if getItemAmount(4031008) > 0 and getItemAmount(4031013) == 0 then
	addText("Hmmm...it is definitely the letter from " .. blue(npcRef(1022000)) .. "...so you came all the way here to take the test and make the 2nd job advancement as the warrior. ");
	addText("Alright, I'll explain the test to you. ");
	addText("Don't sweat it too much, it's not that complicated.");
	sendNext();

	addText("I'll send you to a hidden map. ");
	addText("You'll see monsters not normally seen in normal fields. ");
	addText("They look the same like the regular ones, but with a totally different attitude. ");
	addText("They neither boost your experience level nor provide you with item.");
	sendBackNext();

	addText("You'll be able to acquire a marble called " .. blue(itemRef(4031013)) .. " while knocking down those monsters. ");
	addText("It is a special marble made out of their sinister, evil minds. ");
	addText("Collect 30 of those, then go talk to a colleague of mine in there. ");
	addText("That's how you pass the test.");
	sendBackNext();

	addText("Once you go inside, you can't leave until you take care of your mission. ");
	addText("If you die, your experience level will decrease...so you better really buckle up and get ready...well, do you want to go for it now?");
	answer = askYesNo();

	if answer == answer_no then
		addText("You don't seem too prepared for this. ");
		addText("Find me when you ARE ready. ");
		addText("There are neither portals or stores inside, so you better get 100% ready for it.");
		sendNext();
	else
		addText("Alright I'll let you in! ");
		addText("Defeat the monsters inside, collect 30 Dark Marbles, then strike up a conversation with a colleague of mine inside. ");
		addText("He'll give you the " .. blue(itemRef(4031013)) .. ", the proof that you've passed the test. ");
		addText("Best of luck to you.");
		sendNext();

		setMap(108000300);
	end
elseif getItemAmount(4031008) > 0 and getItemAmount(4031013) > 0 then
	addText("So you've given up in the middle of this before. ");
	addText("Don't worry about it, because you can always retake the test. ");
	addText("Now...do you want to go back in and try again?");
	answer = askYesNo();

	if answer == answer_no then
		addText("You don't seem too prepared for this. ");
		addText("Find me when you ARE ready. ");
		addText("There are neither portals or stores inside, so you better get 100% ready for it.");
		sendNext();
	else
		addText("Alright! I'll let you in! ");
		addText("Sorry to say this, but I have to take away all your marbles beforehand. ");
		addText("Defeat the monsters inside, collect 30 Dark Marbles, then strike up a conversation with a colleague of mine inside. ");
		addText("He'll give you the " .. blue(itemRef(4031013)) .. ", the proof that you've passed the test. ");
		addText("Best of luck to you.");
		sendNext();

		giveItem(4031013, -getItemAmount(4031013));
		setMap(108000300);
	end
elseif getJob() == 100 and getLevel() >= 30 then
	addText("Do you want to be a stronger warrior? ");
	addText("Let me take care of that for you, then. ");
	addText("You look definitely qualified for it. ");
	addText("For now, go see " .. blue(npcRef(1022000)) .. " of Perion first.");
	sendNext();
else
	-- TODO FIXME text?
end