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
-- Berserk/Heaven's Hammer portal

if isQuestActive(6153) then
	if getItemAmount(4031471) > 0 then
		showMessage("Sayram already has shield.", m_red);
	else
		if getItemAmount(4031475) > 0 then
			if not isInstance("berserk4th") then
				createInstance("berserk4th", 20 * 60, true);
				addInstancePlayer(getID());
				playPortalSE();
				setMap(910500200, "in00");
			else
				showMessage("Other character is on the quest currently. Please try again later.", m_red);
			end
		else
			showMessage("To enter, you need a key to Forgotten Shrine.", m_red);
		end
	end
else
	showMessage("You can't enter sealed place.", m_red);
end