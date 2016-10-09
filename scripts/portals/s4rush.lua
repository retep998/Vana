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
-- Portal to 4th job warrior quest

dofile("scripts/utils/jobHelper.lua");

if isQuestActive(6110) then
	gm = isGm()
	if not (gm or getPartyId() ~= 0) then
		showMessage("You don't have a  party. You can challenge with party.", msg_red);
	elseif gm or isPartyLeader() then
		members = getAllPartyPlayerIds();
		if not (gm or #members == 2) then
			showMessage("You can make a quest when you have a party with two. Please make your party with two members.", msg_red);
		else
			if not (gm or isPartyInLevelRange(120, 200)) then
				showMessage("There is a character among your party whose level is not eligible. You should be level 120 above. Please adjust level.", msg_red);
			else
				memberCount = 0
				mapId = getMap();
				for i = 1, #members do
					member = members[i];
					if setPlayer(member) then
						if getJobLine() == line_warrior and getJobProgression() == progression_fourth and getMap() == mapId then
							memberCount = memberCount + 1;
						end
						revertPlayer();
					end
				end
				if not (gm or memberCount == 2) then
					showMessage("You can't enter. Your party member's job is not Warrior or Your party doesn't consist of two members.", msg_red);
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
						showMessage("Other parties are challenging on quest clear now. Try again later.", msg_red);
					end
				end
			end
		end
	else
		showMessage("Party leader consisting of two Warriors can decide to enter.", msg_red);
	end
else
	showMessage("You can't enter sealed place.", msg_red);
end