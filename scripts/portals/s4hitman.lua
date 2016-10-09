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
-- Assassinate quest

dofile("scripts/utils/tableHelper.lua");

if isQuestActive(6201) then
	if getItemAmount(4031452) == 0 then
		if not isInstance("assassinate4th") then
			createInstance("assassinate4th", 20 * 60, true);
			addInstancePlayer(getId());
			playPortalSe();
			maps = {910200000, 910200001, 910200002};
			setMap(selectElement(maps));
		else
			showMessage("Other characters are on request. You can't enter.", msg_red);
		end
	else
		showMessage("Shawn's request is completed. You don't need to go in again.", msg_red);
	end
else
	showMessage("You can't go. You didn't get Shawn's request.", msg_red);
end