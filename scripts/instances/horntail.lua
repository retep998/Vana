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
	addInstanceMap(240060000);
	addInstanceMap(240060100);
	addInstanceMap(240060200);
end

function playerDisconnect(playerId, isPartyLeader)
	cleanUpHorntail();
end

function timerEnd(name, fromTimer)
	if fromTimer then
		if name == "delayedMarkForDelete" then
			cleanUpZakum();
		end
	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	b = isInstanceMap(newMap);
	if not b then
		removeInstancePlayer(playerId);
		cleanUpHorntail();
	elseif b then
		if setPlayer(playerId) then
			gm = isGm();
			gmInstance = getInstanceVariable("gm", type_bool);
			if gm == gmInstance then
				addInstancePlayer(playerId);
				stopInstanceTimer("delayedMarkForDelete");
			end
			revertPlayer();
		end
	end
end

function cleanUpHorntail()
	if getInstancePlayerCount() == 0 then
		instanceDelay = nil;
		if setInstance("horntailSignup") then
			instanceDelay = getInstanceTime();
			revertInstance();
		end
		if instanceDelay then
			startInstanceTimer("delayedMarkForDelete", instanceDelay + 5);
			return;
		end

		markForDelete();
	end
end

function instanceTimerEnd(fromTimer)
	moveAllPlayers(240050400, "st00");
	removeAllInstancePlayers();
	markForDelete();
end