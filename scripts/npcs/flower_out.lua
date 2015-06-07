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
-- Crumbling Statue

dofile("scripts/utils/npcHelper.lua");

addText("Once I lay my hand on the statue, a strange light covers me and it feels like I am being sucked into somewhere else. ");
addText("Will it be okay to go back to Sleepywood?");
answer = askYesNo();

if answer == answer_no then
	addText("Once I took my hand off the statue it got quiet, as if nothing happened. ");
	sendNext();
else
	setMap(105040300);
end