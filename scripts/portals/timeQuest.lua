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
-- Portals throughout the Time Temple

map = getMap();
toMapIfDone = 0;
toMap = 0;
quest = 0;

-- Memory/green area
if map == 270010100 then
	quest = 3501;
	toMapIfDone = 270010110;
	toMap = 270010000;
elseif map == 270010200 then
	quest = 3502;
	toMapIfDone = 270010210;
	toMap = 270010110;
elseif map == 270010300 then
	quest = 3503;
	toMapIfDone = 270010310;
	toMap = 270010210;
elseif map == 270010400 then
	quest = 3504;
	toMapIfDone = 270010410;
	toMap = 270010310;
elseif map == 270010500 then
	quest = 3507;
	toMapIfDone = 270020000;
	toMap = 270010410;

-- Regrets/blue area
elseif map == 270020100 then
	quest = 3508;
	toMapIfDone = 270020110;
	toMap = 270020000;
elseif map == 270020200 then
	quest = 3509;
	toMapIfDone = 270020210;
	toMap = 270020110;
elseif map == 270020300 then
	quest = 3510;
	toMapIfDone = 270020310;
	toMap = 270020210;
elseif map == 270020400 then
	quest = 3511;
	toMapIfDone = 270020410;
	toMap = 270020310;
elseif map == 270020500 then
	quest = 3514;
	toMapIfDone = 270030000;
	toMap = 270020410;

-- Oblivion/red area
elseif map == 270030100 then
	quest = 3515;
	toMapIfDone = 270030110;
	toMap = 270030100;
elseif map == 270030200 then
	quest = 3516;
	toMapIfDone = 270030210;
	toMap = 270030210;
elseif map == 270030300 then
	quest = 3517;
	toMapIfDone = 270030310;
	toMap = 270030310;
elseif map == 270030400 then
	quest = 3518;
	toMapIfDone = 270030410;
	toMap = 270030410;
elseif map == 270030500 then
	quest = 3521;
	toMapIfDone = 270040000;
	toMap = 270030410;

-- Deep palace
elseif map == 270040000 then
	quest = 3522;
	toMapIfDone = 270040100;
	toMap = 270040000;
end

if isQuestCompleted(quest) then
	playPortalSe();
	setMap(toMapIfDone, "out00");
else
	showMessage("Those who have not received permission cannot walk against the flow of the temple and will return to the previous place.", m_red);
	playPortalSe();
	setMap(toMap);
end