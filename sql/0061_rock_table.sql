CREATE TABLE `teleport_rock_locations` (
  `characterid` int(11) NOT NULL,
  `mapindex` tinyint(3) NOT NULL,
  `mapid` int(11) NOT NULL DEFAULT '999999999',
  PRIMARY KEY (`characterid`,`mapindex`)
);