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
-- Pianus portal

dofile("scripts/lua_functions/bossHelper.lua");

if not isPianusChannel() then
	channels = getPianusChannels();
	if #channels == 0 then
		showMessage("You may not battle Pianus at this time.", 5);
	else
		showMessage("You may only battle Pianus on " .. getChannelString(channels) .. ".", 5);
	end
	return;
end

x = getMaxPianusBattles();
if x == 0 then
	showMessage("You may not battle Pianus at this time.", 5);
else
	if getNumPlayers(230040420) < 10 then
		if enterBoss("Pianus", x) then
			playPortalSE();
			setMap(230040420, "out00");
		else
			showMessage("You may only enter the Cave of Pianus " .. x .. " times per day.", 5);
		end
	else
		showMessage("The Cave of Pianus is currently full. Please come back later.", 5);
	end
end