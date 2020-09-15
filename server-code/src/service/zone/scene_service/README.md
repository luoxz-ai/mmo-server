#scene_service 游戏核心功能

* MapData地图数据
* GameMap不同编号的地图可以公用一个地图数据,但是拥有不同的刷怪数据

* 通过SceneManager 承载Scene
* 每个Scene有自己的SceneTree表示场景空间切分(格子,四叉,八叉)
* DynaScene代表副本,动态创建, 普通Scene静态创建
* 每个SceneService启动时,会根据SceneServiceid去读取哪些Scene需要被创建在当前SceneService
* Role/Monster/Player/Pet 都存在于Scene上

* 一个SceneService可以通过SceneManager挂载多个Scene
* 一个SceneService只有1条主逻辑线程
* 一个SceneService会有一条异步Loading线程
* 当角色需要被创建时放入异步Loading线程,完整读取出来后, 丢给主线程
* 一个SceneService会有自己的AI线程, 通过消息的模式,将场景/Monster/Player 的创建/删除/变化, 同步给AI线程, AI线程可以长时间高负载的进行索敌,寻路等工作而不影响SceneService主逻辑的处理
* 消息从MessageList中被读取出来后,判断是否是玩家的消息,如果是则根据VirtualSocket号存储,等待玩家处理(单个玩家,单帧只允许处理N条消息,避免因大量发送消息,导致其他玩家响应变慢)
* SceneService主逻辑循环 先处理网络消息, 然后处理定时器
* 怪物/场景/玩家 均使用各自的定时器EventManager来处理各种定时回调

* 怪物和玩家的位置信息, hp等信息,需要同步给AI服务或AI线程
* AI服务/AI线程将不停的轮询判断,避免造成主逻辑循环卡顿
* 如果是AI服务,则使用MessagePort通讯, 如果AI线程,则使用lockfree队列通信
* AI使用分层FSM来进行基本逻辑行为的状态的切换, AI的技能选择使用FuzzyLoigc进行模糊逻辑选择, 当需要特殊化的BossAI时,在FSM上引入lua脚本进行特化处理

* status状态/BUFF, 每个状态允许配置不同的属性, 同时将状态分为单次作用,多次作用,不作用
* 通过StatusFlag掩码模式来对每种状态进行切分
* 特殊的状态可以编写对应lua,来实现在特定时刻的特殊操作

* commondata 用来存放一些无法命名的不想单独建表的数据

* datacount 用来保存玩家的各种计数(比如使用物品多少次啊,击杀怪物多少次啊,等等)

* sysvar 用来保存一些系统变量, 且这些变量会在多个zone中同步(只保证最终数据一致性)

* mail邮箱

* mall商城

* item物品
** Item 所有和物品相关的操作
** ItemType 物品配置数据
** ItemData 物品自身数据
** ItemAddition物品强化属性 
** ItemFormula 物品打造合成配置(创建新物品)
** ItemUpgrade 物品升级配置(变更物品类型)
** Equipment装备槽,
** SuitEquip 套装配置
** Package 包裹
** StoragePackage 仓库

* skill 技能系统




* friend 社交系统

* drop 掉落相关





