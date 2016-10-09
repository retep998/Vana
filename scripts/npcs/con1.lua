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
-- Konpei (warps to Showa Hideout)

dofile("scripts/utils/npcHelper.lua");

choices = {
	makeChoiceHandler("Gather up some information on the hideout.  ", function()
		addText("I can take you to the hideout, but the place is infested with thugs looking for trouble. ");
		addText("You'll need to be both incredibly strong and brave to enter the premise. ");
		addText("At the hideaway, you'll find the Boss that controls all the other bosses around this area. ");
		addText("It's easy to get to the hideout, but the room on the top floor of the place can only be entered ONCE a day. ");
		addText("The Boss's Room is not a place to mess around. ");
		addText("I suggest you don't stay there for too long; you'll need to swiftly take care of the business once inside. ");
		addText("The boss himself is a difficult foe, but you'll run into some incredibly powerful enemies on your way to meeting the boss! ");
		addText("It ain't going to be easy. ");
		sendNext();
	end),
	makeChoiceHandler("Take me to the hideout.", function()
		addText("Oh, the brave one. ");
		addText("I've been awaiting your arrival. ");
		addText("If these thugs are left unchecked, there's no telling what going to happen in this neighborhood. ");
		addText("Before that happens, I hope you take care of all of them and beat the boss, who resides on the 5th floor. ");
		addText("You'll need to be on alert at all times, since the boss is too tough for even the wisemen to handle. ");
		addText("Looking at your eyes, however, I can see that eye of the tiger, the eyes that tell me you can do this. ");
		addText("Let's go!");
		sendNext();

		setMap(801040000);
	end),
	makeChoiceHandler("Nothing. ", function()
		addText("I'm a busy person! ");
		addText("Leave me alone if that's all you need!");
		sendOk();
	end),
};

addText("What do you want from me? \r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);