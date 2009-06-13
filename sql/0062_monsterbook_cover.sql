CREATE TABLE  `monsterbook` (
  `charid` int(11) NOT NULL,
  `cardid` int(11) NOT NULL,
  `level` int(1) DEFAULT '1'
);

ALTER TABLE `characters` ADD `monsterbookcover` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0 AFTER `fame_opos`;
