ALTER TABLE `users` CHANGE `password` `password` CHAR( 45 ) NOT NULL;
ALTER TABLE `users` ADD `salt` CHAR( 5 ) NULL AFTER `password`;