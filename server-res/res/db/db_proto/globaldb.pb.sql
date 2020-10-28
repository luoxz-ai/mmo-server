DROP TABLE IF EXISTS `tbld_gmlist`;
CREATE TABLE `tbld_gmlist` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `openid` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'openid',
  `gmlev` int unsigned NOT NULL DEFAULT '0' COMMENT 'gm等级',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


DROP TABLE IF EXISTS `tbld_globalvar`;
CREATE TABLE `tbld_globalvar` (
  `key` int unsigned NOT NULL DEFAULT '0' COMMENT 'key',
  `value0` bigint unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value1` bigint unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value2` bigint unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value3` bigint unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value_str0` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str1` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str2` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str3` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'value_str',
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


DROP TABLE IF EXISTS `tbld_guild`;
CREATE TABLE `tbld_guild` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `lev` bigint unsigned NOT NULL DEFAULT '0' COMMENT '帮派等级',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '帮派名称',
  `leader_id` bigint unsigned NOT NULL DEFAULT '0' COMMENT '帮主id',
  `leader_name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '帮主名称',
  `creator_id` bigint unsigned NOT NULL DEFAULT '0' COMMENT '创建人ID',
  `creator_name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '创建人名称',
  `create_time` int unsigned NOT NULL DEFAULT '0' COMMENT '创帮时间',
  `del_time` int unsigned NOT NULL DEFAULT '0' COMMENT '删除时间',
  `slogan` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '口号',
  `info1` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '信息',
  `info2` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '信息',
  `info3` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '信息',
  `flag` bigint unsigned NOT NULL DEFAULT '0' COMMENT '设置掩码',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


DROP TABLE IF EXISTS `tbld_guild_memberinfo`;
CREATE TABLE `tbld_guild_memberinfo` (
  `id` bigint unsigned NOT NULL DEFAULT '0' COMMENT 'id',
  `guildid` bigint unsigned NOT NULL DEFAULT '0' COMMENT 'guildid',
  `rank` int unsigned NOT NULL DEFAULT '0' COMMENT '帮派职位',
  `score` int unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献',
  `total_score` int unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献累计',
  `join_time` int unsigned NOT NULL DEFAULT '0' COMMENT '入帮时间',
  `level` int unsigned NOT NULL DEFAULT '0' COMMENT '等级',
  `name` int unsigned NOT NULL DEFAULT '0' COMMENT '名字',
  `last_login_time` int unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间',
  `last_logout_time` int unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


