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
-- Thief Job Instructor (Inside)

if getItemAmount(4031013) >= 30 then
	addText("Ohhhhh...you collected all 30 Dark Marbles!! Wasn't it difficult?? Alright. ");
	addText("You've passed the test and for that, I'll reward you #b#t4031012##k. Take that item and go back ");
	addText("to Kerning City.");
	sendNext();
	giveItem(4031013, -getItemAmount(4031013));
	giveItem(4031011, -getItemAmount(4031011));
	giveItem(4031012, 1);
	setMap(103000000);
elseif getItemAmount(4031013) < 30 then 
	addText("What's going on? Doesn't look like you have collected 30 #b#t4031013##k, yet...If you're having ");
	addText("problems with it, then you can leave, come back and try it again. So...wanna give up ");
	addText("and get out of here?");
	yes = askYesNo();

	if yes == 0 then 
		addText("That's right! Stop acting weak and start collecting the marbles. Talk to me when you have ");
		addText("collected 30 #b#t4031013##k.");
		sendNext();
	else
		addText("Really... alright, I'll let you out. Please don't give up, though. You can always try again, ");
		addText("so do not give up. Until then, bye...");
		sendNext();
		setMap(102040000);
	end
end