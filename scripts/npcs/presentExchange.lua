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
-- Pila Present

dofile("scripts/utils/npcHelper.lua");

-- TODO FIXME implement marriage

choices = {
	makeChoiceHandler("I am about to finish my wedding and want to pick my presents which my friends gave to me.", function()
		addText("You are currently not married.");
		sendNext();
	end),
	makeChoiceHandler("I have an " .. red("Onyx Chest") .. " and want to ask for you to open it.", function()
		addText("I've got some fabulous items ready for you. ");
		addText("Are you ready to pick them out?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("I don't think you have an Onyx Chest that I can open, kid...");
			sendNext();
		else
			addText("Awww, really? ");
			addText("I'm the only one who can open your Onyx Chest! ");
			addText("I will be here and wait for you~");
			sendNext();
		end
	end),
	makeChoiceHandler("I have an " .. red("Onyx Chest for Bride and Groom") .. " and want to ask for you to open it.", function()
		addText("I've got some fabulous items ready for you. ");
		addText("Are you ready to pick them out?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("I don't think you have an " .. red("Onyx Chest for Bride and Groom") .. " that I can open, kid...");
			sendNext();
		else
			addText("Awww, really? ");
			addText("I'm the only one who can open your Onyx Chest! ");
			addText("I will be here and wait for you~");
			sendOk();
		end
	end),
};

addText("How do I help you? \r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);