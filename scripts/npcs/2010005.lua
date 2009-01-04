--[[
Copyright (C) 2008-2009 Vana Development Team

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
-- Shuri - Tour Guide
-- Teleports to Florina Beach
if state == 0 then
	addText("Have you heard of the beach with a spectacular view of the ocean called #b#m110000000##k, located a bit far from #m200000000#? I can take you there right now for either #b1500 mesos#k, or if you have #b#t4031134##k with you, in which case you'll be in for free.\r\n");
	addText("#b#L0# I'll pay #b1500 mesos#k.#l\r\n");
	addText("#b#L1# I have #b#t4031134##k.#l\r\n");
	addText("#b#L2# What is #b#t4031134##k?#l");
	sendSimple();
elseif state == 1 then
	what = getSelected();
	if what == 0 then
		addText("You want to pay #b1500 mesos#k and leave for #m110000000#? Okay!! Please beware that you may be running into some monsters around there, though, so make sure not to get caught off-guard. Okay, would you like to head over to #m110000000# right now?");
		sendYesNo();
	elseif what == 1 then
		addText("So you have #b#b#t4031134##k? You can always head over to #m110000000# with that Okay!! Please beware that you may be running into some monsters around there, though, so make sure not to get caught off-guard. Okay, would you like to head over to #m110000000# right now?");
		sendYesNo();
	elseif what == 2 then
		addText("You must be curious about #b#t4031134##k. Yeah, I can see that. #t4031134# is an item where as long as you have in possession, you may make your way to #m110000000# for free. It's such a rare item that even we had to buy those, but unfortunately I lost mine a few weeks ago during a long weekend.");
		sendNext();
	end
elseif state == 2 then
	if what == 0 then
		if getSelected() == 1 then
			if getMesos() >= 1500 then
				giveMesos(-1500);
				setPlayerVariable("florina_origin", getMap());
				setMap(110000000);
				endNPC();
			else
				addText("I think you're lacking mesos. There are many ways to gather up some money, you know, like ... selling your armor ... defeating the monsters ... doing quests ... you know what I'm talking about.");
				sendNext();
			end
		else
			addText("You must have some business to take care of here. You must be tired from all that traveling and hunting. Go take some rest, and if you feel like changing your mind, then come talk to me.");
			sendNext();
		end
	elseif what == 1 then
		if getSelected() == 1 then
			if getItemAmount(4031134) >= 1 then
				setPlayerVariable("florina_origin", getMap());
				setMap(110000000);
				endNPC();
			else
				addText("Hmmm... are you sure you have #b#t4031134##k with you? Please double-check, because I can't find it from you.");
				sendNext();
			end
		else
			addText("You must have some business to take care of here. You must be tired from all that traveling and hunting. Go take some rest, and if you feel like changing your mind, then come talk to me.");
			sendNext();
		end
	elseif what == 2 then
		addText("I came back without it, and it just feels awful not having it. Hopefully someone picked it up and put it somewhere safe. Anyway this is my story and who knows, you may be able to pick it up and put it to good use. If you have any questions, feel free to ask.");
		sendBackNext();
	end
else
	endNPC();
end