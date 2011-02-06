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
-- Platform Usher (Orbis)

addText("Orbis Station has lots of platforms available to choose from. You need to choose the one that'll take you to the destination of your choice. Which platform will you take?\r\n");
addText("#b#L0#The platform to the ship that heads to Ellinia#l\r\n");
addText("#b#L1#The platform to the ship that heads to Ludibrium#l#k\r\n");
addText("#b#L2#The platform to Hak that heads to Leafre#l\r\n");
addText("#b#L3#The platform to Hak that heads to Mu Lung#l\r\n");
addText("#b#L4#The platform to Geenie that heads to Ariant#l#k");
selected = askChoice();

addText("Even if you took the wrong passage you can get back here using the portal, so no worries.");
if selected == 0 then
	map = 200000110
	addText(" Will you move to the #bplatform to the ship that heads to Ellinia#k?");
elseif selected == 1 then
	map = 200000120
	addText(" Will you move to the #bplatform to the ship that heads to Ludibrium#k?");
elseif selected == 2 then
	map = 200000130
	addText("\r\n");
	addText("Will you move to the #bplatform to the ship that heads to Leafre#k?");
elseif selected == 3 then
	map = 200000140
	addText(" Will you move to the #bplatform to Hak that heads to Mu Lung#k?");
elseif selected == 4 then
	map = 200000150
	addText(" Will you move to the #bplatform to Geenie that heads to Ariant#k");
end
yes = askYesNo();

if yes == 1 then
	setMap(map, "west00");
else
	addText("Please make sure you know where you are going and then go to the platform through me.");
	if selected == 0 or selected == 1 or selected == 2 then
		addText(" The ride is on schedule so you better not miss it!");
	end		
	sendNext();
end