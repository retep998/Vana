if state == 0 then
	addText("Take this ship and you'll head off to a bigger continent.#e For 150 mesos#n, I'll take you to #bVictoria Island#k");
	addText(". The thing is, once you leave this place, you can't ever come back. What do you think? Do you want to go to Victoria Island?");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 1 then
		addText("Bored of this place? Here... Give me 150 mesos first...");
		sendNext();
	else
		addText("Hmm... I guess you still have things to do here?");
		sendOK();
	end
elseif state == 2 then
	if getSelected() == 1 then
		if getLevel() >= 7 then
			if getMesos() >= 150 then
				giveMesos(-150);
				addText("Awesome! #e150 mesos#n accepted! Alright, off to Victoria Island!");
				sendNext();
			else
				addText("What? You're telling me you wanted to go without any money? You're one weirdo...");
				sendOK();
				endNPC();
			end
		else
			addText("Let's see... I don't think you are strong enough. You'll have to be at least #bLevel 7#k ");
			addText("to go to Victoria Island.");
			sendOK();
			endNPC();
		end
	else
		endNPC();
	end
elseif state == 3 then
	if getSelected() == 1 then
		setMap(104000000);
	end
	endNPC();
end