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
-- Mu Lung Dojo Bulletin Board

dofile("scripts/utils/npcHelper.lua");

choices = {
	makeChoiceHandler("Challenge the Mu Lung Dojo.", function()
		addText(blue("(Once I had placed my hands on the bulletin board, a mysterious energy began to envelop me.)") .. "\r\n\r\n");
		addText("Would you like to go to Mu Lung Dojo?");
		answer = askYesNo();

		if answer == answer_yes then
			setPlayerVariable("dojo_origin", getMap());
			setMap(925020000, "out00");
		else
			addText(blue("(As I took my hand off the bulletin board, the mysterious energy that was covering me disappeared as well.)"));
			sendNext();
		end
	end),
	makeChoiceHandler("Read the notice in more detail.", function()
		addText(bold("< Notice : Take the challenge! >") .. "\r\n");
		addText("My name is Mu Gong, the owner of the My Lung Dojo. ");
		addText("Since long ago, I have been training in Mu Lung to the point where my skills have now reached the pinnacle. ");
		addText("Starting today, I will take on any and all applicants for Mu Lung Dojo. ");
		addText("The rights to the Mu Lung Dojo will be given only to the strongest person.\r\n");
		addText("If there is anyone who wishes to learn from me, come take the challenge any time! ");
		addText("If there is anyone who wishes to challenge me, you're welcome as well. ");
		addText("I will make you fully aware of your own weakness.");
		sendNext();

		addText("PS:You can challenge me on your own. ");
		addText("But if you don't have that kind of courage, go ahead and call all your friends.");
		sendBackNext();
	end),
};

addText(bold("< Notice >") .. "\r\n");
addText("If there is anyone who has the courage to challenge the Mu Lung Dojo, come to the Mu Lung Dojo.  ");
addText("- Mu Gong -\r\n\r\n\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);