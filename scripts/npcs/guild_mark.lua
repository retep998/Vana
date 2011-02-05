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
-- Guild Emblem Changer NPC

if getGuildRank() ~= 1 then
	addText("oh... You are not a guild master. Guild Emblem can be made, deleted or modified, only by #rGuild Master#k...");
	sendNext();
else
	addText("Hi? My name is #bLea#k. I am in charge of #bGuild Emblem#k.\r\n");
	addText("#b#L0#I'd like to register a guild emblem.#l\r\n");
	addText("#L1#I'd like to delete a guild emblem.#l");
	reaction = askChoice();

	if reaction == 0 then
		if hasEmblem() == true then
			addText("Guild Emblem has already been made. Please delete the guild emblem, first and make it, again.");
			sendNext();
		else
			addText("You need #r5,000,000 Mesos#k to make a guild emblem. To explain it more, guild emblem is an unique pattern for each guild. It will appear right next to the guild name in the game\r\nSo are you going to make a guild emblem?");
			choice = askYesNo();

			if choice == 1 then
				if getMesos() < 5000000 then
					addText("Oh... You don't have enough mesos to create an emblem...");
					sendOK();
				else	
					sendChangeGuildEmblem();
				end
			else
				addText("Oh... okay... A guild emblem would make the guild more unified. Do you need more time for preparing for the guild emblem? Please come back to me when you are ready...");
				sendOK();
			end
		end
	else
		if not hasEmblem() then
			addText("huh? weird... you don't have a guild emblem to delete...");
			sendOK();
		else
			addText("If you delete the current guild emblem, you can make a new guild emblem. You will need #r1,000,000 Mesos#k to delete a guild emblem. Would you like to do it?");
			choice = askYesNo();

			if choice == 1 then
				if getMesos() < 1000000 then 
					addText("You don't have enough Mesos for deleting a guild emblem. You need #b1,000,000 Mesos#k to delete a guild emblem.");
					sendNext();
				else
					removeEmblem();
				end
			else
				addText("Please come back to me, when you are ready.");
				sendOK();
			end
		end
	end
end
