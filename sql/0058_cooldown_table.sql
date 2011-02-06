CREATE TABLE cooldowns (
  id int(11) NOT NULL auto_increment,
  charid int(11) default NULL,
  skillid int(11) default '0',
  timeleft smallint(6) default '0',
  PRIMARY KEY  (id),
  KEY charid (charid)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;