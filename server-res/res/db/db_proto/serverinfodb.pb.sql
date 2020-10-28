DROP TABLE IF EXISTS `tbld_dbinfo`;
CREATE TABLE `tbld_dbinfo` (
  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `db_url` varchar(2048) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '游戏数据mysql地址',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


DROP TABLE IF EXISTS `tbld_serverlist`;
CREATE TABLE `tbld_serverlist` (
  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `server_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '服务器名',
  `open_time` bigint unsigned NOT NULL DEFAULT '0' COMMENT '开服时间戳',
  `open_status` int unsigned NOT NULL DEFAULT '0' COMMENT '开放状态',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '入口地址',
  `mergeto` smallint unsigned NOT NULL DEFAULT '0' COMMENT '已经被合服到哪个服了',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


DROP TABLE IF EXISTS `tbld_servicedetail`;
CREATE TABLE `tbld_servicedetail` (
  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `service_type` int unsigned NOT NULL DEFAULT '0' COMMENT '服务类型',
  `service_idx` int unsigned NOT NULL DEFAULT '0' COMMENT '服务编号',
  `lib_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'lib库名',
  `route_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '内网通讯地址',
  `route_port` int unsigned NOT NULL DEFAULT '0' COMMENT '内网通讯端口',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '外网地址',
  `publish_port` int unsigned NOT NULL DEFAULT '0' COMMENT '外网端口',
  `debug_port` int unsigned NOT NULL DEFAULT '0' COMMENT 'debug端口',
  `bind_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '监听绑定地址',
  PRIMARY KEY (`worldid`,`service_type`,`service_idx`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


