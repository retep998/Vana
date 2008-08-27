-- Mr. Smith - Item Creator

makeids = {1082003, 1082000, 1082004, 1082007, 1082008, 1082023, 1082009, 1082059};
makereqs = {
		{4000021, 15, 4011001, 1, 4031138, 1000},
		{4011001, 2, 4031138, 2000},
		{4000021, 40, 4011000, 2, 4031138, 5000},
		{4011001, 2, 4031138, 10000},
		{4011000, 3, 4003000, 15, 4031138, 20000},
		{4000021, 30, 4011001, 4, 4003000, 30, 4031138, 30000},
		{4000021, 50, 4011001, 5, 4003000, 40, 4031138, 40000},
		{4011001, 3, 4021007, 2, 4000030, 30, 4003000, 45, 4031138, 50000},
		{4011007, 1, 4011000, 8, 4011006, 2, 4000030, 50, 4003000, 50, 4031138, 70000}
	   };

upgradeids = {1082005, 1082006, 1082035, 1082036, 1082024, 1082025, 1082010, 1082011, 1082060, 1082061};
upgradereqs = {
		{1082007, 1, 4011001, 1, 4031138, 20000},
		{1082007, 1, 4011005, 2, 4031138, 25000},
		{1082008, 1, 4021006, 3, 4031138, 30000},
		{1082008, 1, 4021008, 1, 4031138, 40000},
		{1082023, 1, 4011003, 4, 4031138, 45000},
		{1082023, 1, 4021008, 2, 4031138, 50000},
		{1082009, 1, 4011002, 5, 4031138, 55000},
		{1082009, 1, 4011006, 4, 4031138, 60000},
		{1082059, 1, 4011002, 3, 4021005, 5, 4031138, 70000},
		{1082059, 1, 4021007, 2, 4021008, 2, 4031138, 80000}
	      };

materialids = {4003001, 4003001, 4003000};
materialamts = {1, 1, 15};
materialreqs = {
		{4000003, 10},
		{4000018, 5},
		{4011000, 1, 4011001, 1}
	       };

local function hasResources (requisites, multiplier)
	local has = true;
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #requisites - 1, 2 do
		if requisites[index] == 4031138 then -- Mesos are shown as item 4031138 (Sack of Money)
			if getMesos() < (requisites[index + 1] * multiplier) then
				has = false;
				break;
			end
		else
			if getItemAmount(requisites[index]) < (requisites[index + 1] * multiplier) then
				has = false;
				break;
			end
		end
	end
	return has;
end

