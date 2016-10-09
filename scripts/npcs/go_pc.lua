--[[
Copyright (C) 2008-2016 Vana Development Team

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
-- Mong from Kong (Kerning City)

dofile("scripts/utils/npcHelper.lua");

addText("Aren't you connected through the Internet Cafe? ");
addText("If so, then go in here ... you'll probably head to a familiar place. ");
addText("What do you think? Do you want to go in?");
answer = askYesNo();

if answer == answer_yes then
	addText("Hey, hey ... I don't think you're logging on from the internet cafe. ");
	addText("You can't enter this place if you are logging on from home ...");
else
	addText("You must be busy, huh? ");
	addText("But if you're loggin on from the internet cafe, then you should try going in. ");
	addText("You may end up in a strange place once inside.");
end
sendNext();