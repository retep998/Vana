--[[
Copyright (C) 2008-2009 Vana Development Team

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
-- Guild Creation NPC

if getGuildId() == 0 then
	addText("Hey ... are you interested in GUILDS by any chance?");
	sendNext();

	addText("#b#L0#What's a guild?#l\r\n");
	addText("#L1#What do I do to form a guild?#l\r\n");
	addText("#L2#I want to start a guild#l");
	reaction = askChoice();

	if reaction == 0 then
		-- What's a guild?
		addText("A guild is ... you can think of it as a small crew, a crew full of people with similar interests and goals, except it will be officially registered in our Guild Headquarters and be accepted as a valid GUILD.");
		sendNext();
	elseif reaction == 1 then
		-- What do I do to form a guild?
		addText("To make your own guild, you'll need to be at least level 10. You'll also need to have at least 1,500,000 mesos with you. That's the fee you need to pay in order to register your guild.");
		sendNext();
		
		addText("To make a guild you'll need a total of 6 people. Those 6 should be in a same party, and the party leader should come talk to me. Please be aware that the party leader also becomes the Guild Master. Once the Guild Master is assigned, the position remains the same until the Guild breaks up.");
		sendNext();
		
		addText("Once the 6 people are gathered up, you'll need 1,500,000 mesos. This is the fee you pay to register your guild");
		sendNext();
		
		addText("Alright, to register your guild, bring 6 people here~ You can't make one without all 6...Oh, and of course, the 6 cannot be a part of some other guild!!");
		sendNext();
	elseif reaction == 2 then
		-- I want to start a guild
		addText("Alright, now, do you want to make a guild?");
		reaction = askYesNo();
		if reaction == 1 then
			if getPartyID() == 0 then
				addText("I don't care how tough you think you are... In order to form a guild, you need to be in a party of 6. Create a party of 6, and then bring all your party members back here if you are indeed serious about forming a guild.");
				sendNext();
			elseif getPartyCount() < 2 then
				addText("It seems like either you don't have enough members in your party, or some of your members are not here. I need all 6 party members here to register you as a guild. If your party can't even coordinate this simple task, you should think twice about forming a guild.");
				sendNext();
			elseif isPartyLeader() ~= true then
				addText("Please let the party leader talk to me if you want to create a guild.");
				sendNext();
			elseif getPartyCount() >= 2 then
				partyMembers = getAllPartyPlayerIDs();
				stoppedName = "";
				for i = 1, #partyMembers do
					setPlayer(partyMembers[i]);
					if getGuildId() ~= 0 then
						stoppedName = getName();
						break;
					end
				end
				revertPlayer();
				if stoppedName ~= "" then
					addText(stoppedName .. " is already in a guild. You can't make a guild with a partymember that is already in a guild.");
					sendNext();
				else
					addText("Enter the name of your guild and your guild will be created.\r\nThe guild will also be officially registered under our Guild Headquarters, so best of luck to you and your guild!");	
					sendNext();
				
					sendNewGuildWindow();
				end
			end
		end
	end
else	
	addText("Now, how can I help you?\r\n");
	addText("#b#L0#I want to expand my guild#l\r\n");
	addText("#L1#I want to break up my guild#l");
	reaction = askChoice();

	if getGuildRank() ~= 1 then
		addText("Hey, you're not the Guild Master!! This decision can only be made by the Guild Master.");
		sendOK();
	end

	if reaction == 0 then
		capacity = getGuildCapacity();
		if capacity == 100 then
			addText("Your guild seems to have grown quite a bit. I cannot expand your guild any longer...");
			sendNext();
		end
		-- Expanding
		addText("Are you here to expand your guild? Your guild must have grown quite a bit~ To expand your guild, the guild has to be re-registered in our Guiild Headquarters, and that'll require some service fee .....");
		sendNext();
		
		
		-- Lets calculate the increasement fee
		fee = 500000;
		if capacity == 15 then
			fee = fee * 3;
		elseif capacity == 20 then
			fee = fee * 5;
		elseif capacity >= 25 then
			fee = fee * 7;
		end
		
		if capacity == 0 then
		else
			addText("The service fee will only cost you #r" .. fee .. " mesos#k. Would you like to expand your guild?");
			reaction = askYesNo();
			if reaction == 1 then
				if getMesos() >= fee then
					sendIncreaseCapacity();
				else
					addText("Please check again. You'll need to pay the service fee in order to expand your guild and re-register it....");
					sendOK();
				end
			end
		end
		
	elseif reaction == 1 then
		-- Disbanding
		if getAllianceRank() == 1 then
			addText("You need to pass the alliance leadership first before you disband the guild.");
			sendOK();
		end
		addText("Are you sure you want to break up your guild? Really ... just remember, once you break up your guild, that guild will be gone forever. Oh, and one thing. If you want to break up your guild, you'll have to pay a 200,000 meso service fee. Are you sure you still want to do it?");
		reaction = askYesNo();
		
		if reaction == 1 then
			disbandGuild();
		end
	end
end
