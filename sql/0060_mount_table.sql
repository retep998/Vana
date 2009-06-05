CREATE TABLE `mounts` (
  `charid` int(11) NOT NULL,
  `mountid` int(11) NOT NULL,
  `exp` smallint(6) DEFAULT '0',
  `level` int(3) unsigned NOT NULL DEFAULT '1',
  `tiredness` int(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`charid`,`mountid`),
  KEY `mountid` (`mountid`)
);

INSERT INTO mounts (`charid`, `mountid`) SELECT `charid`, `itemid` FROM `items` WHERE FLOOR(items.itemid / 10000) = 190;
