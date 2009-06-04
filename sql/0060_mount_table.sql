CREATE TABLE `mounts` (
  `charid` int(11) NOT NULL,
  `mountid` int(11) NOT NULL,
  `exp` smallint(6) DEFAULT '0',
  `level` int(3) unsigned NOT NULL DEFAULT '1',
  `tiredness` int(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`charid`,`mountid`),
  KEY `mountid` (`mountid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

DELETE FROM `items` WHERE FLOOR(itemid / 10000) = 190;
