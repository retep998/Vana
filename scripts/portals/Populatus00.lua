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
-- Door to Origin of Clocktower

if getItemAmount(4031172) > 0 then -- Medal
	if getNumPlayers(220080001) < 12 then
		if isInstance("papulatus") == false then
			local time = getTime();
			local secondsinday = 60 * 60 * 24;
			local entry1 = tonumber(getPlayerVariable("Papulatus1"));
			local entry2 = tonumber(getPlayerVariable("Papulatus2"));
			if entry1 == nil then
				entry1 = 0;
			end
			if entry2 == nil then
				entry2 = 0;
			end
			if (time < (entry1 + secondsinday)) and (time < (entry2 + secondsinday)) then
				showMessage("You can only enter The Origin of Clocktower twice a day.", 5);
			else
				if time > (entry1 + secondsinday) then
					setPlayerVariable("Papulatus1", time);
				else
					setPlayerVariable("Papulatus2", time);
				end
				playPortalSE();
				setMap(220080001, "st00");
			end
		else
			showMessage("The battle with Papulatus has already begun, so you cannot enter this place.", 5);
		end
	else
		showMessage("The room is already in full capacity with people battling against Papulatus.", 5);
	end
end