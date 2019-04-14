SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for clidatas
-- ----------------------------
DROP TABLE IF EXISTS clidatas;
CREATE TABLE clidatas (
  fd varchar(5) NOT NULL default ' ',
  IP varchar(15) NOT NULL default '0',
  port varchar(6) NOT NULL default '0',
  cpu_rate varchar(3),
  mem_use varchar(5),
  men_total varchar(5),
  PRIMARY KEY  (fd),
}ENGINE=MyISAM DEFAULT CHARSET=latin1;

