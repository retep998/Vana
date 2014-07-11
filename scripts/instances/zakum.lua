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
	addInstanceMap(280030000);
end

function playerDisconnect(playerId, isPartyLeader)
	cleanUpZakum();
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
		cleanUpZakum();
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

function mobDeath(mobId, mapMobId, mapId)
	-- Expire Zakum timer and add a get out timer to this if holding becomes a problem
end

function mobSpawn(mobId, mapMobId, mapId)
	if mobId == 8800000 then
		setInstanceVariable("summoned", true);
	end
	-- Summon holding timer can expire here and this is where the actual Zakum timer would start
end

function cleanUpZakum()
	if getInstancePlayerCount() == 0 then
		instanceDelay = nil;
		if setInstance("zakumSignup") then
			instanceDelay = checkInstanceTimer(instance_timer);
			revertInstance();
		end
		if instanceDelay then
			startInstanceFutureTimer("delayedMarkForDelete", instanceDelay + 5);
			return;
		end
		if getInstanceVariable("summoned", type_bool) then
			setReactorState(211042300, 2118002, 0);
		end
		markForDelete();
	end
end