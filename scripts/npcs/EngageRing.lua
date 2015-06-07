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
-- Moony

dofile("scripts/utils/npcHelper.lua");

choices = {
	makeChoiceHandler("I would like to make an engagement ring for my lover.", function()
		-- TODO FIXME implement marriage
		addText("Looks like you're not quite done. ");
		addText("If you want that ring, you'd better hurry and get me the following all materials to make an engagement ring. ");
		addText("I need you to bring me " .. blue("4 Proof of Love") .. " from Nana.");
		sendNext();

	end),
	makeChoiceHandler("I want an annulment.", function()
		-- TODO FIXME implement marriage
		addText("It looks like you haven't married yet. ");
		addText("I don't have anything to talk to you about this.");
		sendOk();
	end),
};

addText("Have you found true love? ");
addText("If so, I can make you a ring worthy of your devotion...\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);