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
-- Flo (takes you to elemental Thanatos dimension)

dofile("scripts/lua_functions/jobFunctions.lua");

item = 0;

if (isQuestCompleted(6226) and isQuestActive(6225)) then
	item = 4031470;
elseif (isQuestCompleted(6316) and isQuestActive(6315)) then
	item = 4031469;
end

if item ~= 0 then
	if getPartyId() == 0 then
		addText("You don't have a  party. You can challenge with party.");
		sendOk();
	elseif isPartyLeader() then
		if getItemAmount(item) > 0 then
			addText("You already have #b#t" .. item .. "##k. You don't have to enter Tanatos's room.");
			sendOk();
		else
			members = getAllPartyPlayerIds();
			if #members ~= 2 then
				addText("You can make a quest when you have a party with two. Please make your party with two members.");
				sendOk();
			else
				if not isPartyInLevelRange(120, 200) then
					addText("There is a character among your party whose level is not eligible. You should be level 120 above. Please adjust level.");
					sendOk();
				else
					memberCount = 0
					mapId = getMap();
					for i = 1, #members do
						member = members[i];
						if setPlayer(member) then
							if getJobLine() == 2 and getJobTrack() ~= 3 and getJobProgression() == 2 and getMap() == mapId then -- Only want IL/FP Arch Mages
								memberCount = memberCount + 1;
							end
							revertPlayer();
						end
					end
					if memberCount ~= 2 then
						addText("You can't enter. Your party member's job is not Arch Mage or Your party doesn't consist of two members.");
						sendOk();
					else
						if not isInstance("elementalSummon4th") then
							createInstance("elementalSummon4th", 20 * 60, true);
							for i = 1, #members do
								addInstancePlayer(members[i]);
							end
							addInstanceParty(getPartyId());
							warpParty(922020100);
						else
							addText("Other parties are challenging on quest clear now. Try again later.");
							sendOk();
						end
					end
				end
			end
		end
	else
		addText("Party leader can't apply for entering.");
		sendOk();
	end
elseif getJob() == 212 or getJob() == 222 then
	addText("I don't understand you.");
	sendOk();
end