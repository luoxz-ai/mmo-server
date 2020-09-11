DROP TABLE IF EXISTS `tbld_commondata`;
CREATE TABLE `tbld_commondata` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据类型',
  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据编号',
  `data0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据1',
  `data1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据2',
  `data2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据3',
  `data3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据4',
  PRIMARY KEY (`id`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_cooldown`;
CREATE TABLE `tbld_cooldown` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD类型',
  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD编号',
  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD结束时间戳',
  PRIMARY KEY (`id`),
  KEY `idx_player_idx` (`playerid`,`keytype`,`keyidx`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_datacount`;
CREATE TABLE `tbld_datacount` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'data类型',
  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'data编号',
  `data_num` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '累计计数',
  `next_reset_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '下一次重置的时间戳',
  PRIMARY KEY (`id`),
  KEY `idx_player_idx` (`playerid`,`keytype`,`keyidx`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_global_mail`;
CREATE TABLE `tbld_global_mail` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `senderid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '发送者id',
  `sendername` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '发送者名字',
  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '发送时间戳',
  `flag` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '发送者id;',
  `subject` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件标题',
  `content` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件内容',
  `filter_type` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家过滤条件',
  `filter_data` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家过滤条件',
  `attach_data` blob COMMENT '附件数据json',
  PRIMARY KEY (`id`),
  KEY `idx_create_time` (`create_time`),
  KEY `idx_sendid` (`senderid`)
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


DROP TABLE IF EXISTS `tbld_item`;
CREATE TABLE `tbld_item` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `owner_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '所有者id',
  `itemtype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '道具类型编号',
  `position` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 103:邮箱 104:拍卖行',
  `grid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹位置',
  `flag` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '物品掩码',
  `pilenum` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '堆叠数量',
  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '过期时间戳秒',
  `dura` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '当前耐久',
  `dura_limit` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '耐久上限',
  `addition_lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '强化等级',
  `extra` blob COMMENT '额外信息',
  PRIMARY KEY (`id`),
  KEY `idx_owner` (`owner_id`),
  KEY `idx_itemtype` (`itemtype`),
  KEY `idx_owner_pack` (`owner_id`,`position`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_mail`;
CREATE TABLE `tbld_mail` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `ownerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id',
  `senderid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id',
  `sendername` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '发送者名字',
  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '发送时间戳',
  `flag` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '邮件标志',
  `subject` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件标题',
  `content` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件内容',
  PRIMARY KEY (`id`),
  KEY `idx_create_time` (`create_time`),
  KEY `idx_ownerid` (`ownerid`),
  KEY `idx_sendid` (`senderid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_mail_attachment`;
CREATE TABLE `tbld_mail_attachment` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `mailid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '邮件id',
  `attach_type` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '附件类型',
  `attach_data` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数量/具体道具实例ID',
  PRIMARY KEY (`id`),
  KEY `idx_mailid` (`mailid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_pet`;
CREATE TABLE `tbld_pet` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `ownerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '所有者玩家id',
  `pet_type` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '宠物类型',
  `hp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '当前血量',
  `exp` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '当前经验',
  PRIMARY KEY (`id`),
  KEY `idx_pet_type` (`pet_type`),
  KEY `idx_ownerid` (`ownerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_player`;
CREATE TABLE `tbld_player` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `worldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务器编号',
  `oriworldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建角色时的服务器编号',
  `openid` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '玩家账号',
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '玩家名字',
  `prof` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家职业',
  `baselook` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家基本外观',
  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间戳',
  `last_logintime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间戳',
  `last_logouttime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间戳',
  `del_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '删除时间戳',
  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家等级',
  `viplev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'vip等级',
  `record_sceneid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家当前所在的地图编号',
  `record_x` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图x',
  `record_y` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图y',
  `record_face` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图face',
  `home_sceneid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家回城点对应的地图编号',
  `home_x` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图x',
  `home_y` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图y',
  `home_face` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图face',
  `mate_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '伴侣ID',
  `mate_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '伴侣名字',
  `teamid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '队伍id',
  `guildid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '公会id',
  `exp` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家经验',
  `money` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '金币',
  `money_bind` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '绑定金币',
  `gold` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '元宝',
  `gold_bind` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '绑定元宝',
  `hp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'hp',
  `mp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'mp',
  `fp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'fp',
  `np` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'np',
  `pkval` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'pk值',
  `honor` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '荣誉值',
  `achipoint` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就值',
  `bag_size` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '背包大小',
  `stroge_size` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '仓库大小',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`),
  KEY `idx_lev` (`lev`),
  KEY `idx_del_time` (`del_time`),
  KEY `idx_openid` (`openid`),
  KEY `idx_prof` (`prof`),
  KEY `idx_last_logintime` (`last_logintime`),
  KEY `idx_last_logouttime` (`last_logouttime`),
  KEY `idx_worldid` (`worldid`),
  KEY `idx_create_time` (`create_time`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_player_guildinfo`;
CREATE TABLE `tbld_player_guildinfo` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `guildid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'guildid',
  `rank` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派职位',
  `score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献',
  `total_score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献累计',
  `join_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '入帮时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_skill`;
CREATE TABLE `tbld_skill` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `skilltype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '技能编号',
  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '技能等级',
  PRIMARY KEY (`id`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_task`;
CREATE TABLE `tbld_task` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `taskid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务id',
  `num0` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数1',
  `num1` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数2',
  `num2` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数3',
  `num3` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数4',
  `accept_userlev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时的等级',
  `accept_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时间戳',
  `finish_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '完成任务时间戳',
  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务过期时间戳',
  `daycount` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '本日完成次数',
  `daycount_max` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '本日最大可完成次数',
  `state` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务状态',
  PRIMARY KEY (`id`),
  KEY `idx_taskid` (`taskid`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_achievement`;
CREATE TABLE `tbld_achievement` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `achiid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就id',
  `take` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就id',
  PRIMARY KEY (`id`),
  KEY `idx_achiid` (`achiid`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_status`;
CREATE TABLE `tbld_status` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `typeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态类型编号',
  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态等级',
  `power` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据',
  `secs` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '持续时间',
  `times` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '作用次数',
  `laststamp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后一次作用的时间戳',
  `casterid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '施加此状态的角色id',
  `pause` tinyint(11) unsigned NOT NULL DEFAULT '0' COMMENT '是否暂停',
  `statusid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态等级',
  PRIMARY KEY (`id`),
  KEY `idx_statusid` (`typeid`,`lev`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `tbld_systemvar`;
CREATE TABLE `tbld_systemvar` (
  `keyidx` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '系统变量名',
  `data0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据1',
  `data1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据2',
  `data2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据3',
  `data3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据4',
  `str0` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字1',
  `str1` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字2',
  `str2` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字3',
  `str3` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字4',
  PRIMARY KEY (`keyidx`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


