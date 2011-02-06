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
-- Lenario

addText("Hello there! I'm #bLenario#k.\r\n");
addText("#b#L0#Can you please tell me what Guild Union is all about?#l\r\n");
addText("#L1#How do I make a Guild Union?#l\r\n");
addText("#L2#I want to make a Guild Union.#l\r\n");
addText("#L3#I want to add more guilds for the Guild Union.#l\r\n");
addText("#L4#I want to break up the Guild Union.#l");
reaction = askChoice();

if reaction == 0 then
	addText("Guild Union is just as it says, a union of a number of guilds to form a super group. I am in charge of managing these Guild Unions.");
	sendNext();
elseif reaction == 1 then
	addText("To make a Guild Union, 2 Guild Masters need to be in a party. The leader of this party will be assigned as the Guild Union Master.");
	sendNext();

	addText("If the 2 Guild Masters are present, then I need 5,000,000 mesos. It's the fee you'll need to pay in order to register for a Guild Union.");
	sendBackNext();

	addText("Oh, and one more thing! It's obvious, but you can't create a new Guild Union if you are already a member of another one!");
	sendBackNext();
elseif reaction == 2 then
	if getAllianceID() ~= 0 then
		addText("You cannot form a Guild Union if you are already affiliated with a different Union.");
		sendNext();
	elseif getPartyID() == 0 then
		addText("You may not create an alliance until you get into a party of 2 people.");
		sendNext();
	elseif not isPartyLeader() then
		addText("Please let the partyleader talk to me if you want to create an union.");
		sendNext();
	elseif getGuildRank() ~= 1 then
		addText("Only the Guild Master can form a Guild Union.");
		sendNext();
	elseif getPartyCount() ~= 2 then
		addText("Please make sure there are only 2 players in your party.");
		sendNext();
	elseif getPartyMapCount(200000301) ~= 2 then
		addText("Get your other party member on the same map please.");
		sendNext();
	else
		addText("Oh, are you interested in forming a Guild Union?");
		reaction = askYesNo();

		if reaction == 1 then
			addText("Now please enter the name of your new Guild Union. (max. 12 letters)");
			allianceName = askText(4, 12);

			addText("Will " .. allianceName .. " be the name of your Guild Union?");
			reaction = askAcceptDecline();
			if reaction == 1 then
				createAlliance(allianceName);
				addText("You have successfully formed a Guild Union.");
				sendOK();
			end
		end
	end
elseif reaction == 3 then
	if getAllianceRank() ~= 1 then
		addText("Only the Guild Union Master can expand the number of guilds in the Union.");
		sendNext();
	else
		addText("You may currently have up to " .. getAllianceCapacity() .. " guilds in Union. In order to add 1, it'll cost you an additional 1,000,000 mesos.");
		sendNext();
		
		addText("Would you like to spend 1 million mesos to add another guild to your Union?");
		reaction = askYesNo();
		if reaction == 1 then
			if getMesos() < 1000000 then
				addText("You do not have enough mesos to process this.");
				sendNext();
			else
				increaseAllianceCapacity();
				addText("You may now accept up to " .. getAllianceCapacity() + 1 .. " guilds in your Union.");
				sendOK();
			end
		else
			addText("If you want to expand your Guild Union, then let me know.");
			sendNext();
		end
	end
elseif reaction == 4 then
	if getAllianceRank() ~= 1 then
		addText("Only the Guild Union Master may disband the Guild Union.");
		sendNext();
	else
		addText("Are you sure you want to disband your Guild Union?");
		reaction = askAcceptDecline();
		if reaction == 1 then
			disbandAlliance();
			addText("Your Guild Union has been disbanded.");
			sendOK();
		else
			addText("If you want to disband your Guild Union, then please let me know");
			sendNext();
		end
	end
end
