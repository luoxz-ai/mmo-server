/*
Navicat MySQL Data Transfer

Source Server         : 192.168.141.195
Source Server Version : 50722
Source Host           : 192.168.141.195:3306
Source Database       : global

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-05-18 14:25:07
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for tbld_dbinfo
-- ----------------------------
DROP TABLE IF EXISTS `tbld_dbinfo`;
CREATE TABLE `tbld_dbinfo` (
  `worldid` smallint(6) unsigned NOT NULL COMMENT '游戏服编号',
  `db_ip` varchar(255) NOT NULL COMMENT '游戏数据mysql地址',
  `db_port` int(11) unsigned NOT NULL COMMENT '游戏数据mysql端口',
  `db_name` varchar(255) NOT NULL COMMENT '游戏数据mysql表名',
  `db_user` varchar(255) NOT NULL COMMENT '游戏数据mysql用户',
  `db_passwd` varchar(255) NOT NULL COMMENT '游戏数据mysql密码',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for tbld_globalvar
-- ----------------------------
DROP TABLE IF EXISTS `tbld_globalvar`;
CREATE TABLE `tbld_globalvar` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `key` varchar(255) NOT NULL COMMENT 'key',
  `value` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value_str` varchar(1024) NOT NULL COMMENT 'value_str',
  PRIMARY KEY (`id`),
  UNIQUE KEY `key` (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for tbld_gmlist
-- ----------------------------
DROP TABLE IF EXISTS `tbld_gmlist`;
CREATE TABLE `tbld_gmlist` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `openid` varchar(255) NOT NULL COMMENT 'openid',
  `gmlev` int(11) unsigned NOT NULL COMMENT 'gm等级',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for tbld_serverlist
-- ----------------------------
DROP TABLE IF EXISTS `tbld_serverlist`;
CREATE TABLE `tbld_serverlist` (
  `worldid` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `server_name` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT '' COMMENT '服务器名',
  `open_time` bigint(20) NOT NULL DEFAULT '0' COMMENT '开服时间戳',
  `open_status` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '开放状态',
  `publish_addr` varchar(1024) NOT NULL COMMENT '入口地址',
  `mergeto` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '已经被合服到哪个服了',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for tbld_servicedetail
-- ----------------------------
DROP TABLE IF EXISTS `tbld_servicedetail`;
CREATE TABLE `tbld_servicedetail` (
  `worldid` smallint(6) unsigned NOT NULL COMMENT '服编号',
  `serviceid` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '服务编号',
  `service_type` varchar(255) NOT NULL COMMENT 'lib库名',
  `route_addr` varchar(255) NOT NULL COMMENT '内网通讯地址',
  `route_port` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '内网通讯端口',
  `publish_addr` varchar(255) NOT NULL COMMENT '外网地址',
  `publish_port` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '外网端口',
  `debug_port` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT 'debug端口',
  `bind_addr` varchar(255) NOT NULL DEFAULT '0.0.0.0' COMMENT '监听绑定地址',
  PRIMARY KEY (`worldid`,`serviceid`),
  UNIQUE KEY `worldid` (`worldid`,`serviceid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
