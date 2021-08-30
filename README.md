# OnlineGobang
联机五子棋

介绍
---

这是一个五子棋游戏，支持本地游戏和联机游戏。

五子棋程序的原型是 lovely_ckj 的博弈五子棋，在他本人的同意下，huidong 为其开发了联机模组。

由于联机模块较为简易，所以只支持同时进行一场比赛，后续加入联机模式的玩家将进入观战模式。

联机服务器使用 php 制作。

**游戏效果**

![截图](https://github.com/TeamHDJK/OnlineGobang/blob/main/screenshot/1.png)


安装教程
---

**客户端**

首先编译 OnlineGobang 项目，获取客户端程序，或者到 release 中下载。

编译的程序可以直接运行，可以进行本地游戏。

如果要进行联机游戏，则需要进行基本配置。

在 server.h 中可以设置全局变量 g_strServerName 的值，此变量用于指定服务器域名。

客户端程序中也可以自定义服务器域名。

配置服务器相关内容请继续往下看：

**服务器**

服务器端使用 php 编写，使用 mysql 存储游戏数据，相应的 php 源码在 server 目录下。

如果要启用服务器，首先要设置 server/database.php，在此文件中设置数据库的信息。

然后运行 server/install.php，自动安装数据库。

之后只需要调用 server/index.php 就可以进行联机游戏的相关操作，指令列表详见 server/index.php。

服务器配置完毕后，将服务器域名填写到 server.h 的 g_strServerName 中，这样每次游戏进入联机模式都会使用这个服务器域名。

如果是本地的服务器域名，它会像是 "http://localhost/server/" 这样的。

当然，也可以不在 server.h 中配置域名，因为客户端程序中也可以自定义服务器域名。

由于这是一个简易的联机版本，所以只支持同时进行一场游戏，后续加入联机模式的玩家将进入观战模式。

游戏教程
---

嗯……其实游戏操作蛮简单的。

游戏主界面按 A 进入联机模式，按 B 进入本地模式，按 T 进行服务器设置。

下棋时鼠标操作即可。




