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
-- Puddle in Horntail's cave

if isQuestActive(6280) and getItemAmount(4031454) > 0 then
	if getItemAmount(4031455) > 0 then
		addText("You already have #bt4031455##k.");
		sendOK();
	else
		giveItem(4031454, -1);
		giveItem(4031455, 1);
		addText("Holy water was contained from spring.");
		sendOK();
	end
end