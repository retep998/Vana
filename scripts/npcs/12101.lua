-- Rain (Amherst)
if state == 0 then
    addText("This is the town called #b#m1010000##k, located at the northeast part of the Maple Island. You know that Maple Island is for beginners, right? I'm glad there are only weak monsters around this place.");
    sendNext();
elseif state == 1 then
    addText("If you want to get stronger, then go to #b#m60000##k where there's an harbor, ride on the gigantic ship and head to the place called #bVictoria Island#k. It's incomparable in size compared to this tiny island.");
    sendBackNext();
elseif state == 2 then
    addText("At the Victoria Island you can choose your job. Is it called #b#m102000000##k...? I heard there's a bare, desolate town where warriors live. A highland...what kind of a place would that be?");
    sendBackOK();
else
    endNPC();
end
