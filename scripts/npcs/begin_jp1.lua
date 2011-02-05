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
-- 

map = getMap();
if map == 0 then
	addText("Welcome to the world of MapleStory. The purpose of this training camp is to help beginners. Would you like to enter this training camp? Some people start their journey without taking the training program. But I strongly recommend you take the training program first.");
	decision = askYesNo();
	
	if decision == 1 then
		addText("Ok then, I will let you enter the training camp. Please follow your instructor's lead.");
		sendNext();
		
		setMap(1);
	else
		addText("Do you really wanted to start your journey right away?");
		skip = askYesNo();
		
		if skip == 1 then
			addText("It seems like you want to start your journey without taking the training program. Then, I will let you move on the training ground. Be careful~");
			sendNext();
			
			setMap(40000);
		else
			addText("Please talk to me again when you finally made your decision.");
			sendNext();
		end
	end
elseif map == 1 then
	addText("This is the image room where your first training program begins. In this room, you will have an advance look into the job of your choice.");
	sendNext();
	
	addText("Once you train hard enough, you will be entitled to occupy a job. You can become a Bowman in Henesys, a Magician in Ellinia, a Warrior in Perion, and a Thief in Kerning City..");
	sendBackOK();
end
