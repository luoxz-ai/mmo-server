DROP TABLE IF EXISTS `tbld_dbinfo`;
CREATE TABLE `tbld_dbinfo` (
  `worldid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `db_ip` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql地址',
  `db_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏数据mysql端口',
  `db_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql表名',
  `db_user` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql用户',
  `db_passwd` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql密码',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_gmlist`;
CREATE TABLE `tbld_gmlist` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `openid` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'openid',
  `gmlev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'gm等级',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_serverlist`;
CREATE TABLE `tbld_serverlist` (
  `worldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `server_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '服务器名',
  `open_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '开服时间戳',
  `open_status` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '开放状态',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '入口地址',
  `mergeto` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '已经被合服到哪个服了',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_servicedetail`;
CREATE TABLE `tbld_servicedetail` (
  `worldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `service_type` tinyint(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务类型',
  `service_idx` tinyint(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务编号',
  `lib_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'lib库名',
  `route_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '内网通讯地址',
  `route_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '内网通讯端口',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '外网地址',
  `publish_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '外网端口',
  `debug_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT 'debug端口',
  `bind_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '监听绑定地址',
  PRIMARY KEY (`worldid`,`service_type`,`service_idx`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_globalvar`;
CREATE TABLE `tbld_globalvar` (
  `key` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'key',
  `value0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value_str0` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str1` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str2` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str3` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_guild`;
CREATE TABLE `tbld_guild` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `lev` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '帮派等级',
  `name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '帮派名称',
  `leader_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '帮主id',
  `leader_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '帮主名称',
  `creator_id` bigint(32) unsigned NOT NULL DEFAULT '0' COMMENT '创建人ID',
  `creator_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '创建人名称',
  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创帮时间',
  `del_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '删除时间',
  `slogan` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '口号',
  `info1` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息',
  `info2` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息',
  `info3` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息',
  `flag` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '设置掩码',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_guild_memberinfo`;
CREATE TABLE `tbld_guild_memberinfo` (
  `id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'id',
  `guildid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'guildid',
  `rank` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派职位',
  `score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献',
  `total_score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献累计',
  `join_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '入帮时间',
  `level` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '等级',
  `name` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '名字',
  `last_login_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间',
  `last_logout_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


