--[[
Copyright (C) 2008-2009 Vana Development Team

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
-- Portals throughout the Time Temple

map = getMap();
tomap = 0;

-- Memory/green area
if map == 270010100 then
	tomap = 270010110;
elseif map == 270010200 then
	tomap = 270010210;
elseif map == 270010300 then
	tomap = 270010310;
elseif map == 270010400 then
	tomap = 270010410;
elseif map == 270010500 then
	tomap = 270020000;

-- Regrets/blue area
elseif map == 270020100 then
	tomap = 270020110;
elseif map == 270020200 then
	tomap = 270020210;
elseif map == 270020300 then
	tomap = 270020310;
elseif map == 270020400 then
	tomap = 270020410;
elseif map == 270020500 then
	tomap = 270030000;

-- Oblivion/red area
elseif map == 270030100 then
	tomap = 270030110;
elseif map == 270030200 then
	tomap = 270030210;
elseif map == 270030300 then
	tomap = 270030310;
elseif map == 270030400 then
	tomap = 270030410;
elseif map == 270030500 then
	tomap = 270040000;

-- Deep palace
elseif map == 270040000 then
	tomap = 270040100;
end

if tomap ~= 0 then
	setMap(tomap, "out00");
end