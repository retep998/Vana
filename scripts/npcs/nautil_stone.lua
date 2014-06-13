--[[
Copyright (C) 2008-2014 Vana Development Team

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
-- Shiny Stone

dofile("scripts/utils/npcHelper.lua");

if not isQuestActive(2166) or getQuestData(2166) ~= nil then
	addText("It's a beautiful, shiny rock. ");
	addText("I can feel the mysterious power surrounding it.");
	sendNext();
else
	addText("I touched the shiny rock with my hand, and felt a mysterious power flowing into my body.");
	sendNext();
	setQuestData(2166, "5");
end