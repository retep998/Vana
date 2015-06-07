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
-- Rain (Amherst)

dofile("scripts/utils/npcHelper.lua");

addText("This is the town called " .. blue(mapRef(1010000)) .. ", located at the northeast part of the Maple Island. ");
addText("You know that Maple Island is for beginners, right? ");
addText("I'm glad there are only weak monsters around this place.");
sendNext();

addText("If you want to get stronger, then go to " .. blue(mapRef(60000)) .. " where there's an harbor, ride on the gigantic ship and head to the place called " .. blue("Victoria Island") .. ". ");
addText("It's incomparable in size compared to this tiny island.");
sendBackNext();

addText("At the Victoria Island you can choose your job. ");
addText("Is it called " .. blue(mapRef(102000000)) .. "...? ");
addText("I heard there's a bare, desolate town where warriors live. ");
addText("A highland...what kind of a place would that be?");
sendBackOk();