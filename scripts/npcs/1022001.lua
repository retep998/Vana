if getJob() == 0 then
	prices = {120, 100, 80, 100};
else
	prices = {1200, 1000, 800, 1000};
end

places = {"Lith Harbor", "Ellinia", "Kerning City", "Henesys"};
maps = {104000000, 101000000, 103000000, 100000000};

if state == 0 then
	addText("Hi! I drive the Regular Cab. If you want to go from town to town safely and fast, then ride our cab. We’ll gladly take you to your destination with an affordable price.");
	sendNext();
elseif state == 1 then
	if getJob() == 0 then
		addText("We have a special 90% discount for beginners. Choose your destination, for fees will change from place to place.#b\r\n#L1##bLith Harbor(120 mesos)#l\r\n#L2##bEllinia(100 mesos)#l\r\n#L3##bKerning City(80 mesos)#l\r\n#L4##bHenesys(100 mesos)#l#k");
		sendSimple();
	else
		addText("Choose your destination, for fees will change from place to place.#b\r\n#L1##bLith Harbor(1,200 mesos)\r\n#L2##bEllinia(1,000 mesos)\r\n#L3##bKerning City(800 mesos)\r\n#L4##bHenesys(1,000 mesos)#k");
		sendSimple();
	end
elseif state == 2 then
	where = getSelected();
	setVariable("where", where);
	if where > 0 then
		addText("You don’t have anything else to do here, huh? Do you really want to go to #b" .. places[where] .. "#k? It'll cost you #b" .. prices[where] .. " mesos#k.");
		sendYesNo();
	else
		endNPC();
	end
elseif state == 3 then
	if getSelected() == 1 then
		where = getVariable("where");
		if getMesos() >= prices[where] then
			giveMesos(-prices[where]);
			setMap(maps[where]);
			endNPC();
		else
			addText("You don't have enough mesos. Sorry to say this, but without them, you won't be able to ride this cab.");
			sendOK();
			endNPC();
		end
	else
		addText("There’s a lot to see in this town, too. Come back and find me when you need to go to a different town.");
		sendOK();
		endNPC();
	end
else
	endNPC();
end