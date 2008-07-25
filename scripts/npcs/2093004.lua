-- Dolphin (Herb Town Pier)

if state == 0 then
	addText("Will you move to #b#m230000000##k now? The price is #b10000 mesos#k.");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 1 then
		if getMesos() >= 10000 then
			giveMesos(-10000);
			setMap(230000000);
		else
			addText("I don't think you have enough money...");
			sendNext();
		end
		endNPC();
	else
		addText("Hmmm ...too busy to do it right now? If you feel like doing it, though, come back and find me.");
		sendOK();
	end
else
	endNPC();
end
