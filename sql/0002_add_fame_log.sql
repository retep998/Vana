CREATE TABLE `fame_log` (
  `id` int(11) NOT NULL auto_increment,
  `from` int(11) NOT NULL,
  `to` int(11) NOT NULL,
  `time` datetime NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `from` (`from`,`to`,`time`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
