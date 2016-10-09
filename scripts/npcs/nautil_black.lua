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
-- Muirhat

dofile("scripts/utils/npcHelper.lua");

if not isQuestActive(2175) then
	addText("The Black Magician and his followers. ");
	addText("Kyrin and the crew of the Nautilus. ");
	addText("They'll be chasing one another until one of them doesn't exist, that's for sure.");
	sendOk();
else
	addText("Are you ready? ");
	addText("Good, I'll send you to where the disciples of the Black Magician are. ");
	addText("Look for the pigs around the area where I'll be sending you. ");
	addText("You'll be able to find it by tracking them.");
	sendNext();

	addText("When they are weakened, they'll change back to their original state. ");
	addText("If you find something suspicious, you must fight them until they are weak. ");
	addText("I'll be here awaiting your findings.");
	sendBackNext();

	setMap(912000000);
end