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

function beginInstance()
	addInstanceMap(200090000);
	addInstanceMap(200090001);

	balrogCount = 0;
	if getRandomNumber(100) < 50 then balrogCount = balrogCount + 1; end
	if getRandomNumber(100) < 50 then balrogCount = balrogCount + 1; end
	
	if balrogCount > 0 then
		setInstanceVariable("balrog_count", balrogCount);
		startInstanceTimer("spawn_balrog", 60);
		startInstanceTimer("despawn_balrog", 9 * 60);
	end
end

function timerEnd(name, fromTimer)
	if fromTimer then
		if name == "spawn_balrog" then
			setBoatDocked(200090000, boat_docked);
			setMusic("Bgm04/ArabPirate", 200090000);
			for i = 1, getInstanceVariable("balrog_count", true) do
				spawnMobPos(200090000, 8150000, -590, -221);
			end
		elseif name == "despawn_balrog" then
			setBoatDocked(200090000, boat_undocked);
			setMusic("default", 200090000);
			clearMobs(200090000, false);
		end
	end
end

function instanceTimerEnd(fromTimer)
	if getInstancePlayerCount() > 0 then
   		moveAllPlayers(101000300);
		removeAllInstancePlayers();
  	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		removeInstancePlayer(playerId);
	elseif not isInstanceMap(oldMap) then
		addInstancePlayer(playerId);
	end
end