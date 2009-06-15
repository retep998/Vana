ALTER TABLE `teleport_rock_locations` CHANGE `characterid` `charid` int(11) NOT NULL;
ALTER TABLE `monsterbook` ADD CONSTRAINT PRIMARY KEY (`charid`, `cardid`);