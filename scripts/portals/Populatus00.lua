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
-- Door to Origin of Clocktower

dofile("scripts/lua_functions/bossHelper.lua");

if not isPapChannel() then
	channels = getPapChannels();
	if #channels == 0 then
		showMessage("You may not battle Papulatus at this time.", m_red);
	else
		showMessage("You may only battle Papulatus on " .. getChannelString(channels) .. ".", m_red);
	end
	return;
end

if getItemAmount(4031172) > 0 then -- Medal
	if getNumPlayers(220080001) < 12 then
		if isInstance("papulatus") == false then
			x = getMaxPapBattles();
			if x == 0 then
				showMessage("You may not battle Papulatus at this time.", m_red);
			else
				if enterBoss("Papulatus", x) then
					playPortalSE();
					setMap(220080001, "st00");
				else
					showMessage("You can only enter The Origin of Clocktower " .. x .. " " .. timeString(x) .. " a day.", m_red);
				end
			end
		else
			showMessage("The battle with Papulatus has already begun, so you cannot enter this place.", m_red);
		end
	else
		showMessage("The room is already in full capacity with people battling against Papulatus.", m_red);
	end
end