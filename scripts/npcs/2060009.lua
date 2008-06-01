-- Dolphin (Aquarium)
-- By Strife of KStory + Optimised/Cleaned up by Doyos of Vana

prices = {1000, 10000};
maps = {230030200, 251000100};
if state == 0 then
	addText("Oceans are all connected to each other. ");
	addText("Places you can't reach by foot can be easily reached oversea. ");
	addText("How about taking #bDolphin Taxi#k with us today?");
	addText("\r\n#L1#Go to #bThe Sharp Unknown#k after paying 1000 mesos.#l");
	addText("\r\n#L2#Go to #bHerb Town#k after paying 10000 mesos.#l");
	sendSimple();
elseif state == 1 then
	where = getSelected();
	if getMesos() >= prices[where] then -- Go to Sharp Unknown/Herb Town
		giveMesos(-prices[where]);
		setMap(maps[where]);
		endNPC();
	else
		addText("I don't think you have enough money...");
		sendOK();
		endNPC();
	end
else
	endNPC();
end
