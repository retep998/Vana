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
-- Pig farm portal

if isQuestActive(2073) then
	if not isInstance("pigFarm") then
		createInstance("pigFarm", 10 * 60, true);
		addInstancePlayer(getID());
		playPortalSE();
		setMap(900000000);
	else
		showMessage("It seems like someone already has visited Yoota's Farm.", m_red);
	end
else
	showMessage("There's a door that'll lead me somewhere, but I can't seem to get in there.", m_red);
end