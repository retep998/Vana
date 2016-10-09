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
-- Door to Origin of Clocktower

dofile("scripts/utils/bossHelper.lua");

if not isGm() and not isPapulatusChannel() then
	channels = getPapulatusChannels();
	if #channels == 0 then
		showMessage("You may not battle Papulatus at this time.", msg_red);
	else
		showMessage("You may only battle Papulatus on " .. getChannelString(channels) .. ".", msg_red);
	end
	return;
end

if isGm() or getItemAmount(4031172) > 0 then -- Medal
	if isGm() or getNumPlayers(220080001) < 12 then
		if isGm() or isInstance("papulatus") == false then
			x = getMaxPapulatusBattles();
			if not isGm() or x == 0 then
				showMessage("You may not battle Papulatus at this time.", msg_red);
			else
				if isGm() or enterBoss("Papulatus", x) then
					playPortalSe();
					setMap(220080001, "st00");
				else
					showMessage("You can only enter The Origin of Clocktower " .. x .. " " .. timeString(x) .. " a day.", msg_red);
				end
			end
		else
			showMessage("The battle with Papulatus has already begun, so you cannot enter this place.", msg_red);
		end
	else
		showMessage("The room is already in full capacity with people battling against Papulatus.", msg_red);
	end
end