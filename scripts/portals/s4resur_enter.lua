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
-- Portal to Bishop Resurrection quest - dark part

if isQuestActive(6134) then
	if getItemAmount(4031448) > 0 then
		showMessage("You won over the darkness. Darkness doesn't admit you.", m_red);
	else
		if not isInstance("resurrectionDark4th") then
			createInstance("resurrectionDark4th", 20 * 60, true);
			addInstancePlayer(getID());
			playPortalSE();
			setMap(922020000);
		else
			showMessage("The door is locked. You can't enter.", m_red);
		end
	end
else
	showMessage("You feel fear over the door. You can't enter. The door is locked.", m_red);
end