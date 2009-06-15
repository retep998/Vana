CREATE TABLE  `active_quests` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `charid` int(11) NOT NULL,
  `questid` smallint(16) NOT NULL,
  `mobid` int(11) NOT NULL DEFAULT 0,
  `mobskilled` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
);

CREATE TABLE  `completed_quests` (
  `charid` int(11) NOT NULL,
  `questid` smallint(6) NOT NULL,
  `endtime` bigint(20) DEFAULT '0',
  PRIMARY KEY (`charid`, `questid`)
);
