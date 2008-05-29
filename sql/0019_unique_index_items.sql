ALTER TABLE items DROP column id;
ALTER TABLE items DROP INDEX charid;
ALTER TABLE items CHANGE itemid itemid int not null;
ALTER TABLE items CHANGE charid charid int not null;
ALTER TABLE items CHANGE inv inv tinyint(4) not null;
ALTER TABLE items CHANGE pos pos smallint(6) not null;
ALTER TABLE items CHANGE amount amount smallint(6) not null default 1;
ALTER TABLE items ADD UNIQUE INDEX (charid,inv,pos);
