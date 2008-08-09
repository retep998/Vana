SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `pets`;
CREATE TABLE `pets` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `index` tinyint(3) NOT NULL default '-1',
  `name` varchar(12) NOT NULL,
  `type` int(11) NOT NULL default '5000000',
  `level` tinyint(3) NOT NULL default '1',
  `closeness` smallint(6) NOT NULL default '0',
  `fullness` tinyint(3) NOT NULL default '1',
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
