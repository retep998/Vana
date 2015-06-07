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
-- Shanks

dofile("scripts/utils/npcHelper.lua");

if getItemAmount(4031801) > 0 then
	lucas = true;
end

addText("Take this ship and you'll head off to a bigger continent.");
addText(bold(" For 150 mesos") .. ", I'll take you to " .. blue("Victoria Island") .. ". ");
addText("The thing is, once you leave this place, you can't ever come back. ");
addText("What do you think? Do you want to go to Victoria Island?");
answer = askYesNo();

if answer == answer_yes then
	if getLevel() >= 7 then
		if lucas then
			addText("Okay, now give me 150 mesos... ");
			addText("Hey, what's that? ");
			addText("Is that the recommendation letter from Lucas, the chief of Amherst? ");
			addText("Hey, you should have told me you had this. ");
			addText("I, Shanks, recognize greatness when I see one, and since you have been recommended by Lucas, I see that you have a great, great potential as an adventurer. ");
			addText("No way would I charge you for this trip!");
			sendNext();

			addText("Since you have the recommendation letter, I won't charge you for this. ");
			addText("Alright, buckle up, because we're going to head to Victoria Island right now, and it might get a bit turbulent!!");
			sendBackNext();

			giveItem(4031801, -1);
			setMap(104000000);
		elseif getMesos() >= 150 then
			addText("Bored of this place? ");
			addText("Here... Give me 150 mesos first...");
			sendNext();

			addText("Awesome! " .. bold("150 mesos") .. " accepted! ");
			addText("Alright, off to Victoria Island!");
			sendNext();

			giveMesos(-150);
			setMap(104000000);
		else
			addText("What? ");
			addText("You're telling me you wanted to go without any money? ");
			addText("You're one weirdo...");
			sendBackNext();
		end
	else
		addText("Let's see... ");
		addText("I don't think you are strong enough. ");
		addText("You'll have to be at least " .. blue("Level 7") .. " to go to Victoria Island.");
		sendNext();
	end
else
	addText("Hmm... I guess you still have things to do here?");
	sendNext();
end