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
-- Shane in Ellinia

dofile("scripts/lua_functions/npcHelper.lua");

function validQuest(questId)
	return isQuestActive(questId) or isQuestCompleted(questId);
end

if validQuest(2051) then
	mapId = 101000102;

	if isQuestActive(2051) then
		price = getLevel() * 200;
		
		addText("It's you from the other day. ");
		addText("Did " .. npcRef(1061005) .. " make another request to you? ");
		addText("What? ");
		addText("You need to go in further this time? ");
		addText("Hmm ... it's pretty dangerous in there, but ... alright, for " .. red(price) .. " mesos I'll let you in that deep. ");
		addText("So, do you want to pay your way in?");
		answer = askYesNo();

		if answer == answer_no then
			addText("I see ... but understand that you can't get in here for free.");
			sendNext();
		else
			if giveMesos(-price) then
				setMap(mapId);
			else
				addText("Do you not have enough Mesos? ");
				addText("Will you check whether you have more than " .. red(price) .. " Mesos? ");
				addText("Don't even try to ask me for a discount.");
				sendNext();
			end
		end
	else
		addText("It's you again. ");
		addText("Is " .. npcRef(1061005) .. " busy making ant-aging serum? ");
		addText("Anyway, honestly, I was a little shocked that you were able to get through this place. ");
		addText("For that, I'll let you enter free of charge. ");
		addText("You might be able to get some precious items deep inside...");
		sendNext();

		addText("Oh, by the way... once, " .. npcRef(1032100) .. " of this town had secretly gone inside, and when I caught her, she was so taken aback that she lost her " .. blue(itemRef(1032013)) .. " in there. ");
		addText("I tried to look for it but couldn't figure out where it was... ");
		addText("Do you think you can go in and find it? ");
		addText("Would you like to enter now?");
		answer = askYesNo();

		if answer == answer_no then
			addText("Suit yourself. ");
			addText("I was trying to be considerate for once.");
			sendNext();
		else
			setMap(mapId);
		end
	end
elseif validQuest(2050) then
	mapId = 101000100;

	if isQuestActive(2050) then
		price = getLevel() * 100;

		addText("So you came here at the request of " .. npcRef(1061005) .. " to take the medicinal herb? ");
		addText("Well...I inherited this land from my father and I can't let some stranger in just like that ... ");
		addText("But, with " .. red(price) .. " mesos, it's a whole different story...");
		addText("So, do you want to pay your way in?");
		answer = askYesNo();

		if answer == answer_no then
			addText("I see ... but understand that you can't get in here for free.");
			sendNext();
		else
			if giveMesos(-price) then
				setMap(mapId);
			else
				addText("Lacking mesos by any chance? ");
				addText("Make sure you have more than " .. red(price) .. " mesos on hand. ");
				addText("Don't expect me to give you any discounts.");
				sendNext();
			end
		end
	else
		addText("It's you again. ");
		addText("Is " .. npcRef(1061005) .. " busy making diet pills? ");
		addText("Anyway, honestly, I was a little shocked that you were able to get through this place. ");
		addText("For that, I'll let you enter free of charge. ");
		addText("You might be able to get some precious items deep inside...");
		addText("Would you like to enter now?");
		answer = askYesNo();

		if answer == answer_no then
			addText("Suit yourself. ");
			addText("I was trying to be considerate for once.");
			sendNext();
		else
			setMap(mapId);
		end
	end
else
	addText("You want to go in? ");
	addText("Must have heard that there's a precious medicinal herb in here, huh? ");
	addText("But I can't let some stranger like you who doesn't know that I own this land in. ");
	addText("I'm sorry but I'm afraid that's all there is to it.");
	sendOk();
end