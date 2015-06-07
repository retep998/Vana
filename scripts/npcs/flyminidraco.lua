--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- Corba, turns you into a dragon so you can fly to the Time Temple

dofile("scripts/utils/npcHelper.lua");

addText("If you had wings, I'm sure you could go there.  ");
addText("But, that alone won't be enough.  ");
addText("If you want to fly though the wind that's sharper than a blade, you'll need tough scales as well.  ");
addText("I'm the only Halfling left that knows the way back... ");
addText("If you want to go there, I can transform you.  ");
addText("No matter what you are, for this moment, you will become a " .. blue("Dragon") .. "...\r\n");
addText(blue(choiceRef("I want to become a dragon.")));
askChoice();

useItem(2210016);
setMap(200090500);