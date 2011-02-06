ALTER TABLE equip DROP COLUMN id;
ALTER TABLE equip DROP INDEX charid;
ALTER TABLE equip CHANGE charid charid int not null;
ALTER TABLE equip CHANGE pos pos smallint(6) not null;
ALTER TABLE equip ADD UNIQUE INDEX (charid,pos);
