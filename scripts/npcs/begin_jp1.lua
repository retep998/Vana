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
--

dofile("scripts/utils/npcHelper.lua");

map = getMap();
if map == 0 then
	addText("Welcome to the world of MapleStory. ");
	addText("The purpose of this training camp is to help beginners. ");
	addText("Would you like to enter this training camp? ");
	addText("Some people start their journey without taking the training program. ");
	addText("But I strongly recommend you take the training program first.");
	answer = askYesNo();

	if answer == answer_yes then
		addText("Ok then, I will let you enter the training camp. ");
		addText("Please follow your instructor's lead.");
		sendNext();

		setMap(1);
	else
		addText("Do you really wanted to start your journey right away?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("It seems like you want to start your journey without taking the training program. ");
			addText("Then, I will let you move on the training ground. ");
			addText("Be careful~");
			sendNext();

			setMap(40000);
		else
			addText("Please talk to me again when you finally made your decision.");
			sendNext();
		end
	end
elseif map == 1 then
	addText("This is the image room where your first training program begins. ");
	addText("In this room, you will have an advance look into the job of your choice.");
	sendNext();

	addText("Once you train hard enough, you will be entitled to occupy a job. ");
	addText("You can become a Bowman in Henesys, a Magician in Ellinia, a Warrior in Perion, and a Thief in Kerning City..");
	sendBackOk();
end