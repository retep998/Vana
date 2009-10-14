/*
SQLyog Community Edition- MySQL GUI v8.15 RC
MySQL - 5.1.36-community-log : Database - ms_guilds
*********************************************************************
*/


/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
/*Table structure for table `alliances` */

DROP TABLE IF EXISTS `alliances`;

CREATE TABLE `alliances` (
  `id` int(16) NOT NULL AUTO_INCREMENT,
  `leader` int(16) NOT NULL DEFAULT '0',
  `worldid` int(2) NOT NULL,
  `name` varchar(255) NOT NULL,
  `notice` text NOT NULL,
  `rank1title` varchar(12) DEFAULT 'Master',
  `rank2title` varchar(12) DEFAULT 'Jr.Master',
  `rank3title` varchar(12) DEFAULT 'Member',
  `rank4title` varchar(12) NOT NULL DEFAULT 'Member',
  `rank5title` varchar(12) NOT NULL DEFAULT 'Member',
  `capacity` int(3) DEFAULT '2',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;

/*Table structure for table `guild_bbs_replies` */

DROP TABLE IF EXISTS `guild_bbs_replies`;

CREATE TABLE `guild_bbs_replies` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `thread` int(10) NOT NULL,
  `user` int(10) NOT NULL,
  `time` datetime NOT NULL,
  `content` varchar(26) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

/*Table structure for table `guild_bbs_threads` */

DROP TABLE IF EXISTS `guild_bbs_threads`;

CREATE TABLE `guild_bbs_threads` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `user` int(10) NOT NULL,
  `guild` int(10) NOT NULL,
  `time` datetime NOT NULL,
  `icon` smallint(5) NOT NULL,
  `title` varchar(50) NOT NULL,
  `content` text NOT NULL,
  `listid` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=19 DEFAULT CHARSET=latin1;

/*Table structure for table `guilds` */

DROP TABLE IF EXISTS `guilds`;

CREATE TABLE `guilds` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `world` int(3) DEFAULT '0',
  `leaderid` int(10) unsigned NOT NULL DEFAULT '0',
  `gp` int(10) unsigned NOT NULL DEFAULT '0',
  `logo` int(10) unsigned DEFAULT '0',
  `logoColor` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(45) NOT NULL,
  `rank1title` varchar(45) NOT NULL DEFAULT 'Master',
  `rank2title` varchar(45) NOT NULL DEFAULT 'Jr.Master',
  `rank3title` varchar(45) NOT NULL DEFAULT 'Member',
  `rank4title` varchar(45) NOT NULL DEFAULT 'Member',
  `rank5title` varchar(45) NOT NULL DEFAULT 'Member',
  `capacity` int(3) unsigned NOT NULL DEFAULT '10',
  `logoBG` int(10) unsigned DEFAULT '0',
  `logoBGColor` smallint(5) unsigned NOT NULL DEFAULT '0',
  `notice` text,
  `alliance` int(16) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=latin1;

ALTER TABLE `characters` ADD column `guild` INT(20) DEFAULT '0' AFTER `time_level`;
ALTER TABLE `characters` ADD column `guildrank` INT(1) DEFAULT '5' AFTER `guild`;
ALTER TABLE `characters` ADD column `alliance` int(20) DEFAULT '0' NULL after `guildrank`;
ALTER TABLE `characters` ADD column `alliancerank` int(1) DEFAULT '5' NULL after `alliance`;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;