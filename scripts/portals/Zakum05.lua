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
-- Door to Zakum

dofile("scripts/lua_functions/bossHelper.lua");

if not isZakumChannel() then
	channels = getZakumChannels();
	if #channels == 0 then
		showMessage("You may not battle Zakum at this time.", 5);
	else
		showMessage("You may only battle Zakum on " .. getChannelString(channels) .. ".", 5);
	end
	return;
end

if getReactorState(211042300, 2118002) == 1 then
	showMessage("The battle with Zakum has already begun.", 5);
else
	x = getMaxZakumBattles();
	if x == 0 then
		showMessage("You may not battle Zakum at this time.", 5);
	else
		if getEntryCount("Zakum", x) < x then
			playPortalSE();
			setMap(211042400, "west00");
		else
			showMessage("You may only battle Zakum twice per day.", 5);
		end
	end
end