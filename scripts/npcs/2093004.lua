-- Dolphin (Herb Town Pier)
--By Strife of KStory

if state == 0 then
	addText("Will you move to #bAquarium#k now? The price is #b10000 mesos#k");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 1 then
		if getMesos() >= 10000 then
			giveMesos(-10000);
			setMap(230000000);
			endNPC();
		else
			addText("I don't think you have enough money...");
			sendOK();
			endNPC();
		end
	else
		addText("Hmmm ...too busy to do it right now? If you feel like ");
		addText("doing it, though, come back and find me.");
		sendOK();
		endNPC();
	end
else
	endNPC();
end
