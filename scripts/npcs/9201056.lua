--[[
Copyright (C) 2008 Vana Development Team

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
-- New Leaf City/Phantom Forest - NLC Taxi 
if getMap() == 600000000 then
	if state == 0 then
		addText("Hey, there. Want to take a trip deeper into the Masterian wilderness? A lot of this continent is still quite unknown and untamed... so there's still not much in the way of roads. Good thing we've got this baby... We can go offroading, and in style too!");
		sendNext();
	elseif state == 1 then
		addText("Right now, I can drive you to the #bPhantom Forest#k. The old #bPrendergast Mansion#k is located there. Some people say the place is haunted!");
		sendBackNext();
	elseif state == 2 then
		addText("What do you say...? Want to head over there?");
		sendYesNo();
	elseif state == 3 then
		if getSelected() == 1 then
			addText("Alright! Buckle your seat belt, and let's head to the Mansion!\r\nIt's going to get bumpy!");
			sendNext();
		else
			addText("Really? I don't blame you... Sounds like a pretty scary place to me too! If you change your mind, I'll be right here.");
			sendOK();
		end
	elseif state == 4 then
		if getSelected() == 1 then
			setMap(682000000);
		end
		endNPC();
	end
elseif getMap() == 682000000 then
	if state == 0 then
		addText("Hey, there. Hope you had fun here! Ready to head back to #bNew Leaf City#k?");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			addText("Back to civilization it is! Hop in and get comfortable back there... We'll have you back to the city in a jiffy!");
			sendNext();
		else
			addText("Oh, you want to stay and look around some more? That's understandable. If you wish to go back to #bNew Leaf City#k, you know who to talk to!");
			sendOK();
		end
	elseif state == 2 then
		if getSelected() == 1 then
			setMap(600000000);
		end
		endNPC();
	end
end
