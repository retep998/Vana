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
-- Maple leaf bubble thing on top of guild HQ

if isQuestActive(6230) and getItemAmount(4031456) == 0 and getItemAmount(4031476) > 0 then
	giveItem(4031476, -1);
	if not giveItem(4031456, 1) then
		addText("Maple Marble can't be earned as there's no blank on Others window. Make a blank and try again.");
	else
		addText("Maple leaves were absorbed into sparkling glass marble.");
	end
	sendOK();
end