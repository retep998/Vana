-- phpMyAdmin SQL Dump
-- version 2.11.6
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jul 30, 2008 at 04:36 AM
-- Server version: 5.0.51
-- PHP Version: 5.2.6

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `maplestory`
--

-- --------------------------------------------------------

--
-- Table structure for table `storageitems`
--

CREATE TABLE IF NOT EXISTS `storageitems` (
  `userid` int(11) NOT NULL,
  `world_id` int(11) NOT NULL,
  `slot` smallint(6) NOT NULL,
  `itemid` int(11) NOT NULL,
  `amount` int(11) NOT NULL default '1',
  `type` tinyint(4) NOT NULL default '0',
  `slots` tinyint(4) NOT NULL default '7',
  `scrolls` tinyint(4) NOT NULL default '0',
  `istr` smallint(6) NOT NULL default '0',
  `idex` smallint(6) NOT NULL default '0',
  `iint` smallint(6) NOT NULL default '0',
  `iluk` smallint(6) NOT NULL default '0',
  `ihp` smallint(6) NOT NULL default '0',
  `imp` smallint(6) NOT NULL default '0',
  `iwatk` smallint(6) NOT NULL default '0',
  `imatk` smallint(6) NOT NULL default '0',
  `iwdef` smallint(6) NOT NULL default '0',
  `imdef` smallint(6) NOT NULL default '0',
  `iacc` smallint(6) NOT NULL default '0',
  `iavo` smallint(6) NOT NULL default '0',
  `ihand` smallint(6) NOT NULL default '0',
  `ispeed` smallint(6) NOT NULL default '0',
  `ijump` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`userid`,`world_id`,`slot`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
