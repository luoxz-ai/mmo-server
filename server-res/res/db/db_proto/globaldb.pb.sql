DROP TABLE IF EXISTS `tbld_dbinfo`;
CREATE TABLE `tbld_dbinfo` (
  `worldid` int(11) UNSIGNED  NOT NULL COMMENT '游戏服编号',
  `db_ip` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '游戏数据mysql地址',
  `db_port` int(11) UNSIGNED  NOT NULL COMMENT '游戏数据mysql端口',
  `db_name` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '游戏数据mysql表名',
  `db_user` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '游戏数据mysql用户',
  `db_passwd` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '游戏数据mysql密码',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_globalvar`;
CREATE TABLE `tbld_globalvar` (
  `key` int(11) UNSIGNED  NOT NULL COMMENT 'key',
  `value0` bigint(20) UNSIGNED  NOT NULL COMMENT 'value',
  `value1` bigint(20) UNSIGNED  NOT NULL COMMENT 'value',
  `value2` bigint(20) UNSIGNED  NOT NULL COMMENT 'value',
  `value3` bigint(20) UNSIGNED  NOT NULL COMMENT 'value',
  `value_str0` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT 'value_str',
  `value_str1` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT 'value_str',
  `value_str2` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT 'value_str',
  `value_str3` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT 'value_str',
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_gmlist`;
CREATE TABLE `tbld_gmlist` (
  `id` bigint(20) UNSIGNED  NOT NULL AUTO_INCREMENT COMMENT '',
  `openid` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT 'openid',
  `gmlev` int(11) UNSIGNED  NOT NULL COMMENT 'gm等级',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_serverlist`;
CREATE TABLE `tbld_serverlist` (
  `worldid` int(11) UNSIGNED  NOT NULL COMMENT '游戏服编号',
  `server_name` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '服务器名',
  `open_time` bigint(20) UNSIGNED  NOT NULL COMMENT '开服时间戳',
  `open_status` int(11) UNSIGNED  NOT NULL COMMENT '开放状态',
  `publish_addr` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '入口地址',
  `mergeto` int(11) UNSIGNED  NOT NULL COMMENT '已经被合服到哪个服了',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_servicedetail`;
CREATE TABLE `tbld_servicedetail` (
  `worldid` int(11) UNSIGNED  NOT NULL COMMENT '游戏服编号',
  `serviceid` int(11) UNSIGNED  NOT NULL COMMENT '服务编号',
  `service_type` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT 'lib库名',
  `route_addr` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '内网通讯地址',
  `route_port` int(11) UNSIGNED  NOT NULL COMMENT '内网通讯端口',
  `publish_addr` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '外网地址',
  `publish_port` int(11) UNSIGNED  NOT NULL COMMENT '外网端口',
  `debug_port` int(11) UNSIGNED  NOT NULL COMMENT 'debug端口',
  `bind_addr` varchar(255) CHARACTER SET utf8 NOT NULL COMMENT '监听绑定地址',
  PRIMARY KEY (`worldid`),
  UNIQUE KEY `idx_worldid`(`worldid`,`serviceid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


