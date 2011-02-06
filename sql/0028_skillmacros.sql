CREATE TABLE  `skillmacros` (
  `charid` int(11) NOT NULL auto_increment,
  `pos` int(10) unsigned NOT NULL,
  `name` varchar(255) NOT NULL,
  `shout` tinyint(1) unsigned NOT NULL,
  `skill1` int(11) default NULL,
  `skill2` int(11) default NULL,
  `skill3` int(11) default NULL,
  PRIMARY KEY  (`charid`,`pos`)
);
