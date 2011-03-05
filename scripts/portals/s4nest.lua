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
-- Portal to 4th job bowman bird nest

if isQuestActive(6241) or isQuestActive(6243) then
	if getItemAmount(4001113) > 0 or getItemAmount(4001114) > 0 then
		if not isInstance("bird4th") then
			createInstance("bird4th", 20 * 60, true);
			playPortalSE();
			addInstancePlayer(getID());
			setMap(924000100);
		else
			showMessage("Other characters are on request. You can't enter.", m_red);
		end
	else
		if isQuestActive(6241) then
			showMessage("You don't have Phoenix's Egg. You can't enter.", m_red);
		else
			showMessage("You don't have Freezer's Egg. You can't enter.", m_red);
		end
	end
else
	showMessage("You can't enter sealed place.", m_red);
end