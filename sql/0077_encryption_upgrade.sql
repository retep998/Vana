ALTER TABLE `users`
CHANGE `password` `password` CHAR(130) NOT NULL,
CHANGE `salt` `salt` CHAR(10) NULL