CREATE TABLE storage (userid INT NOT NULL, world_id INT NOT NULL, slots SMALLINT NOT NULL DEFAULT 4, mesos INT NOT NULL DEFAULT 0, PRIMARY KEY (userid, world_id));
