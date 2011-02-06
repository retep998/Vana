CREATE TABLE IF NOT EXISTS `cashshop_coupon_codes` (
  `serial` varchar(22) NOT NULL,
  `maplepoints` int(11) NOT NULL DEFAULT '0',
  `nxcredit` int(11) NOT NULL DEFAULT '0',
  `nxprepaid` int(11) NOT NULL DEFAULT '0',
  `mesos` int(11) NOT NULL DEFAULT '0',
  `used` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`serial`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `cashshop_coupon_item_rewards` (
  `serial` varchar(22) NOT NULL,
  `itemid` int(11) NOT NULL,
  `amount` int(11) NOT NULL DEFAULT '1',
  `days_usable` int(5) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `cashshop_limit_sell` (
  `serial` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `cashshop_modified_items` (
  `serial` int(11) NOT NULL,
  `discount_price` int(11) DEFAULT NULL,
  `mark` enum('sale','hot','event','new') DEFAULT NULL,
  `showup` tinyint(1) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `cashshop_sell_log` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `charid` int(11) NOT NULL,
  `packageid` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE IF NOT EXISTS `storage_cash` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `world_id` int(11) NOT NULL,
  `itemid` int(11) NOT NULL,
  `amount` int(3) NOT NULL DEFAULT '1',
  `from` varchar(13) NOT NULL DEFAULT '',
  `petid` int(11) NOT NULL DEFAULT '0',
  `expires` datetime NOT NULL DEFAULT '2079-01-01 00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE IF NOT EXISTS `character_wishlist` (
  `charid` int(11) NOT NULL,
  `serial` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `characters_change_name_requests` (
  `charid` int(11) NOT NULL,
  `name` varchar(13) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `characters_change_world_requests` (
  `charid` int(11) NOT NULL,
  `worldid` int(3) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `character_cashshop_gifts` (
  `charid` int(11) NOT NULL,
  `cashid` int(11) NOT NULL,
  `itemid` int(11) NOT NULL,
  `sender` varchar(13) NOT NULL,
  `message` varchar(73) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

RENAME TABLE `storageitems` TO `storage_items`;

ALTER TABLE `items` ADD `cashid` INT( 11 ) NOT NULL DEFAULT '0', ADD `expiration` DATETIME NULL DEFAULT '2079-01-01 00:00:00';
ALTER TABLE `storage` ADD `credit_nx` INT( 11 ) NOT NULL DEFAULT '0', ADD `prepaid_nx` INT( 11 ) NOT NULL DEFAULT '0', ADD `maplepoints` INT( 11 ) NOT NULL DEFAULT '0';
