DELETE FROM `buddylist`;
ALTER TABLE `buddylist` ADD COLUMN `groupname` VARCHAR(13) DEFAULT 'Default Group' NULL AFTER `name`;

CREATE TABLE `buddylist_pending` (
  `char_id` int(11) NOT NULL,
  `inviter_name` varchar(13) NOT NULL,
  `inviter_id` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;