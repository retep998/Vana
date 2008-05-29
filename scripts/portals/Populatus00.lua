-- Door to Origin of Clocktower
if getNumPlayers(220080001) < 12 and getReactorState(220080001, 2201004) == 0 then
	setPortalToId(220080001);
	setPortalTo("st00");
else
	showMessage("The battle with Papulatus has already begun, so you cannot enter this portal.", 5);
	setPortalToId(-1);
end
