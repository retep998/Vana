ALTER TABLE `users` DROP INDEX `username_password`, ADD INDEX ( `username` );
