-- Jeff - Ice Valley II
if state == 0 then
	if getLevel() >= 50 then
		addText("My name is Jeff. I can take you into the dungeon of El Nath, where even stronger monsters then out here can be found. Do you want to go?");
		sendYesNo();
	else
		addText("Sorry, you do not look strong enough to enter the El Nath dungeon.");
		sendOK();
		endNPC();
	end
elseif state == 1 then
	if getSelected() == 1 then
		endNPC();
		setMap(211040300);
	else
		addText("I can see why you wouldn't want to go in. If you do decide to enter, talk to me again.");
		sendOK();
		endNPC();
	end
end
