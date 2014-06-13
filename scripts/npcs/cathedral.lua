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
-- Assistant Nicole

dofile("scripts/utils/npcHelper.lua");

choices = {
	makeChoiceHandler("I am now ready to get Married in cathedral.", function()
		-- TODO FIXME implement weddings
		addText("You need to be in " .. blue("a 2-person party with your fiancee or fiance") .. " to get married.");
		sendNext();
	end),
	makeChoiceHandler("I am invited to the wedding!", function()
		-- TODO FIXME implement weddings
		addText("Greetings! ");
		addText("I can tell that you're a guest of the Bride and Groom, would you like to enter the Cathedral?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("I apologize, but the Wedding hasn't started yet. ");
			addText("When it does, I'll be sure to let you in. ");
			sendNext();
		else
			addText("Well, it looks like this isn't your cup of tea, please stand aside and let others enter. ");
			sendNext();
		end
	end),
};

addText("I can guide you to the Wedding. Which one suits you?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);