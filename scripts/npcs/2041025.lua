-- Machine Apparatus - Origin of Clocktower
if state == 0 then
	addText("Beep... beep... you can make you escape to a safer place through me. ");
	addText("Beep ... beep ... would you like to leave this place?");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 1 then
		if getNumPlayers(220080001) == 1 then
			setReactorsState(220080000, 2208001, 0);
			setReactorsState(220080000, 2208003, 0);
			setReactorsState(220080001, 2201004, 0);
		end
		setMap(220080000);
	end
	endNPC();
else
	endNPC();
end
