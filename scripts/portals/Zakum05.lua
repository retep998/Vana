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
-- Door to Zakum

dofile("scripts/utils/bossHelper.lua");

questStatus = getPlayerVariable("zakum_quest_status", type_int);

if not isGm() then
	if questStatus == nil or questStatus < 3 or getItemAmount(4001017) < 1 then
		showMessage("You may only enter this place after clearing level 3. You'll also need to have the Eye of Fire in possession.", msg_red);
		return;
	end
	if not isZakumChannel() then
		channels = getZakumChannels();
		if #channels == 0 then
			showMessage("You may not enter the altar of Zakum at this time.", msg_red);
		else
			showMessage("You can only enter the altar of Zakum on " .. getChannelString(channels) .. ".", msg_red);
		end
		return;
	end
	if getReactorState(211042300, 2118002) == 1 then
		showMessage("The battle against the boss has already begun, so you can't go in.", msg_red);
		return;
	end
end

x = getMaxZakumBattles();
if not isGm() and x == 0 then
	showMessage("You may not enter the altar of Zakum at this time.", msg_red);
else
	if isGm() or getEntryCount("Zakum", x) < x then
		playPortalSe();
		setMap(211042400, "west00");
	else
		showMessage("You can only enter the altar of Zakum" .. x .. " " .. timeString(x) .. " per day.", msg_red);
	end
end