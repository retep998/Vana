-- Machine Apparatus (Origin of Clocktower)
if state == 0 then
	addText("Would you like to leave this place? I can take you somewhere safer.");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 1 then
		setMap(220080000);
	end
	endNPC();
end
