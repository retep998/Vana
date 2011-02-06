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
-- Hak (Herb Town, Mu Lung, Orbis)

if getMap() == 251000000 then
	addText("Hello there? I'm the crane that flies from #bOrbis#k to #bMu Lung#k and back. I fly around all the time, so I figured, why not make some money by taking travelers like you along for a small fee? It's good business for me. Anyway, what do you think? Do you want to fly to #bMu Lung#k right now? I only charge #b1500 mesos#k.");
	yes = askYesNo();
	
	if yes == 1 then
		if giveMesos(-1500) then
			setMap(250000100);
		else
			addText("Are you sure you have enough mesos?");
			sendNext();
		end
	else
		addText("OK. If you ever change your mind, please let me know.");
		sendNext();
	end
elseif getMap() == 250000100 then
	addText("Hello there? I'm the crane that flies from #bMu Lung#k to #bOrbis#k and back. I fly around all the time, so I figured, why not make some money by taking travelers like you along for a small fee? It's good business for me. Anyway, what do you think? \r\n");
	addText("#b#L0# Orbis(6000 mesos)#l\r\n");
	addText("#L1# Herb Town(1500 mesos)#l");
	what = askChoice();
	
	if what == 0 then
		if isInstance("hakToOrbis") then
			addText("Someone else is on the way to Orbis right now. Talk to me in a little bit.");
			sendNext();
		else
			if giveMesos(-6000) then
				createInstance("hakToOrbis", 60, true);
				addInstancePlayer(getID());
				setMap(200090310);
			else
				addText("Are you sure you have enough mesos?");
				sendNext();
			end
		end
	elseif what == 1 then
		addText("Will you move to #b#m251000000##k now? If you have #b1500mesos#k, I'll take you there right now.");
		yes = askYesNo();
		
		if yes == 1 then
			if giveMesos(-1500) then
				setMap(251000000);
			else
				addText("Are you sure you have enough mesos?");
				sendNext();
			end
		else
			addText("OK. If you ever change your mind, please let me know.");
			sendNext();
		end
	end
elseif getMap() == 200000141 then
	addText("Hello there? I'm the crane that flies from #bOrbis#k to #bMu Lung#k and back. I fly around all the time, so I figured, why not make some money by taking travelers like you along for a small fee? It's good business for me. Anyway, what do you think? \r\n");
	addText("#b#L0# Mu Lung(6000 mesos)#l");
	what = askChoice();
	
	if what == 0 then
		if isInstance("hakToMuLung") then
			addText("Someone else is on the way to Mu Lung right now. Talk to me in a little bit.");
			sendNext();
		else
			if giveMesos(-6000) then
				createInstance("hakToMuLung", 60, true);
				addInstancePlayer(getID());
				setMap(200090300);
			else
				addText("Are you sure you have enough mesos?");
				sendNext();
			end
		end
	end
end