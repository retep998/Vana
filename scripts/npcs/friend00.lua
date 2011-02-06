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
-- Mr. Goldstein (Buddy List Admin)

price = 250000;

addText("I hope I can make as much as yesterday ...well, hello! Don't you want to extend your buddy list? You look like someone who'd have a whole lot of friends... well, what do you think? With some money I can make it happen for you. Remember, though, it only applies to one character at a time, so it won't affect any of your other characters on your account. Do you want to do it?");
yes = askYesNo();

if yes == 0 then
	addText("I see... you don't have as many friends as I thought you would. Hahaha, just kidding! Anyway if you feel like changing your mind, please feel free to come back and we'll talk business. If you make a lot of friends, then you know ... hehe ...");
	sendNext();
else
	addText("Alright, good call! It's not that expensive actually. #b250,000 mesos and I'll add 5 more slots to your buddy list#k. And no, I won't be selling them individually. Once you buy it, it's going to be permanently on your buddy list. So if you're one of those that needs more space there, then you might as well do it. What do you think? Will you spend 250,000 mesos for it?");
	yes = askYesNo();
	
	if yes == 0 then
		addText("I see... I don't think you don't have as many friends as I thought you would. If not, you just don't have 250,000 mesos with you right this minute? Anyway, if you ever change your mind, come back and we'll talk business. That is, of course, once you have get some financial relief ... hehe ...");
		sendNext();
	else
		if getMesos() < price or getBuddySlots() == 50 then
			addText("Hey... are you sure you have #b250,000 mesos#k?? If so then check and see if you have extended your buddy list to the max. Even if you pay up, the most you can have on your buddy list is #b50#k.");
			sendNext();
		else
			giveMesos(-price);
			addBuddySlots(5);
			
			addText("Alright! Your buddy list will have 5 extra slots by now. Check and see for it yourself. And if you still need more room on your buddy list, you know who to find. Of course, it isn't going to be for free ... well, so long ...");
			sendOK();
		end
	end
end