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
-- Nine Spirits Egg portal

if getItemAmount(4001094) > 0 then
	if not isInstance("horntailEgg") then
		createInstance("horntailEgg", 5 * 60, true);
		addInstancePlayer(getID());
		playPortalSE();
		setMap(240040611);
	else
		showMessage("Someone else is already inside in an attempt to complete the quest. Please try again later.", m_red);
	end
else
	showMessage("In order to enter the premise, you'll need to have the Nine Spirit's Egg in possession..", m_red);
end