local function takeResources (requisites, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1 ,#requisites - 1, 2 do
		if requisites[index] == 4031138 then
			giveMesos((-1 * multiplier * requisites[index + 1]));
		else
			giveItem(requisites[index], (-1 * multiplier * requisites[index + 1]));
		end
	end
end

if state == 0 then
	addText("I am Mr. Thunder's apprentice. He's gettin up there with age, and he isn't what he used to be...haha, oh crap, please don't tell him that I said that...Anyway...I can make various items specifically designed for the warriors, so what do you think? Wanna leave it up to me?");
	sendYesNo();
elseif state == 1 then
	what = getSelected();
	if what == 0 then
		addText("Sigh ... I'll definitely hear it from my boss if I don't make the norm today ... Oh well, that sucks.");
		sendNext();
		endNPC();
	else
		addText("Alright! The service fee will be reasonable so don't worry about it. What do you want to do?\r\n");
		addText("#b#L0# Make a glove#l\r\n");
		addText("#L1# Upgrade a glove#l\r\n");
		addText("#L2# Create materials#l");
		sendSimple();
	end
elseif state == 2 then
	where = getSelected();
	if where == 0 then
		addText("I'm the best glove-maker in this town!! Now...what kind of a glove do you want to make?\r\n");
		addText("#L0##b #t1082003##k(level limit : 10, warrior)#l\r\n");
		addText("#L1##b #t1082000##k(level limit : 15, warrior)#l\r\n");
		addText("#L2##b #t1082004##k(level limit : 20, warrior)#l\r\n");
		addText("#L3##b #t1082001##k(level limit : 25, warrior)#l\r\n");
		addText("#L4##b #t1082007##k(level limit : 30, warrior)#l\r\n");
		addText("#L5##b #t1082008##k(level limit : 35, warrior)#l\r\n");
		addText("#L6##b #t1082023##k(level limit : 40, warrior)#l\r\n");
		addText("#L7##b #t1082009##k(level limit : 50, warrior)#l\r\n");
		addText("#L8##b #t1082059##k(level limit : 60, warrior)#l");
		sendSimple();
	elseif where == 1 then
		addText("So you want to upgrade the glove? Ok, then. A word of warning, though: All the items that will be used for upgrading will be gone, but if you use an item that has been #rupgraded#k with a scroll, the effect will disappear when upgraded. Please take that into consideration when making the decision, ok?");
		sendNext();
	elseif where == 2 then
		addText("So you want to make some materials, huh? Okay...what kind of materials do you want to make?\r\n");
		addText("#L0##bMake #t4003001# with #t4000003##k#l\r\n");
		addText("#L1##bMake #t4003001# with #t4000018##k#l\r\n");
		addText("#L2##bMake #t4003000#s#k#l");
		sendSimple();
	end
elseif state == 3 then
	what = getSelected();
	if where == 0 then
		glove = makeids[what + 1];
		reqs = makereqs[what + 1];
		if what == 0 then
			addText("To make one #t1082003#, I need the following items. The level limit is 10 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4000021# 15 #t4000021#s\r\n");
			addText("#v4011001# #t4011001# \r\n");
			addText("#v4031138# 1000 mesos");
		elseif what == 1 then
			addText("To make one #t1082000#, I need the following items. The level limit is 15 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4011001# 2 #t4011001#s \r\n");
			addText("#v4031138# 2000 mesos");
		elseif what == 2 then
			addText("To make one #t1082004#, I need the following items. The level limit is 20 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4000021# 40 #t4000021#s \r\n");
			addText("#v4011000# 2 #t4011000#s \r\n");
			addText("#v4031138# 5000 mesos");
		elseif what == 3 then
			addText("To make one #t1082001#, I need the following items. The level limit is 25 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4011001# 2 #t4011001#s \r\n");
			addText("#v4031138# 10000 mesos");
		elseif what == 4 then
			addText("To make one #t1082007#, I need the following items. The level limit is 30 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4011000# 3 #t4011000#s \r\n");
			addText("#v4011001# 2 #t4011001#s \r\n");
			addText("#v4003000# 15 #t4003000#s \r\n");
			addText("#v4031138# 20000 mesos");
		elseif what == 5 then
			addText("To make one #t1082008#, I need the following items. The level limit is 35 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4000021# 30 #t4000021#s \r\n");
			addText("#v4011001#  4 #t4011001#s \r\n");
			addText("#v4003000# 30 #t4003000#s \r\n");
			addText("#v4031138# 30000 mesos");
		elseif what == 6 then
			addText("To make one #t1082023#, I need the following items. The level limit is 40 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4000021# 50 #t4000021#s \r\n");
			addText("#v4011001# 5 #t4011001#s \r\n");
			addText("#v4003000# 40 #t4003000#s \r\n");
			addText("#v4031138# 40000 mesos");
		elseif what == 7 then
			addText("To make one #t1082009#, I need the following items. The level limit is 50 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4011001# 3 #t4011001#s \r\n");
			addText("#v4021007# 2 #t4021007#s \r\n");
			addText("#v4000030# 30 #t4000030#s \r\n");
			addText("#v4003000# 45 #t4003000#s \r\n");
			addText("#v4031138# 50000 mesos");
		elseif what == 8 then
			addText("To make one #t1082059#, I need the following items. The level limit is 60 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v4011007# #t4011007# \r\n");
			addText("#v4011000# 8 #t4011000#s \r\n");
			addText("#v4011006# 2 #t4011006#s \r\n");
			addText("#v4000030# 50 #t4000030#s \r\n");
			addText("#v4003000# 50 #t4003000#s \r\n");
			addText("#v4031138# 70000 mesos");
		end
		sendYesNo();
	elseif where == 1 then
		addText("So~~ what kind of a glove do you want to upgrade and create?\r\n");
		addText("#L0##b #t1082005##k(level limit : 30, warrior)#l\r\n");
		addText("#L1##b #t1082006##k(level limit : 30, warrior)#l\r\n");
		addText("#L2##b #t1082035##k(level limit : 35, warrior)#l\r\n");
		addText("#L3##b #t1082036##k(level limit : 35, warrior)#l\r\n");
		addText("#L4##b #t1082024##k(level limit : 40, warrior)#l\r\n");
		addText("#L5##b #t1082025##k(level limit : 40, warrior)#l\r\n");
		addText("#L6##b #t1082010##k(level limit : 50, warrior)#l\r\n");
		addText("#L7##b #t1082011##k(level limit : 50, warrior)#l\r\n");
		addText("#L8##b #t1082060##k(level limit : 60, warrior)#l\r\n");
		addText("#L9##b #t1082061##k(level limit : 60, warrior)#l");
		sendSimple();
	elseif where == 2 then
		material = materialids[what + 1];
		reqs = materialreqs[what + 1];
		amt = materialamts[what + 1];
		if what == 0 then
			addText("With #b10 #t4000003#es#k, I can make 1 #t4003001#(s). Be thankful, because this one's on me. What do you think? How many do you want?");
		elseif what == 1 then
			addText("With #b5 #t4000018#s#k, I can make 1 #t4003001#(s). Be thankful, because this one's on me. What do you think? How many do you want?");
		elseif what == 2 then
			addText("With #b1 #t4011001#(s) and #t4011000#(s) each#k, I can make 15 #t4003000#s. Be thankful, because this one's on me. What do you think? How many do you want?");
		end
		sendGetNumber(0, 0, 100);
	end
elseif state == 4 then
	if where == 0 then
		extra = getSelected();
		if extra == 0 then
			addText("Lacking the materials? It's ok ... collect them all and then come find me, alright? I'll be waiting...");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(1, glove, 1)) then
				addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
				sendNext();
			else
				addText("Here! take the #t" .. glove .. "#. Don't you think I'm as good as Mr. Thunder? You'll be more than satisfied with what I made here.");
				sendNext();
				takeResources(reqs);
				giveItem(glove, 1);
			end
		end
		endNPC();
	elseif where == 1 then
		glove = upgradeids[what + 1];
		reqs = upgradereqs[what + 1];
		if what == 0 then
			addText("To make one #t1082005#, I need the following items. The level limit is 30 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082007# #t1082007# \r\n");
			addText("#v4011001# #t4011001# \r\n");
			addText("#v4031138# 20000 mesos");
		elseif what == 1 then
			addText("To make one #t1082006#, I need the following items. The level limit is 30 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082007# #t1082007# \r\n");
			addText("#v4011005# 2 #t4011005#s \r\n");
			addText("#v4031138# 25000 mesos");
		elseif what == 2 then
			addText("To make one #t1082035#, I need the following items. The level limit is 35 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082008# #t1082008# \r\n");
			addText("#v4021006# 3 #t4021006#s \r\n");
			addText("#v4031138# 30000 mesos");
		elseif what == 3 then
			addText("To make one #t1082036#, I need the following items. The level limit is 35 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082008# #t1082008# \r\n");
			addText("#v4021008# #t4021008# \r\n");
			addText("#v4031138# 40000 mesos");
		elseif what == 4 then
			addText("To make one #t1082024#, I need the following items. The level limit is 40 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082023# #t1082023# \r\n");
			addText("#v4011003# 4 #t4011003#s \r\n");
			addText("#v4031138# 45000 mesos");
		elseif what == 5 then
			addText("To make one #t1082025#, I need the following items. The level limit is 40 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082023# #t1082023# \r\n");
			addText("#v4021008# 2 #t4021008#s \r\n");
			addText("#v4031138# 50000 mesos");
		elseif what == 6 then
			addText("To make one #t1082010#, I need the following items. The level limit is 50 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082009# #t1082009# \r\n");
			addText("#v4011002# 5 #t4011002#s \r\n");
			addText("#v4031138# 55000 mesos");
		elseif what == 7 then
			addText("To make one #t1082011#, I need the following items. The level limit is 50 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082009# #t1082009# \r\n");
			addText("#v4011006# 4 #t4011006#s \r\n");
			addText("#v4031138# 60000 mesos");
		elseif what == 8 then
			addText("To make one #t1082060#, I need the following items. The level limit is 60 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082059# #t1082059# \r\n");
			addText("#v4011002# 3 #t4011002#s \r\n");
			addText("#v4021005# 5 #t4021005#s \r\n");
			addText("#v4031138# 70000 mesos");
		elseif what == 9 then
			addText("To make one #t1082061#, I need the following items. The level limit is 60 and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
			addText("#b#v1082059# #t1082059# \r\n");
			addText("#v4021007# 2 #t4021007#s \r\n");
			addText("#v4021008# 2 #t4021008#s \r\n");
			addText("#v4031138# 80000 mesos");
		end
		sendYesNo();
	elseif where == 2 then
		extra = getNumber();
		-- Output for this step is all messed up, thank Nexon
		if what == 0 then
			addText("You want to make #b#t4003001#(s)#k " .. extra .. " time(s)? I'll need  #r" .. reqs[2] * extra .. " #t4000003#es#k then.");
		elseif what == 1 then
			addText("You want to make #b#t4003001#(s)#k " .. extra .. " time(s)? I'll need  #r" .. reqs[2] * extra .. " #t4000018#s#k then.");
		elseif what == 2 then
			addText("You want to make #b#t4003000#s#k " .. extra .. " time(s)? I'll need  #r" .. reqs[2] * extra .. " #t4011001#(s) and #t4011000#(s) each#k then.");
		end
		sendYesNo();
	end
elseif state == 5 then
	if where == 1 then
		extra = getSelected();
		if extra == 0 then
			addText("Lacking the materials? It's ok ... collect them all and then come find me, alright? I'll be waiting...");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(1, glove, 1)) then
				addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
				sendNext();
			else
				addText("Here! take the #t" .. glove .. "#. Don't you think I'm as good as Mr. Thunder? You'll be more than satisfied with what I made here.");
				sendNext();
				takeResources(reqs);
				giveItem(glove, 1);
			end
		end
		endNPC();
	elseif where == 2 then
		if getSelected() == 0 then
			addText("Lacking the materials? It's all good...collect them all and then come find me, alright? I'll be waiting...");
			sendNext();
		else
			if extra == 0 or (not hasResources(reqs)) or (not hasOpenSlotsFor(4, material, amt)) then
				addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
				sendNext();
			else
				addText("Here! take " .. amt * extra  .. " #t" .. material .. "#(s). Don't you think I'm as good as Mr. Thunder? You'll be more than satisfied with what I made here.");
				sendNext();
				takeResources(reqs, extra);
				giveItem(material, amt * extra);
			end
		end
		endNPC();
	end
end