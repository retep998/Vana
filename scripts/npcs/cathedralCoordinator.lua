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
-- Victoria

dofile("scripts/utils/npcHelper.lua");

addText("Before I help you make a reservation for the Cathedral Wedding, I strongly recommend that " .. blue("both you and your partner") .. " need to have at least " .. blue("3 Etc. slots") .. " available. ");
addText("Please check your etc. inventory.");
sendNext();

choices = {
	makeChoiceHandler("How can I get married here?", function()
		addText("To get married in the Cathedral, you'll need " .. red("a Cathedral Wedding Ticket, any Engagement Ring or an Empty Engagement Ring Box and most of all, love") .. ". ");
		addText("Soon as you have them, we'll be happy to assist with your Wedding plans! ");
		addText("If you reserved the Cathedral don't forget to see High Priest John for the Officiator's permission.");
		sendNext();
	end),
	makeChoiceHandler("I'd like to make a Premium Reservation.", function()
		-- TODO FIXME implement reservations
		addText("To make a Reservation, you'll need to be grouped with your fiancee or fiancee... ");
		sendNext();
	end),
	makeChoiceHandler("I'd like to make a Normal Reservation.", function()
		-- TODO FIXME implement reservations
		addText("To make a Reservation, you'll need to be grouped with your fiance... ");
		sendNext();
	end),
	makeChoiceHandler("I have more guests coming, I'd like some more Invitations.", function()
		-- TODO FIXME implement invitations
		addText("To receive some more invitations, you'll need to be grouped with your fiance... ");
		sendNext();
	end),
	makeChoiceHandler("I would like to cancel my wedding reservation.", function()
		addText("If you cancel your wedding reservation, all items and quest information pertaining to weddings will disappear. ");
		addText("Would you really like to cancel?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("You have not yet made a wedding reservation.");
			sendOk();
		else
			-- TODO FIXME implement reservations
		end
	end),
};

addText("And by the way, I must tell you, you look wonderful today! ");
addText("I'm here to help you prepare for your Wedding. ");
addText("I can help you make a Reservation, get additional Invitations, or tell you what you'll need to get married in our Cathedral. ");
addText("What would you like to know?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);