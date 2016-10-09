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
-- Robert Holly (Buddy List Admin)

dofile("scripts/utils/npcHelper.lua");

price = 240000;

addText("I hope I can make as much as yesterday ...well, hello! ");
addText("Don't you want to extend your buddy list? ");
addText("You look like someone who'd have a whole lot of friends... well, what do you think? ");
addText("With some money I can make it happen for you. ");
addText("Remember, though, it only applies to one character at a time, so it won't affect any of your other characters on your account. ");
addText("Do you want to do it?");
answer = askYesNo();

if answer == answer_no then
	addText("I see... you don't have as many friends as I thought you would. ");
	addText("Hahaha, just kidding! ");
	addText("Anyway if you feel like changing your mind, please feel free to come back and we'll talk business. ");
	addText("If you make a lot of friends, then you know ... hehe ...");
	sendNext();
else
	addText("Alright, good call! ");
	addText("It's not that expensive actually. ");
	addText("For a special Ludibrium discount, " .. blue("240,000 mesos and I'll add 5 more slots to your buddy list") .. ". ");
	addText("And no, I won't be selling them individually. ");
	addText("Once you buy it, it's going to be permanently on your buddy list. ");
	addText("So if you're one of those that needs more space there, then you might as well do it. ");
	addText("What do you think? ");
	addText("Will you spend 240,000 mesos for it?");
	answer = askYesNo();

	if answer == answer_no then
		addText("I see... I don't think you don't have as many friends as I thought you would. ");
		addText("If not, you just don't have 240,000 mesos with you right this minute? ");
		addText("Anyway, if you ever change your mind, come back and we'll talk business. ");
		addText("That is, of course, once you have get some financial relief ... hehe ...");
		sendNext();
	else
		if getMesos() < price or getBuddySlots() == 50 then
			addText("Hey... are you sure you have " .. blue("240,000 mesos") .. "?? ");
			addText("If so then check and see if you have extended your buddy list to the max. ");
			addText("Even if you pay up, the most you can have on your buddy list is " .. blue("50") .. ".");
			sendNext();
		else
			giveMesos(-price);
			addBuddySlots(5);

			addText("Alright! ");
			addText("Your buddy list will have 5 extra slots by now. ");
			addText("Check and see for it yourself. ");
			addText("And if you still need more room on your buddy list, you know who to find. ");
			addText("Of course, it isn't going to be for free ... well, so long ...");
			sendOk();
		end
	end
end