--[[
Copyright (C) 2008-2011 Vana Development Team

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

if (getItemAmount(4031008) > 0 and getItemAmount(4031013) == 0) then	
	addText("Hmmm...it is definitely the letter from #b#p1022000##k...so you came all the way here to take ");
	addText("the test and make the 2nd job advancement as the warrior. Alright, I'll explain the test to you. ");
	addText("Don't sweat it too much, it's not that complicated.");
	sendNext();

	addText("I'll send you to a hidden map. You'll see monsters you don't normally see. They look the same like ");
	addText("the regular ones, but with a totally different attitude. They neither boost your experience level ");
	addText("nor provide you with item.");
	sendBackNext();

	addText("You'll be able to acquire a marble called #b#t4031013##k while knocking down those monsters. It is ");
	addText("a special marble made out of their sinister, evil minds. Collect 30 of those, then go talk to a ");
	addText("colleague of mine in there. That's how you pass the test.");
	sendBackNext();

	addText("Once you go inside, you can't leave until you take care of your mission. If you die, your experience ");
	addText("level will decrease...so you better really buckle up and get ready...well, do you want to go for it now?");
	yes = askYesNo();

	if yes == 0 then
		addText("You don't seem too prepared for this. Find me when you ARE ready. There are neither portals ");
		addText("or stores inside, so you better get 100% ready for it.");
		sendNext();
	else
		addText("Alright I'll let you in! Defeat the monsters inside, collect 30 Dark Marbles, then strike up a ");
		addText("conversation with a colleague of mine inside. He'll give you the #b#t4031013##k, the ");
		addText("proof that you've passed the test. Best of luck to you.");
		sendNext();
		setMap(108000300);
	end
elseif (getItemAmount(4031008) > 0 and getItemAmount(4031013) > 0) then
	addText("So you've given up in the middle of this before. Don't worry about it, because you can always retake the ");
	addText("test. Now...do you want to go back in and try again?");
	yes = askYesNo();

	if yes == 0 then
		addText("You don't seem too prepared for this. Find me when you ARE ready. There are neither portals ");
		addText("or stores inside, so you better get 100% ready for it.");
		sendNext();
	else
		addText("Alright! I'll let you in! Sorry to say this, but I have to take away all your marbles beforehand.");
		addText(" Defeat the monsters inside, collect 30 Dark Marbles, then strike up a ");
		addText("conversation with a colleague of mine inside. He'll give you the #b#t4031013##k, the ");
		addText("proof that you've passed the test. Best of luck to you.");
		sendNext();

		giveItem(4031013, -getItemAmount(4031013));
		setMap(108000300);
	end
elseif (getJob() == 100 and getLevel() >= 30) then
	addText("Do you want to be a stronger warrior? Let me take care of that for you, then. You look definitely ");
	addText("qualified for it. For now, go see #b#p1022000##k of Perion first.");
	sendNext();
end