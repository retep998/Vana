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
-- Portal to Bishop Resurrection quest - cauldron thing

if isQuestActive(6132) then
	if getItemAmount(4161017) > 0 then
		showMessage("You can't enter as you already have Caron's Note.", m_red);
	else
		if not isInstance("resurrectionMix4th") then
			createInstance("resurrectionMix4th", 5 * 60, true);
			addInstancePlayer(getID());
			playPortalSE();
			setMap(923000100);
		else
			showMessage("You feel someone is out there in the cold cave.", m_red);
		end
	end
else
	showMessage("You can't enter because of cold energy from the cave.", m_red);
end