-- Dolphin (Aquarium)

prices = {1000, 10000};
maps = {230030200, 251000100};

if state == 0 then
	addText("Oceans are all connected to each other. Places you can't reach by foot can be easily reached oversea. How about taking #bDolphin Taxi#k with us today? \r\n");
	addText("#L0# Go to #bThe Sharp Unknown#k after paying 1000mesos.#l\r\n");
	addText("#L1# Go to #bHerb Town#k after paying 10000mesos.#l");
	sendSimple();
elseif state == 1 then
	where = getSelected() + 1;
	if getMesos() >= prices[where] then -- Go to Sharp Unknown/Herb Town
		giveMesos(-prices[where]);
		setMap(maps[where]);
		endNPC();
	else
		addText("I don't think you have enough money...");
		sendNext();
	end
else
	endNPC();
end
