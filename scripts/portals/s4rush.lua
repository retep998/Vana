--[[
Copyright (C) 2008-2012 Vana Development Team

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
-- Portal to 4th job warrior quest

dofile("scripts/lua_functions/jobFunctions.lua");

if isQuestActive(6110) then
	if getPartyId() == 0 then
		showMessage("You don't have a  party. You can challenge with party.", env_redMessage);
	elseif isPartyLeader() then
		members = getAllPartyPlayerIds();
		if #members ~= 2 then
			showMessage("You can make a quest when you have a party with two. Please make your party with two members.", env_redMessage);
		else
			if not isPartyInLevelRange(120, 200) then
				showMessage("There is a character among your party whose level is not eligible. You should be level 120 above. Please adjust level.", env_redMessage);
			else
				memberCount = 0
				mapId = getMap();
				for i = 1, #members do
					member = members[i];
					if setPlayer(member) then
						if getJobLine() == 1 and getJobProgression() == 2 and getMap() == mapId then
							-- Only want 4th job warriors
							memberCount = memberCount + 1;
						end
						revertPlayer();
					end
				end
				if memberCount ~= 2 then
					showMessage("You can't enter. Your party member's job is not Warrior or Your party doesn't consist of two members.", env_redMessage);
				else
					if not isInstance("rush4th") then
						createInstance("rush4th", 20 * 60, true);
						for i = 1, #members do
							addInstancePlayer(members[i]);
						end
						addInstanceParty(getPartyId());
						playPortalSe();
						warpParty(910500100);
					else
						showMessage("Other parties are challenging on quest clear now. Try again later.", env_redMessage);
					end
				end
			end
		end
	else
		showMessage("Party leader consisting of two Warriors can decide to enter.", env_redMessage);
	end
else
	showMessage("You can't enter sealed place.", env_redMessage);
end